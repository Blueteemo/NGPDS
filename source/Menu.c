/* NGPDS-specific menu adaptation of NDS_Shared/EmuMenu.c.
 * Original Copyright (c) 2020-2025 Fredrik Ahlström, MIT.
 * Full original notice: Docs/licenses/NDS_Shared-menu.txt.
 * The shared dependency remains unmodified. */
#include <nds.h>

//#include <string.h>
//#include <stdio.h>
#include <time.h>

#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
#include "Shared/AsmExtra.h"
#include "Main.h"
#include "Gui.h"
#include "FileHandling.h"
#include "Gfx.h"				// gFlicker & gTwitch
#include "io.h"
#include "Sound.h"

#include "UiText.h"
#include "UiCanvas.h"
#include "bios.h"
#include "Cart.h"

#define MENU_MAX_DEPTH (8)

extern const Menu *const menus[];

static void exitUI(void);
static void drawClock(void);
static void setSelectedMenu(int menuNr);
static void setSelectedMain(int menuNr);
static void nullUI(void);
static void subUI(void);

const char *const autoTxt[]  = {"Off", "On", "With R"};
const char *const brighTxt[] = {"I", "II", "III", "IIII", "IIIII"};
static const char *const speedTxt[] = {"Normal", "200%", "Max", "50%"};
static const char *const sleepTxt[] = {"5min", "10min", "30min", "Off"};
static const char *const flickTxt[] = {"No Flicker", "Flicker"};

u8 autoA = 0;
u8 autoB = 0;
u8 gGammaValue = 0;

bool gDebugSet = false;
bool settingsChanged = false;
bool pauseEmulation = false;
bool enableExit = false;

int emuSettings = 0;
int sleepTime = 60*60*5;
int selected = 0;
int menuYOffset = 0;

static int selectedMenu = 0;
static int selectedMain = 0;
static int lastMainMenu = 1;
static int menuItemRow = 0;
static int lineRepeat = 0;
static bool browserActive;
static int pendingMainTab;
static char browserTitle[256];
static int browserFirst, browserSelected, browserItems;
static int pendingBrowserSelection = -1;
static int oldNameOffset = -1;
// How deep we are in the menu tree
static int menuLevel = 0;
static char menuPath[MENU_MAX_DEPTH];
static char menuPositions[MENU_MAX_DEPTH];

static int logBufPtr = 0;
static int logBufPtrOld = 0;
static int logTimer = 0;
static time_t oldTime = 0;

static char logBuffer[8][256];
static char timeBuffer[6];

void guiRunLoop(void) {
	if (pendingMainTab && !browserActive) {
		int tab = pendingMainTab;
		pendingMainTab = 0;
		if (selectedMenu) setSelectedMain(tab);
	}
	if (selectedMenu == 0) {
		nullUI();
	}
	else {
		subUI();
	}
	updateTime();
	/* Honor the user's pause preference once a cartridge/real BIOS exists;
	 * an empty HLE console has no standalone CPU program to execute. */
	if (!gameInserted && !g_BIOSBASE_BNW && !g_BIOSBASE_COLOR && !pauseEmulation) {
		pauseEmulation = true;
		soundSetMuteGUI();
	}
}

void uiNullDefault(void) {
	uiCanvasClear(UI_BG);
	const char *first = uiTranslate("Touch screen or press L+R");
	const char *second = uiTranslate("to open the menu.");
	uiText("NGPDS", 128 - uiTextWidth("NGPDS") / 2, 56, UI_INK, 244);
	uiText(first, 128 - uiTextWidth(first) / 2, 88, UI_INK, 244);
	uiText(second, 128 - uiTextWidth(second) / 2, 108, UI_SECONDARY, 244);
}

void uiAutoSub(void) { uiAuto(); }

void uiAuto(void) {
	setupMenu();
	const Menu *menu = menus[selectedMenu];
	for (int i = 0; i < menu->itemCount; i++) {
		const char *value = menu->items[i].txt2 ? menu->items[i].txt2() : NULL;
		drawSubItem(menu->items[i].text, value);
	}
}


void setupMenu(void) {
	const Menu *menu = menus[selectedMenu];
	int pages = (menu->itemCount + UI_PAGE_ROWS - 1) / UI_PAGE_ROWS;
	uiHeader(menu->header, selectedMain, selected / UI_PAGE_ROWS + 1, pages ? pages : 1);
	uiFooter(selectedMenu >= 4 && selectedMenu <= 8);
	menuItemRow = 0;
}
void setupSubMenuText(void) {
	setupSubMenu(menus[selectedMenu]->header);
}
void setupSubMenu(const char *menuString) { (void)menuString; setupMenu(); }

void drawTabs(void) { setupMenu(); }

void uiDummy() {
}

void ui1() {
	setSelectedMain(1);
}
void ui2() {
	setSelectedMain(2);
}
void ui3() {
	setSelectedMain(3);
}
void ui4() {
	enterMenu(4);
}
void ui5() {
	enterMenu(5);
}
void ui6() {
	enterMenu(6);
}
void ui7() {
	enterMenu(7);
}
void ui8() {
	enterMenu(8);
}
void ui9() {
	enterMenu(9);
}
void ui10() {
	enterMenu(10);
}

void setSelectedMenu(int menuNr) {
	selectedMenu = menuNr;
	if (selectedMenu == 0) {
		exitUI();
	}
	else if (selectedMenu < 4) {
		selectedMain = selectedMenu;
	}
	cls(0);
	redrawUI();
}

void setSelectedMain(int menuNr) {
	menuLevel = 0;
	enterMenu(menuNr);
}

void enterMenu(int menuNr) {
	menuPositions[menuLevel] = selected;
	menuLevel++;
	if (menuLevel >= MENU_MAX_DEPTH) {
		menuLevel = MENU_MAX_DEPTH - 1;
	}
	if (menuPath[menuLevel] == menuNr) {
		selected = menuPositions[menuLevel];
	}
	else {
		menuPath[menuLevel] = menuNr;
		selected = 0;
	}
	setSelectedMenu(menuNr);
}

void backOutOfMenu() {
	menuPositions[menuLevel] = selected;
	menuLevel--;
	if (menuLevel < 0) {
		menuLevel = 0;
	}
	selected = menuPositions[menuLevel];
	setSelectedMenu(menuPath[menuLevel]);
}

void openMenu() {
	enterGUI();
	setSelectedMain(lastMainMenu);
	powerOn(POWER_2D_B);
	powerOn(PM_BACKLIGHT_TOP);
	powerOn(PM_BACKLIGHT_BOTTOM);
	lcdMainOnTop();
	if (emuSettings & AUTOPAUSE_EMULATION) {
		pauseEmulation = true;
		soundSetMuteGUI();
	}
	if (emuSettings & AUTOSAVE_NVRAM) {
		saveNVRAM();
	}
}

void closeMenu() {
	if (menuLevel > 1) {
		menuLevel = 1;
	}
	backOutOfMenu();
}

bool isMenuOpen() {
	return (selectedMenu != 0);
}

void exitUI() {
	if (settingsChanged && (emuSettings & AUTOSAVE_SETTINGS)) {
		saveSettings();
	}
	lastMainMenu = selectedMain;
	if (lastMainMenu < 1) {
		lastMainMenu = 1;
	}

	if (emuSettings & MAIN_ON_BOTTOM) {
		lcdMainOnBottom();
	}
	if (emuSettings & POWER_SAVE_MENU) {
		powerOff(POWER_2D_B);
		if (emuSettings & MAIN_ON_BOTTOM) {
			powerOff(PM_BACKLIGHT_TOP);
		}
		else {
			powerOff(PM_BACKLIGHT_BOTTOM);
		}
	}
	exitGUI();
	/* HLE has no standalone console program without a cartridge. Keep
	 * its CPU idle so the empty-ROM menu can be reopened. Real BIOS
	 * standalone operation and every inserted cartridge retain their path. */
	pauseEmulation = !gameInserted && !g_BIOSBASE_BNW && !g_BIOSBASE_COLOR;
	soundSetMuteGUI();
}

/// This is during emulation.
void nullUI() {
	int key = getInput();
	if (EMUinput & KEY_OPEN_MENU) {
		openMenu();
		return;
	}

	if (gDebugSet) {
		drawItem(fpsText, 0, 0, 0);
		nullUIDebug(key);
	}
	else {
		nullUINormal(key);
	}
	updateInfoLog();
}

/// This is during menu.
void subUI() {
	const int key = getMenuInput(menus[selectedMenu]->itemCount);

	if (key & KEY_A) {
		menus[selectedMenu]->items[selected].fn();
	}
	if (key & KEY_B) {
		backOutOfMenu();
	}
	if (key & KEY_L) {
		setSelectedMain(selectedMain == 1 ? 3 : selectedMain - 1);
	}
	if (key & KEY_R) {
		setSelectedMain(selectedMain == 3 ? 1 : selectedMain + 1);
	}
	if (key & (KEY_A|KEY_UP|KEY_DOWN|KEY_LEFT|KEY_RIGHT|KEY_TOUCH)) {
		redrawUI();
	}
	updateInfoLog();
}

int getMenuInput(int itemCount) {
	int keyHit = getInput();
	selected = getMenuTouch(&keyHit, selected, itemCount);
	selected = getMenuPos(keyHit, selected, itemCount);
	return keyHit;
}

//---------------------------------------------------------------------------------
int getInput(void) {
	EMUinput = convertInput(keysHeld());
	int keyHit = keysDown();
	int dpad = keysDownRepeat() & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
	keyHit = dpad | (keyHit & (KEY_A | KEY_B | KEY_X | KEY_Y | KEY_START | KEY_L | KEY_R | KEY_TOUCH));
	if (browserActive && (keyHit & (KEY_L | KEY_R))) {
		pendingMainTab = keyHit & KEY_R ? (selectedMain == 3 ? 1 : selectedMain + 1) :
			(selectedMain == 1 ? 3 : selectedMain - 1);
		keyHit |= KEY_B;
	}
	if (browserActive && (keyHit & KEY_TOUCH)) {
		touchPosition touch;
		touchRead(&touch);
		if (touch.py < 24 && touch.px >= 8 && touch.px < 248) {
			int tab = (touch.px - 8) / 80 + 1;
			if (tab != selectedMain) {
				pendingMainTab = tab;
				keyHit |= KEY_B;
			}
		} else if (touch.py >= 48 && touch.py < 168 && touch.px >= 8 && touch.px < 248) {
			int item = browserFirst + (touch.py - 48) / 24;
			if (item < browserItems) {
				if (item == browserSelected) keyHit |= KEY_A;
				else pendingBrowserSelection = item;
			}
		} else if (touch.py >= 168) {
			if (touch.px >= 8 && touch.px < 72) keyHit |= KEY_A;
			else if (touch.px >= 72 && touch.px < 160) keyHit |= KEY_B;
		}
	}
	return keyHit;
}

//---------------------------------------------------------------------------------
int getMenuPos(int keyHit, int sel, int itemCount) {
	if (itemCount <= 0) return 0;
	if (browserActive && pendingBrowserSelection >= 0) {
		sel = pendingBrowserSelection;
		pendingBrowserSelection = -1;
		return sel < itemCount ? sel : itemCount - 1;
	}
	if (keyHit & KEY_UP) sel = (sel + itemCount - 1) % itemCount;
	if (keyHit & KEY_DOWN) sel = (sel + 1) % itemCount;
	if (keyHit & KEY_RIGHT) {
		sel += UI_PAGE_ROWS;
		if (sel >= itemCount) sel = itemCount - 1;
	}
	if (keyHit & KEY_LEFT) {
		sel -= UI_PAGE_ROWS;
		if (sel < 0) sel = 0;
	}
	return sel;
}

//---------------------------------------------------------------------------------
int getMenuTouch(int *keyHit, int sel, int itemCount) {
	if (!(*keyHit & KEY_TOUCH)) return sel;
	touchPosition touch;
	touchRead(&touch);
	if (touch.py < 24 && touch.px >= 8 && touch.px < 248) {
		setSelectedMain((touch.px - 8) / 80 + 1);
		return 0;
	}
	if (touch.py >= 48 && touch.py < 168 && touch.px >= 8 && touch.px < 248) {
		int item = menuYOffset + (touch.py - 48) / 24;
		if (item < itemCount) {
			if (sel == item) *keyHit |= KEY_A;
			else sel = item;
		}
	} else if (touch.py >= 168) {
		if (touch.px >= 8 && touch.px < 72) *keyHit |= KEY_A;
		else if (touch.px >= 72 && touch.px < 160) *keyHit |= KEY_B;
	}
	return sel;
}

void redrawUI(void) {
	browserActive = false;
	pendingBrowserSelection = -1;
	int count = menus[selectedMenu]->itemCount;
	if (selected >= count) selected = count ? count - 1 : 0;
	menuYOffset = (selected / UI_PAGE_ROWS) * UI_PAGE_ROWS;
	menus[selectedMenu]->drawFunc();
	outputLogToScreen();
	uiCanvasPresent();
}

int drawFileList(char **entries, int sel, int itemCount) {
	browserActive = true;
	browserSelected = sel;
	browserItems = itemCount;
	browserFirst = (sel / UI_PAGE_ROWS) * UI_PAGE_ROWS;
	lineRepeat = 0;
	oldNameOffset = -1;
	uiHeader(browserTitle, selectedMain, sel / UI_PAGE_ROWS + 1,
		(itemCount + UI_PAGE_ROWS - 1) / UI_PAGE_ROWS);
	for (int i = 0; i < UI_PAGE_ROWS; i++) {
		const char *name = directoryStringFromPos(entries, browserFirst + i);
		if (!name) name = "";
		if (*name == '~') name++;
		uiDrawFileEntry(name, i, browserFirst + i == sel);
	}
	uiFooter(false);
	uiCanvasPresent();
	return sel - browserFirst;
}

void drawLongFilename(char **entries, int item, int row) {
	const char *name = directoryStringFromPos(entries, item);
	if (!name) return;
	if (*name == '~') name++;
	if (uiTextWidth(name) <= 224) return;
	const char *end = name;
	int extra = 0;
	while (*end && uiTextWidth(end) > 224) {
		uiNextCodepoint(&end);
		extra++;
	}
	int step = lineRepeat++;
	int cycle = 120 + extra * 16;
	int position = step % cycle;
	int offset = position < 60 ? 0 : (position - 60) / 8;
	if (offset > extra) offset = extra;
	if (position > 60 + extra * 8) {
		offset = extra - (position - 60 - extra * 8) / 8;
		if (offset < 0) offset = 0;
	}
	if (offset == oldNameOffset) return;
	oldNameOffset = offset;
	while (offset-- > 0 && *name) uiNextCodepoint(&name);
	int y = UI_ROW_TOP + row * UI_ROW_HEIGHT;
	uiFillRect(16, y + 5, 224, 14, UI_ACTIVE);
	uiText(name, 16, y + 16, UI_INK, 240);
	uiCanvasPresent();
}

void cls(int chrMap) { (void)chrMap; uiCanvasClear(UI_BG); }

int drawItem(const char *str, int col, int row, int palette) {
	uiText(uiTranslate(str), col * 8, row * 8 + 10, palette ? UI_VALUE : UI_INK, 256);
	uiCanvasPresent();
	return col + (uiTextWidth(str) + 7) / 8;
}

void drawText(const char *str, int row, int palette) {
	if (row == 0) {
		strlcpy(browserTitle, str, sizeof(browserTitle));
		browserActive = true;
		uiHeader(browserTitle, selectedMain, 1, 1);
		uiFooter(false);
	} else {
		const char *text = uiTranslate(str);
		while (*text == ' ') text++;
		int x = 128 - uiTextWidth(text) / 2;
		if (x < 12) x = 12;
		uiFillRect(0, row * 8 - 2, 256, 16, UI_BG);
		uiTextEllipsis(text, x, row * 8 + 10, palette ? UI_VALUE : UI_INK, 244);
	}
	uiCanvasPresent();
}
void drawMenuText(const char *text, int row, int pal) { drawItem(text, 1, row, pal); }
void drawSubText(const char *text, int row, int pal) { drawItem(text, 2, row, pal); }

void strlMerge(char *dst, const char *first, const char *second, int size) {
	if (dst != first) strlcpy(dst, first, size);
	strlcat(dst, second, size);
}

void drawStrings(const char *first, const char *second, int col, int row, int pal) {
	char joined[256];
	strlcpy(joined, uiTranslate(first), sizeof(joined));
	if (second) {
		strlcat(joined, " ", sizeof(joined));
		strlcat(joined, uiTranslate(second), sizeof(joined));
	}
	drawItem(joined, col, row, pal);
}

void drawMenuItem(const char *text) { drawSubItem(text, NULL); }
void drawSubItem(const char *label, const char *value) {
	uiDrawEntry(label, value, menuItemRow - menuYOffset, selected == menuItemRow);
	menuItemRow++;
}
void setMenuItemRow(int row) { menuItemRow = row; }

void int2Str(int i, char *s) {
	u32 j = bin2BCD(i);
	for (int k = 28; k >= 0; k -= 4) {
		int mod = (j>>k) & 15;
		*(s++) = (mod + '0');
	}
	*(s++) = 0;
}

void char2Str(char *dst, int val) {
	u32 j = bin2BCD(val);
	for (int k = 8; k >= 0; k -= 4) {
		int mod = (j>>k) & 15;
		*(dst++) = (mod + '0');
	}
	*(dst++) = 0;
}

void int2HexStr(char *dest, int val) {
	for (int i = 0; i < 8; i++) {
		dest[7-i] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
		val = val>>4;
	}
	dest[8] = 0;
}

void short2HexStr(char *dest, short val) {
	for (int i = 0; i < 4; i++) {
		dest[3-i] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
		val = val>>4;
	}
	dest[4] = 0;
}

void char2HexStr(char *dest, char val) {
	dest[0] = ((val>>4) & 0xF) + ((((val>>4) & 0xF) < 10) ? '0' : '7');
	dest[1] = (val & 0xF) + (((val & 0xF) < 10) ? '0' : '7');
	dest[2] = 0;
}

void updateInfoLog(void) {
	if (logBufPtrOld != logBufPtr) {
		logBufPtrOld = logBufPtr;
		logTimer = 120;
		outputLogToScreen();
		uiCanvasPresent();
	}
	if (logTimer > 0 && !--logTimer) {
		logBuffer[(logBufPtr - 1) & 7][0] = 0;
		if (!browserActive) redrawUI();
	}
}

void outputLogToScreen(void) {
	if (!logTimer || logBufPtr <= 0) return;
	const char *text = uiTranslate(logBuffer[(logBufPtr - 1) & 7]);
	if (!*text) return;
	uiFillRect(8, 132, 240, 36, UI_TOP);
	uiFillRect(8, 132, 240, 1, UI_RULE);
	char line[256];
	int lineNumber = 0;
	while (*text && lineNumber < 2) {
		unsigned length = 0;
		while (*text) {
			const char *next = text;
			uiNextCodepoint(&next);
			size_t bytes = next - text;
			if (length + bytes >= sizeof(line)) break;
			memcpy(line + length, text, bytes);
			line[length + bytes] = 0;
			if (uiTextWidth(line) > 224) break;
			length += bytes;
			text = next;
		}
		line[length] = 0;
		uiText(line, 16, 145 + lineNumber * 14, UI_INK, 240);
		lineNumber++;
	}
}

void infoOutput(const char *text) {
	strlcpy(logBuffer[logBufPtr++ & 7], text, sizeof(logBuffer[0]));
}

void debugOutput(const char *str) {
	if (gDebugSet) {
		infoOutput(str);
	}
	debugOutputToEmulator(str);
}

void exitEmulator() {
	if (enableExit) {
		exit(0);
	}
}

char *updateTime() {
	time_t unixTime = time(NULL);
	if (oldTime != unixTime) {
		oldTime = unixTime;
		struct tm *timeStruct = gmtime((const time_t *)&unixTime);
		timeBuffer[0] = bin2BCD(timeStruct->tm_hour);
		timeBuffer[1] = bin2BCD(timeStruct->tm_min);
		timeBuffer[2] = bin2BCD(timeStruct->tm_sec);
		timeBuffer[3] = bin2BCD(timeStruct->tm_year);
		timeBuffer[4] = bin2BCD(timeStruct->tm_mon + 1);
		timeBuffer[5] = bin2BCD(timeStruct->tm_mday);
		if (emuSettings & SHOW_UI_CLOCK) {
			drawClock();
		}
	}
	return timeBuffer;
}

void drawClock() {
	if (isMenuOpen()) return;
	char str[9];

	strlcpy(str, "00:00:00", sizeof(str));
	str[0] = ((timeBuffer[0] >> 4) & 0x3) + '0'; // Hours.
	str[1] = (timeBuffer[0] & 0xF) + '0';
	str[3] = ((timeBuffer[1] >> 4) & 0xF) + '0'; // Minutes.
	str[4] = (timeBuffer[1] & 0xF) + '0';
	str[6] = ((timeBuffer[2] >> 4) & 0xF) + '0'; // Seconds.
	str[7] = (timeBuffer[2] & 0xF) + '0';

	drawItem(str, 23, 0, 0);
}

//---------------------------------------------

void autoPauseGameSet() {
	emuSettings ^= AUTOPAUSE_EMULATION;
	settingsChanged = true;
	pauseEmulation = (emuSettings & AUTOPAUSE_EMULATION);
	soundSetMuteGUI();
}

const char *getAutoPauseGameText() {
	return autoTxt[emuSettings & AUTOPAUSE_EMULATION];
}

void autoStateSet() {
	emuSettings ^= AUTOLOAD_STATE;
	settingsChanged = true;
}

const char *getAutoStateText() {
	return autoTxt[(emuSettings & AUTOLOAD_STATE)>>2];
}

void autoSettingsSet() {
	emuSettings ^= AUTOSAVE_SETTINGS;
	settingsChanged = true;
}

const char *getAutoSettingsText() {
	return autoTxt[(emuSettings & AUTOSAVE_SETTINGS)>>1];
}

void autoNVRAMSet() {
	emuSettings ^= AUTOLOAD_NVRAM;
	settingsChanged = true;
}

const char *getAutoNVRAMText() {
	return autoTxt[(emuSettings & AUTOLOAD_NVRAM)>>3];
}

void saveNVRAMSet() {
	emuSettings ^= AUTOSAVE_NVRAM;
	settingsChanged = true;
}

const char *getSaveNVRAMText() {
	return autoTxt[(emuSettings & AUTOSAVE_NVRAM)>>4];
}

void showClockSet() {
	emuSettings ^= SHOW_UI_CLOCK;
	settingsChanged = true;
}

const char *getShowClockText() {
	return autoTxt[(emuSettings & SHOW_UI_CLOCK)>>5];
}

void debugTextSet() {
	gDebugSet ^= true;
}

const char *getDebugText() {
	return autoTxt[gDebugSet & 1];
}

void sleepSet() {
	int i = (emuSettings+0x100) & AUTOSLEEP_MASK;
	emuSettings = (emuSettings & ~AUTOSLEEP_MASK) | i;
	if (i == AUTOSLEEP_5MIN) {
		sleepTime = 60*60*5;		// 5 min
	}
	else if (i == AUTOSLEEP_10MIN) {
		sleepTime = 60*60*10;		// 10 min
	}
	else if (i == AUTOSLEEP_30MIN) {
		sleepTime = 60*60*30;		// 30 min
	}
	else if (i == AUTOSLEEP_OFF) {
		sleepTime = 0x7F000000;		// 360 days...
	}
	settingsChanged = true;
}

const char *getSleepText() {
	return sleepTxt[(emuSettings & AUTOSLEEP_MASK)>>8];
}

void powerSaveSet() {
	emuSettings ^= POWER_SAVE_MENU;
	settingsChanged = true;
}

const char *getPowerSaveText() {
	return autoTxt[(emuSettings & POWER_SAVE_MENU)>>10];
}

void screenSwapSet() {
	emuSettings ^= MAIN_ON_BOTTOM;
	settingsChanged = true;
}

const char *getScreenSwapText() {
	return autoTxt[(emuSettings & MAIN_ON_BOTTOM)>>11];
}

void gammaSet() {
	gGammaValue++;
	if (gGammaValue > 4) gGammaValue = 0;
	settingsChanged = true;
}

const char *getGammaText() {
	return brighTxt[gGammaValue];
}

void autoASet() {
	autoA++;
	joyCfg |= KEY_A+(KEY_A<<16);
	if (autoA == 1) {
		joyCfg &= ~KEY_A;
	}
	else if (autoA == 2) {
		joyCfg &= ~(KEY_A<<16);
	}
	else {
		autoA = 0;
	}
}

const char *getAutoAText() {
	return autoTxt[autoA];
}

void autoBSet() {
	autoB++;
	joyCfg |= KEY_B+(KEY_B<<16);
	if (autoB == 1) {
		joyCfg &= ~KEY_B;
	}
	else if (autoB == 2) {
		joyCfg &= ~(KEY_B<<16);
	}
	else {
		autoB = 0;
	}
}

const char *getAutoBText() {
	return autoTxt[autoB];
}

void speedSet() {
	int i = (emuSettings+0x40) & EMUSPEED_MASK;
	emuSettings = (emuSettings & ~EMUSPEED_MASK) | i;
	setEmuSpeed(i>>6);
}

const char *getSpeedText() {
	return speedTxt[(emuSettings & EMUSPEED_MASK)>>6];
}

void flickSet() {
	gFlicker++;
	if (gFlicker > 1) {
		gFlicker = 0;
		gTwitch = 0;
	}
}

const char *getFlickText() {
	return flickTxt[gFlicker & 1];
}

void uiLanguageChanged(void) {
	memset(logBuffer, 0, sizeof(logBuffer));
	logTimer = 0;
	logBufPtrOld = logBufPtr;
	uiCanvasClear(UI_BG);
}
