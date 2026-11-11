#include <nds.h>

#include "NeoGeoPocket.h"
#include "NGPBorder.h"
#include "NGPBorderOriginal.h"
#include "NGPBorderOff.h"
#include "Emubase.h"
#include "Shared/EmuMenu.h"
#include "Cart.h"
#include "Gfx.h"
#include "RasterSprites.h"
#include "Sound.h"
#include "io.h"
#include "TLCS900H/TLCS900H.h"
#include "ARMZ80/ARMZ80.h"


int packState(void *statePtr) {
	int size = 0;
	memcpy(statePtr+size, ngpRAM, sizeof(ngpRAM));
	size += sizeof(ngpRAM);
	size += ioSaveState(statePtr+size);
	size += sn76496SaveState(statePtr+size, &k2Audio_0);
	size += k2GESaveState(statePtr+size, &k2GE_0);
	size += Z80SaveState(statePtr+size, &Z80OpTable);
	size += tlcs900HSaveState(statePtr+size, &tlcs900HState);
	return size;
}

void unpackState(const void *statePtr) {
	rasterSpritesReset();
	int size = 0;
	memcpy(ngpRAM, statePtr+size, sizeof(ngpRAM));
	size += sizeof(ngpRAM);
	size += ioLoadState(statePtr+size);
	size += sn76496LoadState(&k2Audio_0, statePtr+size);
	size += k2GELoadState(&k2GE_0, statePtr+size);
	size += Z80LoadState(&Z80OpTable, statePtr+size);
	tlcs900HLoadState(&tlcs900HState, statePtr+size);
}

int getStateSize() {
	int size = 0;
	size += sizeof(ngpRAM);
	size += ioGetStateSize();
	size += sn76496GetStateSize();
	size += k2GEGetStateSize();
	size += Z80GetStateSize();
	size += tlcs900HGetStateSize();
	return size;
}

unsigned getBorderMode(void) {
	unsigned mode = (emuSettings & BORDER_MODE_MASK) >> BORDER_MODE_SHIFT;
	return mode < BORDER_MODE_COUNT ? mode : BORDER_GRAPHITE;
}

/* Map blocks 0–1 and tile blocks 2–3 belong to K2GE. Both border slots
 * fit in the main background VRAM already mapped by setupGraphics(). */
static const unsigned borderMapBlocks[] = {2, 3};
static const unsigned borderTileBlocks[] = {1, 4};
static volatile unsigned displayedBorderSlot;
static const u16 *volatile pendingBorderPalette;

void setupEmuBackground(void) {
	static const struct {
		const void *tiles;
		const void *map;
		const u16 *palette;
	} borders[BORDER_MODE_COUNT] = {
		[BORDER_GRAPHITE] = {NGPBorderTiles, NGPBorderMap, NGPBorderPal},
		[BORDER_ORIGINAL] = {NGPBorderOriginalTiles, NGPBorderOriginalMap, NGPBorderOriginalPal},
		[BORDER_OFF] = {NGPBorderOffTiles, NGPBorderOffMap, NGPBorderOffPal},
	};
	unsigned mode = getBorderMode();
	int oldIME = enterCriticalSection();
	/* Cancel an uncommitted selection before reusing its slot. */
	pendingBorderPalette = NULL;
	unsigned slot = displayedBorderSlot ^ 1;
	leaveCriticalSection(oldIME);

	decompress(borders[mode].tiles, BG_TILE_RAM(borderTileBlocks[slot]), LZ77Vram);
	decompress(borders[mode].map, BG_MAP_RAM(borderMapBlocks[slot]), LZ77Vram);
	pendingBorderPalette = borders[mode].palette;
}

void publishEmuBackground(void) {
	const u16 *palette = pendingBorderPalette;
	if (!palette) return;
	unsigned slot = displayedBorderSlot ^ 1;
	/* BG2 still carries the game's scanline backdrop when the border is off.
	 * Only change decoration colors; paletteTxAll owns the game pens. */
	for (unsigned i = 1; i < 256; i++) {
		unsigned pen = i & 15;
		if (i != 32 && !((pen >= 1 && pen <= 3) || (pen >= 5 && pen <= 7))) {
			EMUPALBUFF[i] = i == 16 ? 0 : palette[i];
		}
	}
	REG_BG2CNT = BG_32x32 | BG_MAP_BASE(borderMapBlocks[slot]) | BG_COLOR_256 |
		BG_TILE_BASE(borderTileBlocks[slot]) | BG_PRIORITY(2);
	displayedBorderSlot = slot;
	pendingBorderPalette = NULL;
}
