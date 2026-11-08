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
#include "io.h"
#include "cpu.h"
#include "bios.h"
#include "Sound.h"
#include "TLCS900H/Version.h"
#include "ARMZ80/Version.h"
#include "K2GE/Version.h"
#include "K2Audio/Version.h"

#define EMUVERSION "V0.6.0 2026-09-17"

void hacksInit(void);

static void uiLanguageSet(void);
static void debugStepFrame(void);
static const char *getUiLanguageText(void);
static const char *getBufferModeText(void);
static void gammaChange(void);
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
	{"Settings", ui7},
	{"Debug", ui8},
};
const MItem ctrlItems[] = {
	{"B Autofire:", autoBSet, getAutoBText},
	{"A Autofire:", autoASet, getAutoAText},
	{"Swap A-B:  ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Gamma:", gammaChange, getGammaText},
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
const MItem setItems[] = {
	{"Interface Language:", uiLanguageSet, getUiLanguageText},
	{"Autosave Flash:", saveNVRAMSet, getSaveNVRAMText},
	{"Autosave Settings:", autosaveSettingsSet, getAutoSettingsText},
	{"Autoload Flash:", autoNVRAMSet, getAutoNVRAMText},
	{"Menu Screen Off:", powerSaveSet, getPowerSaveText},
	{"Autoload State:", autoStateSet, getAutoStateText},
	{"Autopause Game:", autoPauseGameSet, getAutoPauseGameText},
	{"Emulator on Bottom:", screenSwapSet, getScreenSwapText},
	{"Speed:", speedSet, getSpeedText},
	{"Autosleep:", sleepSet, getSleepText},
};
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

extern char gameInfoString[32];
static const char *aboutVersion(void) { return EMUVERSION; }
static const char *aboutAuthor(void) { return "FluBBaOfWard"; }
static const char *aboutGame(void) {
	if (!gameInserted) return "No game loaded";
	updateGameInfo();
	return gameInfoString + 11;
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
	{"Original Author", uiDummy, aboutAuthor},
	{"Game", uiDummy, aboutGame},
	{"B Button", uiDummy, aboutA},
	{"A Button", uiDummy, aboutB},
	{"L Button", uiDummy, aboutD},
	{"X / Start", uiDummy, aboutOption},
	{"Y / Select", uiDummy, aboutPower},
	{"D-Pad", uiDummy, aboutPad},
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
const Menu menu7 = MENU_M("Settings", uiAuto, setItems);
const Menu menu8 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu9 = MENU_M("Quit Emulator?", uiAuto, quitItems);
const Menu menu10 = MENU_M("", uiDummy, dummyItems);

const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6, &menu7, &menu8, &menu9, &menu10 };

static void debugStepFrame(void) {
	if (!gameInserted && !g_BIOSBASE_BNW && !g_BIOSBASE_COLOR) {
		infoOutput("Load a game first.");
		return;
	}
	stepFrame();
}

static void uiLanguageSet(void) {
	gUiLanguage = (gUiLanguage + 1) % UI_LANGUAGE_COUNT;
	settingsChanged = true;
	uiLanguageChanged();
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

void resetConsole() {
	checkMachine();
	resetCartridgePreservingFlash(false);
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

/// Change gamma (brightness)
void gammaChange() {
	gammaSet();
	paletteInit(gGammaValue);
	paletteTxAll();					// Make new palette visible
	setupMenuPalette();
	settingsChanged = true;
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

void paletteChange() {
	gPaletteBank++;
	if (gPaletteBank > 4) {
		gPaletteBank = 0;
	}
	monoPalInit(gPaletteBank);
	paletteTxAll();
	fixBiosSettings();
	settingsChanged = true;
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

void machineSet() {
	gMachineSet++;
	if (gMachineSet >= HW_SELECT_END) {
		gMachineSet = 0;
	}
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

void z80SpeedSet() {
	gZ80Speed++;
	if (gZ80Speed >= 5) {
		gZ80Speed = 0;
	}
	tweakZ80Speed(gZ80Speed);
}
const char *getZ80SpeedText() {
	return cpuSpeedTxt[gZ80Speed&7];
}
