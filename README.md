# NGPDS — Fusion Edition

**English** · [简体中文](README.zh-CN.md) · [日本語](README.ja.md)

**[Download v1.0.0 (.nds)](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/NGPDS_Fusion_v1.0.0.nds)** · [Release notes](https://github.com/Blueteemo/NGPDS/releases/tag/v1.0.0) · [Source and dependencies (ZIP)](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/NGPDS_Fusion_v1.0.0_Source.zip) · [SHA-256](https://github.com/Blueteemo/NGPDS/releases/download/v1.0.0/SHA256SUMS.txt)

This branch retains the earlier combined runtime. For the v1.0.0 source and its pinned build dependencies, use the ZIP linked above.

Fusion Edition is Blueteemo's unofficial maintenance fork of NGPDS, the Neo Geo Pocket and Neo Geo Pocket Color emulator for Nintendo DS-family systems created by **Fredrik Ahlström (FluBBaOfWard)**.

The v1.0.0 baseline identifies itself as `V1.0.0 2026-11-13` in About.

[Original project](https://github.com/FluBBaOfWard/NGPDS) · [Original user guide](Docs/UPSTREAM_README.md) · [Earlier combined baseline](https://github.com/Blueteemo/NGPDS/tree/1c55ee4115e84cd40bd5ffc51c447814a3367aa2)

## What Fusion Edition adds

- **Graphics compatibility.** Restores story text in the tested scenes of *Infinity Cure*. Improves character transitions, scanline scrolling and profile text in *Memories Off Pure*. The renderer uses scanline state, with caching to reduce redundant sprite work. [Rendering notes](Docs/raster.md)
- **Flash saves and settings.** Adds Flash autosave, checks new writes before replacement, keeps a recovery copy and skips unchanged saves. Tested legacy saves include valid eight-byte empty Flash records. [Persistence notes](Docs/persistence.md)
- **Trilingual menus.** English, Simplified Chinese and Japanese share a 12px Fusion Pixel interface. Touch controls, menu hints and key routing adapt to the selected input mode.
- **Display options.** Choose the Graphite, Classic or Off border. Original, Mild Vivid (+15%) and Vivid (+20%) adjust saturation independently of the original gamma control. [Border artwork notes](https://github.com/Blueteemo/NGPDS/blob/a3b33264382e5056ab8b997d76957dd1607d0ec8/Docs/GRAPHITE_BORDER.md)
- **Audio.** **Options → General → Audio Mode** offers Standard (default) and Classic (legacy mixer). A short fade-in addresses the startup pop. The pop was gone in earlier NDSi + R4 testing. [Startup audio notes](https://github.com/Blueteemo/NGPDS/blob/a3b33264382e5056ab8b997d76957dd1607d0ec8/Docs/STARTUP_AUDIO.md)

## Install and use

1. Download `NGPDS_Fusion_v1.0.0.nds` and copy it to the flashcart. Some carts require DLDI patching.
2. Keep the existing `ngpds/` or `data/ngpds/` directory when upgrading. It holds settings and saves. Make a backup before replacing the executable.
3. Supply your own ROMs. The built-in HLE BIOS can run supported games and handle supported Flash saves. To use a BIOS image you supply, select it under **Options → Machine**.
4. Tap the lower screen or press **L+R** to open the menu. Under **Options → General → Interface Language**, choose English, 简体中文 or 日本語.

### Menu and display

By default, the open menu receives physical buttons and touch. **Autopause Game** controls whether the game keeps running behind the menu. Turn it off to reveal **Game Control in Menu**. This separate option is off by default. Enable it to send physical buttons to the loaded game and use touch for the menu. Touch **Next page** to cycle through a multipage menu in that mode. Without a loaded ROM, physical buttons operate the menu and file browser.

**Menu Screen Off**, **Autopause Game** and **Emulator on Bottom** are inherited NGPDS options. The screen-off option turns off the unused screen while the menu is closed, including the top screen if the game is on the bottom.

### Saving

**Autosave Flash** and **Autosave Settings** start enabled for a fresh configuration. Existing configurations keep their choices. With autosave disabled, use **File → Save Flash** or **File → Save Settings**. Save states are separate from in-game Flash saves.

## Build

The v1.0.0 source archive includes the main repository and all seven pinned dependency commits as Git bundles. Its `rebuild.sh` restores a clean checkout offline, builds `NGPDS.nds` and checks its SHA-256. The verified toolchain is Wonderful arm-none-eabi GCC 16.2.0 with BlocksDS 1.24.0.

From the directory containing the source archive’s `rebuild.sh`:

```sh
export WONDERFUL_TOOLCHAIN=/path/to/wonderful
sh rebuild.sh /path/to/new-checkout
```

## Verification and limits

Earlier menu and startup audio builds were tested on NDSi + R4. Automated tests cover selected scenes from the Japanese and Chinese-patched versions of Pure and Cure, menu input, legacy saves and simulated write failures. In melonDS, the Japanese release of Cure saved and resumed after a cold boot using the built-in HLE BIOS. That result covers the tested game and save path. Physical SD-card power loss and percussion fidelity remain unverified. A clean rebuild from the v1.0.0 source ZIP matched the Release executable byte for byte. The executable’s checksum also matched after copying it to R4.

[Menu internals and test history](https://github.com/Blueteemo/NGPDS/blob/a3b33264382e5056ab8b997d76957dd1607d0ec8/Docs/TRILINGUAL_MENU.md) · [Original scroll diagnostic ROMs](https://github.com/Blueteemo/ngpc-scroll-tests)

## Credits

- Original NGPDS: **Fredrik Ahlström (FluBBaOfWard)**.
- Fusion Edition maintenance: **Blueteemo**.
- UI font: **Fusion Pixel**, with [font sources, credits and licenses](https://github.com/Blueteemo/NGPDS/tree/a3b33264382e5056ab8b997d76957dd1607d0ec8/third_party/fusion-pixel-12px/).

Original copyright and dependency notices remain with the code. The [original user guide](Docs/UPSTREAM_README.md) retains the upstream credits. Commercial ROMs, BIOS images and personal saves are supplied by the user.

```text
Huge thanks to Loopy for the incredible PocketNES, without it this emu would
probably never have been made.
Thanks to:
Flavor & Koyote for NGP info.
Dwedit for help and inspiration with a lot of things.
```

## Earlier source archive

[v0.2.3 source archive](https://github.com/Blueteemo/NGPDS/raw/d5389c337dc0f4e93cac3e75bf18e2be0ffd602b/NGPDS_Fusion_v0.2.3_Source.zip) · [SHA-256](https://github.com/Blueteemo/NGPDS/blob/d5389c337dc0f4e93cac3e75bf18e2be0ffd602b/SHA256SUMS.txt)
