#include <nds.h>
#include <maxmod9.h>

#include "Main.h"
#include "UiText.h"
#include "UiCanvas.h"
#include "Shared/EmuMenu.h"
#include "Shared/FileHelper.h"
#include "Shared/AsmExtra.h"
#include "Gui.h"
#include "FileHandling.h"
#include "Cart.h"
#include "cpu.h"
#include "Gfx.h"
#include "io.h"
#include "Sound.h"
#include "bios.h"

static void checkTimeOut(void);
static void setupGraphics(void);
static void setupStream(void);

bool gameInserted = false;
static int sleepTimer = 60*60*5;	// 5 min
static volatile bool vBlankOverflow = false;

static mm_ds_system sys;
static mm_stream myStream;

u16 *map0sub;
u16 *map1sub;

//---------------------------------------------------------------------------------
void myVblank(void) {
//---------------------------------------------------------------------------------
	vBlankOverflow = true;
//	DC_FlushRange(EMUPALBUFF, 0x400);
	publishEmuBackground();
	vblIrqHandler();
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	if (argc > 1) {
		enableExit = true;
	}
	// Try to allocate 4MB on DSi
	allocatedRomMemSize = 0x400000 + 0x1000;
	allocatedRomMem = malloc(allocatedRomMemSize);
	if (allocatedRomMem == NULL) {
		// Try to allocate 2MB on DS
		allocatedRomMemSize = 0x200000 + 0x1000;
		allocatedRomMem = malloc(allocatedRomMemSize);
	}
	maxRomSize = allocatedRomMemSize;
	romSpacePtr = allocatedRomMem;

	setupGraphics();
	setupStream();
	irqSet(IRQ_VBLANK, myVblank);
	setupGUI();
	initSettings();
	bool fsOk = initFileHelper();
	loadSettings();
	if (fsOk) {
		loadBnWBIOS();
		loadColorBIOS();
		redrawUI();
	}
	else {
		infoOutput("fatInitDefault() failure.");
	}
	if (g_BIOSBASE_BNW == NULL) {
		installHleBios(biosSpace);
	}
	if (g_BIOSBASE_COLOR == NULL) {
		installHleBios(biosSpaceColor);
	}
	checkMachine();
	machineInit();
	setupEmuBackground();
	if (argc > 1) {
		loadGame(argv[1]);
		soundSetMuteGUI();
	}
	scanKeys();
	getInput();

	while (1) {
		waitVBlank();
		guiRunLoop();
		runEmulationFrame();
//		checkTimeOut();
	}
	return 0;
}

void runEmulationFrame(void) {
	/* An empty HLE console has no standalone CPU program to execute. */
	if (!hasConsoleProgram() || !persistenceCanRun()) {
		if (!pauseEmulation) {
			pauseEmulation = true;
			soundSetMuteGUI();
		}
	}
	if (!pauseEmulation) {
		run();
	}
}

//---------------------------------------------------------------------------------
void pausVBlank(int count) {
//---------------------------------------------------------------------------------
	while (--count) {
		waitVBlank();
	}
}

//---------------------------------------------------------------------------------
void waitVBlank() {
//---------------------------------------------------------------------------------
	// Workaround for bug in Bios.
	if (!vBlankOverflow) {
		swiIntrWait(1, IRQ_VBLANK);
	}
	vBlankOverflow = false;
	/* Sample once per UI update. A VBlank during emulation must not
	 * replace a key-down edge before the menu can read it. */
	scanKeys();
}

//---------------------------------------------------------------------------------
static void checkTimeOut() {
//---------------------------------------------------------------------------------
	if (EMUinput) {
		sleepTimer = sleepTime;
	}
	else {
		sleepTimer--;
		if (sleepTimer < 0) {
			sleepTimer = sleepTime;
			// systemSleep doesn't work as expected.
			//systemSleep();	
		}
	}
}

//---------------------------------------------------------------------------------
void setEmuSpeed(int speed) {
//---------------------------------------------------------------------------------
	if (speed == 0) {		// Normal Speed
		waitMaskIn = 0x00;
		waitMaskOut = 0x00;
	}
	else if (speed == 1) {	// Double speed
		waitMaskIn = 0x00;
		waitMaskOut = 0x01;
	}
	else if (speed == 2) {	// Max speed (4x)
		waitMaskIn = 0x00;
		waitMaskOut = 0x03;
	}
	else if (speed == 3) {	// 50% speed
		waitMaskIn = 0x01;
		waitMaskOut = 0x00;
	}
}

//---------------------------------------------------------------------------------
static void setupGraphics() {
//---------------------------------------------------------------------------------

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
	vramSetBankC(VRAM_C_MAIN_BG_0x06040000);
	vramSetBankD(VRAM_D_MAIN_BG_0x06060000);
	vramSetBankE(VRAM_E_MAIN_SPRITE);
	// Extend native OBJ tiles into unused F/G banks for the sparse raster atlas.
	vramSetBankF(VRAM_F_MAIN_SPRITE_0x06410000);
	vramSetBankG(VRAM_G_MAIN_SPRITE_0x06414000);
	vramSetBankH(VRAM_H_SUB_BG);
	vramSetBankI(VRAM_I_SUB_SPRITE);

	// Set up the main display
	GFX_DISPCNT = MODE_0_2D
				 | DISPLAY_BG0_ACTIVE
				 | DISPLAY_BG1_ACTIVE
				 | DISPLAY_BG2_ACTIVE
				 | DISPLAY_SPR_ACTIVE
				 | DISPLAY_WIN0_ON
				 | DISPLAY_WIN1_ON
				 ;
	videoSetMode(GFX_DISPCNT);
	GFX_BG0CNT = BG_32x64 | BG_MAP_BASE(0) | BG_COLOR_16 | BG_TILE_BASE(2) | BG_PRIORITY(0);
	GFX_BG1CNT = BG_32x64 | BG_MAP_BASE(0) | BG_COLOR_16 | BG_TILE_BASE(2) | BG_PRIORITY(1);
	REG_BG0CNT = GFX_BG0CNT;
	REG_BG1CNT = GFX_BG1CNT;
	// Background 2 for border
	REG_BG2CNT = BG_32x32 | BG_MAP_BASE(2) | BG_COLOR_256 | BG_TILE_BASE(1) | BG_PRIORITY(2);

	// The menu canvas fits entirely in the original VRAM H allocation.
	map0sub = BG_MAP_RAM_SUB(12);
	map1sub = BG_MAP_RAM_SUB(13);
	uiCanvasInitNds();
}

void setupMenuPalette() {
	uiCanvasPalette();
}

//---------------------------------------------------------------------------------
static void setupStream(void) {
//---------------------------------------------------------------------------------

	//----------------------------------------------------------------
	// initialize maxmod without any soundbank (unusual setup)
	//----------------------------------------------------------------
	sys.mod_count 			= 0;
	sys.samp_count			= 0;
	sys.mem_bank			= 0;
	sys.fifo_channel		= FIFO_MAXMOD;
	mmInit(&sys);

	//----------------------------------------------------------------
	// open stream
	//----------------------------------------------------------------
	myStream.sampling_rate	= sample_rate;				// sampling rate =
	myStream.buffer_length	= buffer_size;				// buffer length =
	myStream.callback		= soundRenderStartup;				// set callback function
	myStream.format			= MM_STREAM_16BIT_STEREO;	// format = stereo 16-bit
	myStream.timer			= MM_TIMER0;				// use hardware timer 0
	myStream.manual			= false;					// use manual filling
	mmStreamOpen(&myStream);

	//----------------------------------------------------------------
	// when using 'automatic' filling, your callback will be triggered
	// every time half of the wave buffer is processed.
	//
	// so: 
	// 25000 (rate)
	// ----- = ~21 Hz for a full pass, and ~42hz for half pass
	// 1200  (length)
	//----------------------------------------------------------------
	// with 'manual' filling, you must call mmStreamUpdate
	// periodically (and often enough to avoid buffer underruns)
	//----------------------------------------------------------------
}
