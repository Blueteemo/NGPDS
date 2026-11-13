#include <nds.h>

#include "Gui.h"
#include "UiText.h"
#include "UiCanvas.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Emubase.h"
#include "Main.h"
#include "FileHandling.h"
#include "Cart.h"
#include "Gfx.h"
#include "NeoGeoPocket.h"
#include "io.h"
#include "cpu.h"
#include "bios.h"
#include "Sound.h"
#include "TLCS900H/Version.h"
#include "ARMZ80/Version.h"
#include "K2GE/Version.h"
#include "K2Audio/Version.h"

#define EMUVERSION "V0.6.0 2026-09-17"
#define FUSION_VERSION "V1.0.0"
#define FUSION_BUILD_DATE "2026-11-13"

void hacksInit(void);

static void uiLanguageSet(void);
static void debugStepFrame(void);
static const char *getUiLanguageText(void);
static const char *getBufferModeText(void);
static void aboutControls(void);
static void aboutComponents(void);
static void gammaChange(void);
static void colorModeChange(void);
static const char *getColorModeText(void);
static void borderModeChange(void);
static const char *getBorderModeText(void);
static void paletteChange(void);
static const char *getPaletteText(void);
static void bufferModeSet(void);
static void languageSet(void);
static const char *getLanguageText(void);
static void machineSet(void);
static const char *getMachineText(void);
static void batteryChange(void);
static void subBatteryChange(void);
static void speedHackSet(void);
static const char *getSpeedHackText(void);
static void z80SpeedSet(void);
static const char *getZ80SpeedText(void);
static void swapABSet(void);
static const char *getSwapABText(void);
static void fgrLayerSet(void);
static const char *getFgrLayerText(void);
static void bgrLayerSet(void);
static const char *getBgrLayerText(void);
static void sprLayerSet(void);
static const char *getSprLayerText(void);

static void uiDisplay(void);
static void uiMachine(void);
static void audioModeSet(void);
static const char *getAudioModeText(void);
static void updateGameInfo(void);
static void checkBattery(void);


const MItem dummyItems[] = {
	{"", uiDummy}
};
const MItem fileItems[] = {
	{"Load Game", selectGame},
	{"Load State", loadState},
	{"Save State", saveState},
	{"Load Flash", loadNVRAM},
	{"Save Flash", saveNVRAM},
	{"Save Settings", saveSettingsManual},
	{"Eject Game", ejectCart},
	{"Reset Console", resetConsole},
	{"Quit Emulator", ui9},
};
const MItem optionItems[] = {
	{"Controller", ui4},
	{"Display", ui5},
	{"Machine", ui6},
	{"General", ui7},
	{"Debug", ui8},
};
const MItem ctrlItems[] = {
	{"B Autofire:", autoBSet, getAutoBText},
	{"A Autofire:", autoASet, getAutoAText},
	{"Swap A-B:  ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Gamma:", gammaChange, getGammaText},
	{"Color Mode:", colorModeChange, getColorModeText},
	{"Screen Border:", borderModeChange, getBorderModeText},
	{"B&W Palette:", paletteChange, getPaletteText},
	{"VRAM Double Buffer:", bufferModeSet, getBufferModeText},
};
const MItem machineItems[] = {
	{"Console Language:", languageSet, getLanguageText},
	{"Machine:", machineSet, getMachineText},
//	{"NGPC B&W Palette:", ngpcPalSet, getNgpPalText},
	{"Change Batteries", batteryChange},
	{"Change Sub Battery", subBatteryChange},
	{"Cpu Speed Hacks:", speedHackSet, getSpeedHackText},
	{"Z80 Clock:", z80SpeedSet, getZ80SpeedText},
	{"Select BnW Bios", selectBnWBios},
	{"Select Color Bios", selectColorBios},
};
#define GENERAL_BEFORE_GAME_INPUT \
	{"Interface Language:", uiLanguageSet, getUiLanguageText}, \
	{"Audio Mode:", audioModeSet, getAudioModeText}, \
	{"Autosave Flash:", saveNVRAMSet, getSaveNVRAMText}, \
	{"Autosave Settings:", autosaveSettingsSet, getAutoSettingsText}, \
	{"Autoload Flash:", autoNVRAMSet, getAutoNVRAMText}, \
	{"Menu Screen Off:", powerSaveSet, getPowerSaveText}, \
	{"Autoload State:", autoStateSet, getAutoStateText}, \
	{"Autopause Game:", autoPauseGameSet, getAutoPauseGameText}

#define GENERAL_AFTER_GAME_INPUT \
	{"Emulator on Bottom:", screenSwapSet, getScreenSwapText}, \
	{"Speed:", speedSet, getSpeedText}, \
	{"Autosleep:", sleepSet, getSleepText}

static const MItem pausedSettings[] = {
	GENERAL_BEFORE_GAME_INPUT,
	GENERAL_AFTER_GAME_INPUT,
};
static const MItem liveSettings[] = {
	GENERAL_BEFORE_GAME_INPUT,
	{"Game Control in Menu:", gameInputInMenuSet, getGameInputInMenuText},
	GENERAL_AFTER_GAME_INPUT,
};
#undef GENERAL_BEFORE_GAME_INPUT
#undef GENERAL_AFTER_GAME_INPUT
const MItem debugItems[] = {
	{"Debug Output:", debugTextSet, getDebugText},
	{"Disable Foreground:", fgrLayerSet, getFgrLayerText},
	{"Disable Background:", bgrLayerSet, getBgrLayerText},
	{"Disable Sprites:", sprLayerSet, getSprLayerText},
	{"Step Frame", debugStepFrame},
};
const MItem quitItems[] = {
	{"Yes ", exitGameSafely},
	{"No ", backOutOfMenu},
};

static const char *aboutVersion(void) { return EMUVERSION; }
static const char *aboutForkVersion(void) { return FUSION_VERSION " " FUSION_BUILD_DATE; }
static const char *aboutAuthor(void) { return "FluBBaOfWard"; }
static const char *aboutMaintainer(void) {
	return gUiLanguage == UI_LANGUAGE_ZH_CN ? "千岚之夏" : "Blueteemo";
}
static const char *aboutA(void) { return "NGP A"; }
static const char *aboutB(void) { return "NGP B"; }
static const char *aboutD(void) { return "NGP D"; }
static const char *aboutOption(void) { return "Option button"; }
static const char *aboutPower(void) { return "Power button"; }
static const char *aboutPad(void) { return "Joystick"; }
static const char *aboutZ80(void) { return ARMZ80VERSION; }
static const char *aboutCpu(void) { return TLCS900VERSION; }
static const char *aboutGpu(void) { return K2GEVERSION; }
static const char *aboutPsg(void) { return ARMSNGGVERSION; }
static const char *aboutFont(void) { return "Fusion Pixel 12px"; }
const MItem aboutItems[] = {
	{"NGPDS", uiDummy, aboutVersion},
	{"Fusion Version", uiDummy, aboutForkVersion},
	{"Original Author", uiDummy, aboutAuthor},
	{"Fork Maintainer", uiDummy, aboutMaintainer},
	{"Button Guide", aboutControls},
	{"Components & Font", aboutComponents},
};
const MItem aboutControlItems[] = {
	{"B Button", uiDummy, aboutA},
	{"A Button", uiDummy, aboutB},
	{"L Button", uiDummy, aboutD},
	{"X / Start", uiDummy, aboutOption},
	{"Y / Select", uiDummy, aboutPower},
	{"D-Pad", uiDummy, aboutPad},
};
const MItem aboutComponentItems[] = {
	{"ARMZ80", uiDummy, aboutZ80},
	{"TLCS-900H", uiDummy, aboutCpu},
	{"K2GE", uiDummy, aboutGpu},
	{"SN76496", uiDummy, aboutPsg},
	{"UI Font", uiDummy, aboutFont},
};

const Menu menu0 = MENU_M("", uiNullNormal, dummyItems);
Menu menu1 = MENU_M("Files & Saves", uiAuto, fileItems);
const Menu menu2 = MENU_M("Options", uiAuto, optionItems);
const Menu menu3 = MENU_M("About", uiAbout, aboutItems);
const Menu menu4 = MENU_M("Controller Settings", uiAuto, ctrlItems);
const Menu menu5 = MENU_M("Display Settings", uiDisplay, displayItems);
const Menu menu6 = MENU_M("Machine Settings", uiMachine, machineItems);
static const Menu pausedGeneral = MENU_M("General", uiAuto, pausedSettings);
static const Menu liveGeneral = MENU_M("General", uiAuto, liveSettings);
const Menu menu8 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu9 = MENU_M("Quit Emulator?", uiAuto, quitItems);
const Menu menu10 = MENU_M("", uiDummy, dummyItems);
const Menu menu11 = MENU_M("Button Guide", uiAuto, aboutControlItems);
const Menu menu12 = MENU_M("Components & Font", uiAuto, aboutComponentItems);

static const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6,
	&pausedGeneral, &menu8, &menu9, &menu10, &menu11, &menu12 };

const Menu *getMenu(int menu) {
	if (menu == 7 && !(emuSettings & AUTOPAUSE_EMULATION)) return &liveGeneral;
	return menus[menu];
}

static void aboutControls(void) { enterMenu(11); }
static void aboutComponents(void) { enterMenu(12); }

static void adjustAudioMode(int direction) {
	static const unsigned modeFlags[] = {COMPATIBLE_AUDIO, LEGACY_AUDIO};
	int mode = cycleSetting(soundModeFromSettings(emuSettings), AUDIO_MODE_COUNT, direction);
	emuSettings = (emuSettings & ~AUDIO_MODE_MASK) | modeFlags[mode];
	soundSetMode(mode);
	settingsChanged = true;
}

static void audioModeSet(void) {
	adjustAudioMode(1);
}

static const char *getAudioModeText(void) {
	static const char *const names[] = {"Standard Audio", "Classic Audio"};
	return names[soundModeFromSettings(emuSettings)];
}

bool hasConsoleProgram(void) {
	/* A BIOS for the other machine cannot run the active HLE console. */
	return gameInserted || (gMachine == HW_NGPMONO ?
		g_BIOSBASE_BNW != NULL : g_BIOSBASE_COLOR != NULL);
}

static void debugStepFrame(void) {
	if (!hasConsoleProgram()) {
		infoOutput("Load a game first.");
		return;
	}
	stepFrame();
}

static void adjustUiLanguage(int direction) {
	gUiLanguage = cycleSetting(gUiLanguage, UI_LANGUAGE_COUNT, direction);
	settingsChanged = true;
	uiLanguageChanged();
}

static void uiLanguageSet(void) {
	adjustUiLanguage(1);
}

static const char *getUiLanguageText(void) {
	static const char *const names[] = {"English", "简体中文", "日本語"};
	return names[gUiLanguage < UI_LANGUAGE_COUNT ? gUiLanguage : 0];
}

static const char *getBufferModeText(void) {
	return autoTxt[gBufferEnable & 1];
}

static int oldBattery;
u8 gZ80Speed = 0;
char gameInfoString[32];

const char *const ctrlTxt[]  = {"1P", "2P"};
const char *const dispTxt[]  = {"Unscaled", "Scaled"};

const char *const machTxt[]  = {"Auto", "NeoGeo Pocket", "NeoGeo Pocket Color"};
const char *const bordTxt[]  = {"Black", "Border Color", "None"};
const char *const palTxt[]   = {"Black & White", "Red", "Green", "Blue", "Classic"};
const char *const langTxt[]  = {"Japanese", "English"};
const char *const cpuSpeedTxt[]  = {"Full Speed", "Half Speed", "1/4 Speed", "1/8 Speed", "1/16 Speed"};

/// This is called at the start of the emulator
void setupGUI() {
	keysSetRepeat(25, 4);	// Delay, repeat.
	menu1.itemCount = ARRSIZE(fileItems) - (enableExit?0:1);
	openMenu();
}

/// This is called when going from emu to ui.
void enterGUI() {
    if(emuSettings&AUTOPAUSE_EMULATION){pauseEmulation=true;soundSetMuteGUI();}
    updateSettingsFromNGP();
    if(emuSettings&AUTOSAVE_SETTINGS)saveSettings();
}
void exitGUI() {
    /* Compare actual config bytes, including BIOS paths and unflagged input
     * changes. No rewrite if identical, independent of Shared's dirty flag. */
    if(emuSettings&AUTOSAVE_SETTINGS)saveSettings();
}

void quickSelectGame(void) {
	openMenu();
	selectGame();
	closeMenu();
}

void uiNullNormal(void) {
	uiNullDefault();
	oldBattery = 0;
}

void uiAbout(void) { uiAuto(); }

void uiDisplay(void) { uiAuto(); }

static void uiMachine(void) { uiAuto(); }

void checkBattery(void) {
	if (oldBattery != batteryLevel) {
		oldBattery = batteryLevel;
		if (batteryLevel < 0x8400) infoOutput("NGP battery low.");
	}
}

void nullUINormal(int key) {
	checkBattery();
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void nullUIDebug(int key) {
	checkBattery();
	if (key & KEY_TOUCH) {
		openMenu();
	}
}

void updateGameInfo() {
	char catalog[8];
	strlMerge(gameInfoString, "Game Name: ", ngpHeader->name, sizeof(gameInfoString));
	strlcat(gameInfoString, " #", sizeof(gameInfoString));
	short2HexStr(catalog, ngpHeader->catalog);
	strlcat(gameInfoString, catalog, sizeof(gameInfoString));
}
//---------------------------------------------------------------------------------
void debugIO(u16 port, u8 val, const char *message) {
	char debugString[32];

	debugString[0] = 0;
	strlcat(debugString, message, sizeof(debugString));
	short2HexStr(&debugString[strlen(debugString)], port);
	strlcat(debugString, " val:", sizeof(debugString));
	char2HexStr(&debugString[strlen(debugString)], val);
	debugOutput(debugString);
}
//---------------------------------------------------------------------------------
void debugIOUnimplR(u16 port) {
	debugIO(port, 0, "Unimpl R port:");
}
void debugIOUnimplW(u8 val, u16 port) {
	debugIO(port, val, "Unimpl W port:");
}
void debugDivideError() {
	debugOutput("Divide Error.");
}
void debugUndefinedInstruction() {
	debugOutput("Undefined Instruction.");
}
void debugCrashInstruction() {
	debugOutput("CPU Crash!");
}

//---------------------------------------------------------------------------------
/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}
const char *getSwapABText() {
	return autoTxt[(joyCfg>>10)&1];
}

static void updateDisplayPalette(void) {
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
}

/// Change gamma (brightness)
static void adjustGamma(int direction) {
	adjustMenuGamma(direction);
	updateDisplayPalette();
	settingsChanged = true;
}

static void gammaChange(void) {
	adjustGamma(1);
}

static void adjustColorMode(int direction) {
	int mode = cycleSetting(getColorMode(), COLOR_MODE_COUNT, direction);
	emuSettings = (emuSettings & ~COLOR_MODE_MASK) | (mode << COLOR_MODE_SHIFT);
	updateDisplayPalette();
	settingsChanged = true;
}

static void colorModeChange(void) {
	adjustColorMode(1);
}

static const char *getColorModeText(void) {
	static const char *const names[] = {"Original", "Mild Vivid", "Vivid"};
	return names[getColorMode()];
}

static void adjustBorderMode(int direction) {
	int mode = cycleSetting(getBorderMode(), BORDER_MODE_COUNT, direction);
	emuSettings = (emuSettings & ~BORDER_MODE_MASK) | (mode << BORDER_MODE_SHIFT);
	setupEmuBackground();
	settingsChanged = true;
}

static void borderModeChange(void) {
	adjustBorderMode(1);
}

static const char *getBorderModeText(void) {
	static const char *const names[] = {"Graphite", "Classic", "Hidden"};
	return names[getBorderMode()];
}

/// Turn on/off rendering of foreground
void fgrLayerSet(){
	gGfxMask ^= 0x01;
}
const char *getFgrLayerText() {
	return autoTxt[gGfxMask&1];
}
/// Turn on/off rendering of background
void bgrLayerSet(){
	gGfxMask ^= 0x02;
}
const char *getBgrLayerText() {
	return autoTxt[(gGfxMask>>1)&1];
}
/// Turn on/off rendering of sprites
void sprLayerSet(){
	gGfxMask ^= 0x10;
}
const char *getSprLayerText() {
	return autoTxt[(gGfxMask>>4)&1];
}

static void adjustPalette(int direction) {
	gPaletteBank = cycleSetting(gPaletteBank, 5, direction);
	monoPalInit(gPaletteBank);
	paletteTxAll();
	fixBiosSettings();
	settingsChanged = true;
}
static void paletteChange(void) {
	adjustPalette(1);
}
const char *getPaletteText() {
	return gPaletteBank<5?palTxt[gPaletteBank]:"BIOS palette";
}

void bufferModeSet() {
	gBufferEnable ^= 0x01;
	k2GE_0EnableBufferMode(gBufferEnable);
}
/*
void borderSet() {
	bcolor++;
	if (bcolor > 2) {
		bcolor = 0;
	}
	makeborder();
}
*/
void languageSet() {
	gLang ^= 0x01;
	fixBiosSettings();
}
const char *getLanguageText() {
	return langTxt[gLang];
}

static void adjustMachine(int direction) {
	gMachineSet = cycleSetting(gMachineSet, HW_SELECT_END, direction);
}
static void machineSet(void) {
	adjustMachine(1);
}
const char *getMachineText() {
	return machTxt[gMachineSet];
}

void batteryChange() {
	batteryLevel = 0xFFFF;				// 0xFFFF for 2 days battery?
}

void subBatteryChange() {
	gSubBatteryLevel = 0x3FFFFFF;		// 0x3FFFFFF for 2 years battery?
}

void speedHackSet() {
	emuSettings ^= ALLOW_SPEED_HACKS;
	hacksInit();
}
const char *getSpeedHackText() {
	return autoTxt[(emuSettings & ALLOW_SPEED_HACKS)>>17];
}

static void adjustZ80Speed(int direction) {
	gZ80Speed = cycleSetting(gZ80Speed, ARRSIZE(cpuSpeedTxt), direction);
	tweakZ80Speed(gZ80Speed);
}
static void z80SpeedSet(void) {
	adjustZ80Speed(1);
}
const char *getZ80SpeedText() {
	return cpuSpeedTxt[gZ80Speed&7];
}

void changeMenuValue(int menu, int row, int direction) {
	const MItem *item = &getMenu(menu)->items[row];
	if (!item->txt2) {
		return;
	}
	static const struct {
		fptr action;
		void (*adjust)(int direction);
	} cycles[] = {
		{autoASet, adjustMenuAutoA},
		{autoBSet, adjustMenuAutoB},
		{gammaChange, adjustGamma},
		{colorModeChange, adjustColorMode},
		{borderModeChange, adjustBorderMode},
		{paletteChange, adjustPalette},
		{machineSet, adjustMachine},
		{z80SpeedSet, adjustZ80Speed},
		{uiLanguageSet, adjustUiLanguage},
		{audioModeSet, adjustAudioMode},
		{speedSet, adjustMenuSpeed},
		{sleepSet, adjustMenuSleep},
	};
	for (unsigned i = 0; i < ARRSIZE(cycles); i++) {
		if (item->fn == cycles[i].action) {
			cycles[i].adjust(direction);
			return;
		}
	}
	/* The remaining value rows in Options are two-state toggles. */
	item->fn();
}
