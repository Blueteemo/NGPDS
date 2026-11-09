#ifndef UI_CANVAS_H
#define UI_CANVAS_H
#include <stdbool.h>
#include <stdint.h>

#define UI_SCREEN_WIDTH 256
#define UI_SCREEN_HEIGHT 192
#define UI_PAGE_ROWS 5
#define UI_ROW_TOP 48
#define UI_ROW_HEIGHT 24

enum UiColor {
	UI_BG, UI_TOP, UI_RULE, UI_ACTIVE, UI_INK, UI_SECONDARY, UI_VALUE, UI_WARNING
};
void uiCanvasClear(unsigned color);
void uiFillRect(int x, int y, int width, int height, unsigned color);
void uiRoundRect(int x, int y, int width, int height, unsigned color);
int uiTextWidth(const char *text);
void uiText(const char *text, int x, int baseline, unsigned color, int right);
void uiTextEllipsis(const char *text, int x, int baseline, unsigned color, int right);
void uiHeader(const char *title, int activeTab, int page, int pages);
int uiRowsTop(void);
void uiListEdges(int visibleRows);
void uiFooter(bool changing);
void uiDrawEntry(const char *label, const char *value, int slot, bool selected);
void uiDrawFileEntry(const char *name, int slot, bool selected);
const uint32_t *uiCanvasPixels(void);
uint8_t *uiCanvasDirty(void);
void uiCanvasInitNds(void);
void uiCanvasPalette(void);
void uiCanvasPresent(void);
#endif
