#include "UiCanvas.h"
#include "UiText.h"
#include "UiFont_bin.h"
#include "UiFontJa_bin.h"
#include <string.h>
#include <stdio.h>

/* 768 4bpp tiles: 24 KiB. The tile map starts at byte 0x6000 in VRAM H. */
static uint32_t pixels[768 * 8];
static uint8_t dirty[768];

const uint32_t *uiCanvasPixels(void) { return pixels; }
uint8_t *uiCanvasDirty(void) { return dirty; }

static void pixel(int x, int y, unsigned color) {
	if ((unsigned)x >= 256 || (unsigned)y >= 192) return;
	unsigned tile = (unsigned)(y / 8) * 32 + (unsigned)x / 8;
	unsigned offset = tile * 8 + (y & 7);
	unsigned shift = (x & 7) * 4;
	uint32_t value = (pixels[offset] & ~(UINT32_C(15) << shift)) |
		((color & 15) << shift);
	if (value != pixels[offset]) {
		pixels[offset] = value;
		dirty[tile] = 1;
	}
}

void uiCanvasClear(unsigned color) {
	uint32_t value = (color & 15) * UINT32_C(0x11111111);
	for (unsigned i = 0; i < 768 * 8; i++) pixels[i] = value;
	memset(dirty, 1, sizeof(dirty));
}

void uiFillRect(int x, int y, int width, int height, unsigned color) {
	int right = x + width, bottom = y + height;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (right > 256) right = 256;
	if (bottom > 192) bottom = 192;
	uint32_t value = (color & 15) * UINT32_C(0x11111111);
	for (int row = y; row < bottom; row++) {
		int col = x;
		while (col < right && (col & 7)) pixel(col++, row, color);
		while (col + 8 <= right) {
			unsigned tile = (row / 8) * 32 + col / 8;
			unsigned offset = tile * 8 + (row & 7);
			if (pixels[offset] != value) {
				pixels[offset] = value;
				dirty[tile] = 1;
			}
			col += 8;
		}
		while (col < right) pixel(col++, row, color);
	}
}

void uiRoundRect(int x, int y, int width, int height, unsigned color) {
	uiFillRect(x + 2, y, width - 4, height, color);
	uiFillRect(x + 1, y + 1, width - 2, height - 2, color);
	uiFillRect(x, y + 2, width, height - 4, color);
}

static const uint8_t *findGlyph(const uint8_t *data, uint32_t code) {
	if (code > 65535) return NULL;
	unsigned count = data[0] | (data[1] << 8);
	unsigned lo = 0, hi = count;
	while (lo < hi) {
		unsigned mid = lo + (hi - lo) / 2;
		const uint8_t *entry = data + 2 + mid * 21;
		unsigned found = entry[0] | (entry[1] << 8);
		if (found < code) lo = mid + 1;
		else hi = mid;
	}
	if (lo == count) return NULL;
	const uint8_t *entry = data + 2 + lo * 21;
	return (unsigned)(entry[0] | (entry[1] << 8)) == code ? entry : NULL;
}

static const uint8_t *glyph(uint32_t code) {
	const uint8_t *entry = NULL;
	if (gUiLanguage == UI_LANGUAGE_JAPANESE) entry = findGlyph(UiFontJa_bin, code);
	if (!entry) entry = findGlyph(UiFont_bin, code);
	if (!entry) entry = findGlyph(UiFontJa_bin, code);
	return entry ? entry : findGlyph(UiFont_bin, '?');
}

int uiTextWidth(const char *text) {
	int width = 0;
	while (text && *text) {
		uint32_t code = uiNextCodepoint(&text);
		if (code < 32) break;
		const uint8_t *entry = glyph(code);
		if (entry) width += entry[2];
	}
	return width;
}

void uiText(const char *text, int x, int baseline, unsigned color, int right) {
	if (right > 256) right = 256;
	while (text && *text) {
		uint32_t code = uiNextCodepoint(&text);
		if (code < 32) break;
		const uint8_t *entry = glyph(code);
		if (!entry) continue;
		int advance = entry[2];
		if (x + advance > right) break;
		for (int y = 0; y < 12; y++) {
			for (int col = 0; col < 12; col++) {
				unsigned bit = y * 12 + col;
				if (entry[3 + bit / 8] & (0x80 >> (bit & 7))) {
					if (x + col < right) pixel(x + col, baseline - 10 + y, color);
				}
			}
		}
		x += advance;
	}
}

void uiTextEllipsis(const char *text, int x, int baseline, unsigned color, int right) {
	if (uiTextWidth(text) <= right - x) {
		uiText(text, x, baseline, color, right);
		return;
	}
	uiText(text, x, baseline, color, right - 18);
	uiText("...", right - 18, baseline, color, right);
}

void uiHeader(const char *title, int activeTab, int page, int pages) {
	uiCanvasClear(UI_BG);
	uiFillRect(0, 0, 256, 24, UI_TOP);
	uiFillRect(0, 23, 256, 1, UI_RULE);
	static const char *const tabs[] = {"File", "Options", "About"};
	for (int i = 0; i < 3; i++) {
		const char *label = uiTranslate(tabs[i]);
		int left = 8 + i * 80;
		if (activeTab == i + 1) uiRoundRect(left + 2, 2, 76, 20, UI_ACTIVE);
		uiText(label, left + 40 - uiTextWidth(label) / 2, 16,
			activeTab == i + 1 ? UI_INK : UI_SECONDARY, left + 78);
	}
	char number[16];
	snprintf(number, sizeof(number), "%d/%d", page, pages);
	int numberLeft = 244 - uiTextWidth(number);
	uiTextEllipsis(uiTranslate(title), 12, 40, UI_INK, numberLeft - 8);
	uiText(number, numberLeft, 40, UI_SECONDARY, 244);
}

void uiFooter(bool changing) {
	uiFillRect(0, 168, 256, 24, UI_BG);
	uiFillRect(8, 168, 240, 1, UI_RULE);
	uiText(uiTranslate(changing ? "A Change" : "A Select"), 12, 183, UI_SECONDARY, 80);
	uiText(uiTranslate("B Back"), 88, 183, UI_SECONDARY, 160);
	const char *hint = uiTranslate("L/R Tabs");
	uiText(hint, 244 - uiTextWidth(hint), 183, UI_SECONDARY, 244);
}

static void drawEntry(const char *label, const char *value, int slot, bool selected, bool translate) {
	if (slot < 0 || slot >= UI_PAGE_ROWS) return;
	int y = UI_ROW_TOP + slot * UI_ROW_HEIGHT;
	if (selected) uiRoundRect(8, y, 240, 24, UI_ACTIVE);
	else if (slot < UI_PAGE_ROWS - 1) uiFillRect(16, y + 23, 224, 1, UI_TOP);
	char caption[256];
	const char *translated = translate ? uiTranslate(label) : label;
	snprintf(caption, sizeof(caption), "%s", translated ? translated : "");
	size_t length = strlen(caption);
	if (translate && length && caption[length - 1] == ':') caption[length - 1] = 0;
	else if (translate && length >= 3 && !strcmp(caption + length - 3, "：")) caption[length - 3] = 0;
	const char *status = uiTranslate(value);
	int statusLeft = status ? 240 - uiTextWidth(status) : 240;
	uiTextEllipsis(caption, 16, y + 16, UI_INK, statusLeft - (status ? 8 : 0));
	if (status) uiText(status, statusLeft, y + 16,
		value && !strcmp(value, "Off") ? UI_SECONDARY : UI_VALUE, 240);
}

void uiDrawEntry(const char *label, const char *value, int slot, bool selected) {
	drawEntry(label, value, slot, selected, true);
}

void uiDrawFileEntry(const char *name, int slot, bool selected) {
	drawEntry(name, NULL, slot, selected, false);
}
