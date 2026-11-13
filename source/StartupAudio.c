#include <nds.h>
#include "Sound.h"

/* A launch-only ramp prevents the initial full-scale DC step in the legacy
 * stream. The short crossfade below only runs when changing audio modes. */
#define STARTUP_FADE_FRAMES 32768u
static unsigned startupFramesLeft = STARTUP_FADE_FRAMES;
enum { MODE_FADE_FRAMES = 256 };
static unsigned modeFadeLeft;
static s16 lastLeft, lastRight, fadeLeft, fadeRight;

void soundBeginModeFade(void) {
	if (startupFramesLeft) return;
	fadeLeft = lastLeft;
	fadeRight = lastRight;
	modeFadeLeft = MODE_FADE_FRAMES;
}

/* Sound.s exports ARM entry points without interworking metadata. */
#ifdef __arm__
__attribute__((target("arm")))
#endif
mm_word soundRenderStartup(mm_word length, mm_addr dest, mm_stream_formats format) {
	mm_word filled = soundRender(length, dest, format);
	s16 *samples = dest;
	unsigned frames = filled < startupFramesLeft ? filled : startupFramesLeft;
	if (frames) {
		for (unsigned i = 0; i < frames; i++) {
			int gain = (int)(STARTUP_FADE_FRAMES - startupFramesLeft);
			samples[2 * i] = (s32)samples[2 * i] * gain >> 15;
			samples[2 * i + 1] = (s32)samples[2 * i + 1] * gain >> 15;
			startupFramesLeft--;
		}
	}
	frames = filled < modeFadeLeft ? filled : modeFadeLeft;
	for (unsigned i = 0; i < frames; i++) {
		int oldWeight = (int)modeFadeLeft;
		int newWeight = MODE_FADE_FRAMES - oldWeight;
		samples[2*i] = (fadeLeft*oldWeight + samples[2*i]*newWeight) >> 8;
		samples[2*i+1] = (fadeRight*oldWeight + samples[2*i+1]*newWeight) >> 8;
		modeFadeLeft--;
	}
	if (filled) {
		lastLeft = samples[2*(filled-1)];
		lastRight = samples[2*(filled-1)+1];
	}
	return filled;
}
