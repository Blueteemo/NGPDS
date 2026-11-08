# NGPDS — Blueteemo Fork

[简体中文](README.zh-CN.md)

NGPDS is a Neo Geo Pocket / Neo Geo Pocket Color emulator for Nintendo DS-family systems, originally developed by **Fredrik Ahlström (FluBBaOfWard)**. This unofficial fork preserves compatibility, rendering-performance, and save-handling work developed while testing Memories Off - Pure and Infinity Cure.

[Original project](https://github.com/FluBBaOfWard/NGPDS) · [Original usage and menu documentation](Docs/UPSTREAM_README.md)

## Menu test branch

This branch adds a native 12px English / 简体中文 / 日本語 interface and an optional unused-screen power switch on top of the combined fixes and graphite border. The About page separates original authorship, fork maintenance, controls and component details. A startup-B hang with a BIOS for the other machine is corrected. See [controls, building and verification limits](Docs/TRILINGUAL_MENU.md). It is a separate test candidate; the default stable branch and upstream proposals are unchanged.

## Choose a branch

| Branch | Contents |
| --- | --- |
| [`personal/combined-stable`](https://github.com/Blueteemo/NGPDS/tree/personal/combined-stable) | Combined Pure/Cure graphics, performance, Flash-save, and settings improvements. Runtime source matches the locally accepted combined baseline; documentation is updated. |
| `main` | Upstream runtime baseline plus this fork's documentation. **Does not contain the combined improvements.** |
| [`fix/raster-graphics-performance`](https://github.com/Blueteemo/NGPDS/tree/fix/raster-graphics-performance) | Separate graphics/performance proposal, [upstream PR #15](https://github.com/FluBBaOfWard/NGPDS/pull/15). |
| [`fix/flash-autosave-persistence`](https://github.com/Blueteemo/NGPDS/tree/fix/flash-autosave-persistence) | Separate persistence proposal, [upstream PR #14](https://github.com/FluBBaOfWard/NGPDS/pull/14). |

The combined branch preserves accepted commit `8b9a49b2ebb1f37d9e49a835c5f833ca41ec58f2` and its pinned dependencies. Later isolated scroll-boundary experiments and the reduced save-only candidate are **not** included. The upstream proposals remain unmerged. This documentation update does not publish a new emulator binary release.

## Combined-branch changes

- **Memories Off - Pure:** improve the tested character transitions, scanline background scrolling, and profile-page text. Cache sprite inputs and skip redundant composition and uploads to reduce rendering work.
- **Infinity Cure:** restore missing story text in the tested gameplay scenes while preserving the tested CG, background, and layer behavior.
- **Flash saves and settings:** add a Flash autosave option, improve configuration persistence, validate replacement files, retain recovery copies, and skip unchanged Flash writes. Tested legacy `.fla`, `.sta`, and `settings.cfg` formats remain compatible, including valid eight-byte empty Flash records.

Rendering changes address general scanline/register behavior; there are no game-ID, fixed-frame, or image-signature conditions. No audio-synthesis or clock workaround is introduced. The sprite compositor adds no DMA channel or HBlank CPU interrupt; backdrop replay uses DMA1, palette replay uses DMA2/DMA3 after the existing VBlank transfers, and DMA0 retains the scroll stream.

The combined branch enables Flash/settings autosaving for a fresh configuration. Existing configurations retain their explicit choices, including disabled autosaving. **Replacing the executable does not turn an existing Off setting On.** With Flash autosaving off, use **File → Save Flash** to persist in-game progress before leaving.

## Validation and limitations

The combined baseline passed automated emulator checks, save-failure injection, configuration tests, and audio comparisons. Testing focused on the Japanese releases and the specific translated ROM versions used during development. Other translation patches, expanded ROMs, scenes, and games are not automatically covered.

An earlier optimized C build received positive user feedback on NDSi + R4 in ordinary DS/NTR mode. This is historical feedback, **not** a new hardware validation of the final combined executable or its later save changes. Save protection does not guarantee recovery from every SD-card fault or sudden power loss. Reference-core agreement is not physical-hardware proof; complete compatibility and timing accuracy are not claimed.

Combined-branch implementation notes: [rendering](https://github.com/Blueteemo/NGPDS/blob/personal/combined-stable/Docs/raster.md) and [persistence](https://github.com/Blueteemo/NGPDS/blob/personal/combined-stable/Docs/persistence.md).

## Build and use

For the combined source:

```sh
git clone --recursive --branch personal/combined-stable https://github.com/Blueteemo/NGPDS.git
cd NGPDS
# Configure your existing Wonderful / BlocksDS installation first.
make -j8 COMPDB=0
```

The accepted build used Wonderful arm-none-eabi GCC 16.2.0 and BlocksDS 1.24.0. `COMPDB=0` disables compilation-database merging, not code generation. Keep the pinned submodule revisions when reproducing that baseline.

Put `NGPDS.nds` on your card, retain the `ngpds/` data directory, and supply your own games and any required BIOS. Back up saves/configuration before replacing an executable. Menu controls, BIOS selection, and the original DS/DS Lite memory constraints are documented in the [preserved upstream README](Docs/UPSTREAM_README.md). Its game-issue list describes the **upstream baseline**, not a verdict on the modified branch. On the combined branch, **Autosave Flash** is an additional setting.

## Reproducible diagnostics

[NGPC scroll tests](https://github.com/Blueteemo/ngpc-scroll-tests) provides original test ROMs, source, build instructions, and reference observations. It is a separate MIT-licensed diagnostic project, not the license for this emulator. The later minimal K2GE experiment is kept separate from this combined baseline.

## Authorship and notices

Original NGPDS credits and copyright notices are retained below and in the preserved upstream documentation. Dependency notices remain with their respective sources. This fork does not imply endorsement by the original author and does not assign a new license to upstream code. No commercial game ROM, BIOS, private save, or copyrighted-game screenshot is provided.

## Credits

```text
Huge thanks to Loopy for the incredible PocketNES, without it this emu would
probably never have been made.
Thanks to:
Flavor & Koyote for NGP info.
Dwedit for help and inspiration with a lot of things.
```

Fredrik Ahlström

<https://bsky.app/profile/therealflubba.bsky.social>

<https://www.github.com/FluBBaOfWard>

X/Twitter @TheRealFluBBa
