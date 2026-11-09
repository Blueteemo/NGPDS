# Startup audio transient suppression

The unmodified v0.6.0 executable and the previous personal build both produced a near-full-scale launch transient in cold-start emulator captures with no cartridge or BIOS loaded. The legacy muted callback fills `0x80008000`; the stream begins with a zero-filled buffer. Starting playback therefore introduces an abrupt DC step. The SN76496 mixer also has a legacy output bias, so changing only the muted value can move the discontinuity to game entry.

This candidate keeps `Sound.s`, K2Audio and the DAC/PSG synthesis untouched. `StartupAudio.c` wraps the existing callback with a one-time linear ramp over the first 32768 emitted stereo frames (about 0.683 s at the unchanged 48000 Hz stream rate). It does not delay emulation or file access. After that finite interval every emitted sample and return count passes through unchanged. There is no permanent gain reduction, resampling, clock change, new DMA/timer, heap allocation or chip-state layout change. Direct-argv game launches can also hear this initial fade; it is limited to application launch and does not restart on menu close, reset or state load.

The wrapper is compiled as ARM code because the legacy assembly callback lacks the interworking symbol metadata needed for a direct Thumb call. Maxmod's callback interface and sample-count contract are unchanged. The public [Maxmod streaming documentation](https://maxmod.org/ref/tut/streaming.html) describes the callback contract and interleaved signed 16-bit examples.

## Validation

- 300 DS frames, 48000 Hz emulator output, untouched capture levels: previous personal build peak **32479**, candidate **513** (about **36 dB lower**); largest adjacent-sample step **17154 → 2**. The residual low-level startup transition is **not digital silence**. These values do not measure real DS analog output or prove the absence of a separate amplifier power-on click.
- Host ASan/UBSan checks compile the actual wrapper: exact gain progression, buffer guards, the ramp boundary, short/zero callback returns and unchanged post-ramp samples.
- A standalone ARM9 test uses original synthetic tone/noise/DAC inputs, no ROM or BIOS. After advancing beyond the ramp, all **35 callbacks** (muted, PSG rest, tones/noise, DAC; lengths 4/8/32/800/1600) preserve PCM bytes, post-render PSG state, return lengths and buffer guards. The harness runs in ARM mode to call legacy assembly entry points correctly.
- Emulated timer measurements give a median **14 bus ticks** of post-ramp wrapper overhead per call, with interrupt-related outliers retained in the raw results. This is not a physical performance measurement; no new production timer is used.
- Four short Pure/Cure JP/ZH runs retain the selected RGB5 frames from the preceding menu build and have active audio output. This is output-presence/functional regression, not a music-fidelity claim.
- Menu-dismissal input, quiet automatic loading/error feedback, the empty-HLE startup guard, UTF-8 browser and old state import/resave were rerun. Persistence, CPU and graphics implementations remain unchanged.
- A clean rebuild matches the tested executable. Six existing warnings remain: two unused functions (`checkTimeOut`, `updateGameInfo`), two unused `tmp` variables, one old-style definition and the toolchain GNU-stack notice. Previous five-warning summaries omitted the About cleanup's unused helper; this change adds no warning.

```sh
python3 tests/test_startup_audio.py
```

[Machine-readable results](startup-audio-validation.json) identify this exact executable. The older `menu-validation.json` remains historical evidence for its own binary hash.

## Excluded experiment

An early candidate used zero mute samples plus a PSG sign-bit conversion and yielded a zero-valued idle capture. Full-range multi-channel tests exposed waveform-wrap risk, so that candidate was rejected. It is not part of this branch or the final test package. The adopted soft start deliberately preserves all steady-state legacy audio behavior. General PSG/DAC fidelity improvements remain separate work.
