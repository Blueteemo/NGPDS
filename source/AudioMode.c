#include <nds.h>
#include "Sound.h"
#include "Emubase.h"

enum { DAC_DC_INITIAL = -32768 * 128 };

volatile u8 soundMode;
static s32 dacDcLeft = DAC_DC_INITIAL, dacDcRight = DAC_DC_INITIAL;

__attribute__((target("arm")))
void soundResetAudioBuffers(void) {
	int irqState = enterCriticalSection();
	dacDcLeft = dacDcRight = DAC_DC_INITIAL;
	leaveCriticalSection(irqState);
}

int soundModeFromSettings(unsigned settings) {
	return (settings & LEGACY_AUDIO) ? AUDIO_LEGACY : AUDIO_STANDARD;
}

void soundSetMode(int mode) {
	int irqState = enterCriticalSection();
	if (soundMode != mode) {
		soundMode = mode;
		soundResetAudioBuffers();
		soundBeginModeFade();
	}
	leaveCriticalSection(irqState);
}

ITCM_CODE __attribute__((target("arm")))
mm_word soundFilterDac(mm_word length, mm_addr dest) {
	/* AC couple the unsigned DAC stream before switching it with signed PSG.
	 * 171/65536 gives a cutoff near 20 Hz, with 6 dB of transient headroom. */
	s16 *samples = dest;
	for (unsigned i = 0; i < length; i++) {
		s32 left = (s32)samples[2 * i] * 128 - dacDcLeft;
		s32 right = (s32)samples[2 * i + 1] * 128 - dacDcRight;
		dacDcLeft += (left * 171) >> 16;
		dacDcRight += (right * 171) >> 16;
		samples[2 * i] = left >> 8;
		samples[2 * i + 1] = right >> 8;
	}
	return length;
}
