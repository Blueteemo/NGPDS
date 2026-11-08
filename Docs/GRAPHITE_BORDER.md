# Personal graphite border

This branch retains the reviewed Pure/Cure rendering and persistence code and changes only the static border asset. The game viewport remains 160 x 152 at (48, 20). The flat light graphite panel has one consistent dark outer edge and an inset dark rim; the side decoration and decorative lamp are retained. There is no gradient or dithering.

The border keeps palette index 32 throughout the viewport for the existing scanline backdrop path. Decorative pixels avoid the palette entries updated by the game renderer. The asset uses 113 tiles, compared with 155 in the original border; it introduces no per-frame code or additional DMA use.

Validation: BlocksDS build succeeded. Seven paired short melonDS cases (three original scrolling diagnostics and Pure/Cure JP/ZH) produced no RGB5 differences inside the game viewport at the compared captures. The complete panel matched the selected design preview. These are bounded emulator checks, not new hardware, audio, save-system or full-game acceptance. The NDS file is 274432 bytes (SHA-256 0e05a3d3d1871d4480c39067bb16e6585f748e5fa289d29628af9d88f4d339da).

Artwork provenance: the side decoration was retained from the user's existing GarlicOS NGPC overlay. Its original artist and redistribution terms have not been established. This fork claims no authorship of that artwork and assigns it no new license. The graphite panel itself is drawn from simple geometric shapes. Original NGPDS and dependency notices remain unchanged.

No game ROM, BIOS, personal save, game capture or research working directory is included in this commit.
