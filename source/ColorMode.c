#include <nds.h>
#include "Emubase.h"
#include "Gfx.h"
#include "Shared/EmuMenu.h"

extern u16 MAPPED_RGB[4096];
void paletteInitBase(u8 gammaVal);

unsigned getColorMode(void) {
	unsigned mode = (emuSettings & COLOR_MODE_MASK) >> COLOR_MODE_SHIFT;
	return mode < COLOR_MODE_COUNT ? mode : COLOR_ORIGINAL;
}

static unsigned boostChannel(int channel, int luminance, int strength) {
	/* Scale chroma around luminance and round once to RGB555. */
	int value = (25600 * channel + strength * (256 * channel - luminance) + 12800) / 25600;
	if (value < 0) return 0;
	if (value > 31) return 31;
	return (unsigned)value;
}

void paletteInit(u8 gammaVal) {
	paletteInitBase(gammaVal);
	unsigned mode = getColorMode();
	if (mode == COLOR_ORIGINAL) return;
	int strength = mode == COLOR_MILD_VIVID ? 15 : 20;

	/* All frame, sprite and scanline palettes consume the same lookup table. */
	for (unsigned i = 0; i < 4096; i++) {
		unsigned color = MAPPED_RGB[i];
		int r = color & 31;
		int g = (color >> 5) & 31;
		int b = (color >> 10) & 31;
		int y = 54 * r + 183 * g + 19 * b;
		MAPPED_RGB[i] = boostChannel(r, y, strength) |
			(boostChannel(g, y, strength) << 5) |
			(boostChannel(b, y, strength) << 10);
	}
}
