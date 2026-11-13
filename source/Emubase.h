#ifndef EMUBASE_HEADER
#define EMUBASE_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define ALLOW_SPEED_HACKS	(1<<17)
/* Preserve the former on/off color bit as the value for Mild Vivid. */
#define COLOR_MODE_SHIFT	18
#define COLOR_MODE_MASK		(3 << COLOR_MODE_SHIFT)
#define BORDER_MODE_SHIFT	20
#define BORDER_MODE_MASK	(3 << BORDER_MODE_SHIFT)
#define GAME_INPUT_IN_MENU	(1 << 22)
#define LEGACY_AUDIO		(1 << 23)
#define COMPATIBLE_AUDIO	(1 << 24)	/* Kept for v0.2.1/v0.2.2 settings. */
#define AUDIO_MODE_MASK		(LEGACY_AUDIO | COMPATIBLE_AUDIO)

enum { COLOR_ORIGINAL, COLOR_MILD_VIVID, COLOR_VIVID, COLOR_MODE_COUNT };
enum { BORDER_GRAPHITE, BORDER_ORIGINAL, BORDER_OFF, BORDER_MODE_COUNT };

typedef struct {				//(config struct)
	char magic[4];				//="CFG",0
	int emuSettings;
	int unused;					// unused
	u8 gammaValue;				// from gfx.s
	u8 sprites;					// from gfx.s
	u8 config;					// from cart.s
	u8 controller;				// from io.s
	u8 alarmHour;
	u8 alarmMinute;
	u8 birthDay;
	u8 birthMonth;
	u8 birthYear;
	u8 language;
	u8 palette;					// bit 0-2 NGPC B&W palette.
	char currentPath[256];
	char biosPath[256];
	char biosPathColor[256];
} ConfigData;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !EMUBASE_HEADER
