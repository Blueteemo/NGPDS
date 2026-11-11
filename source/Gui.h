#ifndef GUI_HEADER
#define GUI_HEADER

#include "Shared/EmuMenu.h"

#ifdef __cplusplus
extern "C" {
#endif

void setupGUI(void);
void enterGUI(void);
void exitGUI(void);
void quickSelectGame(void);
void nullUINormal(int key);
void nullUIDebug(int key);
bool hasConsoleProgram(void);
const Menu *getMenu(int menu);
void gameInputInMenuSet(void);
const char *getGameInputInMenuText(void);
void changeMenuValue(int menu, int row, int direction);
void adjustMenuGamma(int direction);
void adjustMenuAutoA(int direction);
void adjustMenuAutoB(int direction);
void adjustMenuSpeed(int direction);
void adjustMenuSleep(int direction);

static inline int cycleSetting(int value, int count, int direction) {
	if ((unsigned)value >= (unsigned)count) {
		return direction < 0 ? count - 1 : 0;
	}
	return (value + count + direction) % count;
}

void uiNullNormal(void);
void uiAbout(void);

void debugIOUnimplR(u16 port);
void debugIOUnimplW(u8 val, u16 port);
void debugDivideError(void);
void debugUndefinedInstruction(void);
void debugCrashInstruction(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !GUI_HEADER
