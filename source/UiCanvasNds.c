#include <nds.h>
#include "UiCanvas.h"

void uiCanvasPalette(void) {
	static const uint8_t colors[][3] = {
		{16,24,24}, {24,32,32}, {40,48,48}, {40,56,64},
		{232,240,240}, {152,168,168}, {144,208,208}, {248,176,96},
		[UI_SUBTITLE] = {176,192,192}
	};
	for (unsigned i = 0; i < sizeof(colors) / sizeof(colors[0]); i++) {
		BG_PALETTE_SUB[i] = RGB15(colors[i][0] >> 3, colors[i][1] >> 3, colors[i][2] >> 3);
	}
}

void uiCanvasPresent(void) {
	const uint32_t *source = uiCanvasPixels();
	uint8_t *dirty = uiCanvasDirty();
	volatile uint32_t *destination = (volatile uint32_t *)BG_GFX_SUB;
	for (unsigned tile = 0; tile < 768; tile++) {
		if (!dirty[tile]) continue;
		for (unsigned word = 0; word < 8; word++) {
			destination[tile * 8 + word] = source[tile * 8 + word];
		}
		dirty[tile] = 0;
	}
}

void uiCanvasInitNds(void) {
	/* Reuse VRAM H only: tile pixels [0,0x6000), map [0x6000,0x6800). */
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	REG_BG0CNT_SUB = BG_COLOR_16 | BG_MAP_BASE(12);
	REG_BG0HOFS_SUB = REG_BG0VOFS_SUB = 0;
	for (unsigned i = 0; i < 1024; i++) BG_MAP_RAM_SUB(12)[i] = i < 768 ? i : 0;
	uiCanvasPalette();
	uiCanvasClear(UI_BG);
	uiCanvasPresent();
}
