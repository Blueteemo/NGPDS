# NGPDS — Fusion Edition

**English** · [简体中文](README.zh-CN.md) · [日本語](README.ja.md)

Fusion Edition is Blueteemo's unofficial maintenance fork of NGPDS, the Neo Geo Pocket and Neo Geo Pocket Color emulator for Nintendo DS-family systems created by **Fredrik Ahlström (FluBBaOfWard)**.

The current tested build identifies itself as **V0.1.0 · 2026-11-11** in About.

[Original project](https://github.com/FluBBaOfWard/NGPDS) · [Original user guide](Docs/UPSTREAM_README.md) · [Earlier combined baseline](https://github.com/Blueteemo/NGPDS/tree/personal/combined-stable)

## What Fusion Edition adds

- **Graphics compatibility.** Restores story text in the tested scenes of *Infinity Cure*. Improves character transitions, scanline scrolling and profile text in *Memories Off Pure*. The renderer uses scanline state, with caching to reduce redundant sprite work. [Rendering notes](Docs/raster.md)
- **Flash saves and settings.** Adds Flash autosave, checks new writes before replacement, keeps a recovery copy and skips unchanged saves. Tested legacy saves include valid eight-byte empty Flash records. [Persistence notes](Docs/persistence.md)
- **Trilingual menus.** English, Simplified Chinese and Japanese share a 12px Fusion Pixel interface. Touch controls, menu hints and key routing adapt to the selected input mode.
- **Display options.** Choose the Graphite, Classic or Off border. Original, Mild Vivid (+15%) and Vivid (+20%) adjust saturation independently of the original gamma control. [Border artwork notes](Docs/GRAPHITE_BORDER.md)
- **Startup audio.** A short fade-in smooths the launch transient. The tested NDSi + R4 setup no longer produced the loud startup pop. Game audio synthesis and timing after the fade use the existing implementation. [Audio notes](Docs/STARTUP_AUDIO.md)

## Install and use

1. Build `NGPDS.nds` from the source below and place it on the flashcart. Some carts require DLDI patching.
2. Keep the existing `ngpds/` or `data/ngpds/` directory when upgrading. It holds settings and saves. Make a backup before replacing the executable.
3. Supply your own ROMs and BIOS. For native in-game saving, see the BIOS guidance in the [original user guide](Docs/UPSTREAM_README.md). Select a BIOS under **Options → Machine**.
4. Tap the lower screen or press **L+R** to open the menu. Under **Options → General → Interface Language**, choose English, 简体中文 or 日本語.

### Menu and display

By default, the open menu receives physical buttons and touch. **Autopause Game** controls whether the game keeps running behind the menu. Turn it off to reveal **Game Control in Menu**. This separate option is off by default. Enable it to send physical buttons to the loaded game and use touch for the menu. Touch **Next page** to cycle through a multipage menu in that mode. Without a loaded ROM, physical buttons operate the menu and file browser.

**Menu Screen Off**, **Autopause Game** and **Emulator on Bottom** are inherited NGPDS options. The screen-off option turns off the unused screen while the menu is closed, including the top screen if the game is on the bottom.

### Saving

**Autosave Flash** and **Autosave Settings** start enabled for a fresh configuration. Existing configurations keep their choices. With autosave disabled, use **File → Save Flash** or **File → Save Settings**. Save states are separate from in-game Flash saves.

## Build

This source lives on `personal/trilingual-menu` and uses pinned Git submodules. The tested toolchain was Wonderful arm-none-eabi GCC 16.2.0 with BlocksDS 1.24.0.

```sh
git clone --recursive --branch personal/trilingual-menu https://github.com/Blueteemo/NGPDS.git
cd NGPDS
export WONDERFUL_TOOLCHAIN=/path/to/wonderful
export BLOCKSDS="$WONDERFUL_TOOLCHAIN/thirdparty/blocksds/core"
make -j8 COMPDB=0
```

## Verification and limits

The latest menu build received user testing on NDSi + R4. Automated tests cover selected Japanese and Chinese-patched versions of Pure and Cure, rendering, key routing, old save files and injected write failures. Other ROM revisions, devices and scenes may behave differently. Save recovery has been tested with simulated failures. Sudden power loss on a physical SD card remains unverified. General audio fidelity, including percussion, is a separate investigation.

[Menu internals and test history](Docs/TRILINGUAL_MENU.md) · [Original scroll diagnostic ROMs](https://github.com/Blueteemo/ngpc-scroll-tests)

## Credits

- Original NGPDS: **Fredrik Ahlström (FluBBaOfWard)**.
- Fusion Edition maintenance: **Blueteemo**.
- UI font: **Fusion Pixel**, with [font sources, credits and licenses](third_party/fusion-pixel-12px/).

Original copyright and dependency notices remain with the code. The [original user guide](Docs/UPSTREAM_README.md) retains the upstream credits. Commercial ROMs, BIOS images and personal saves are supplied by the user.

```text
Huge thanks to Loopy for the incredible PocketNES, without it this emu would
probably never have been made.
Thanks to:
Flavor & Koyote for NGP info.
Dwedit for help and inspiration with a lot of things.
```
