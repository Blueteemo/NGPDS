#include "UiText.h"
#include <stddef.h>
#include <string.h>

unsigned char gUiLanguage = UI_LANGUAGE_ENGLISH;

static const struct {
	const char *english;
	const char *chinese;
	const char *japanese;
} translations[] = {
#define UI_STRING(en, zh, ja) {en, zh, ja},
#include "UiStrings.inc"
#undef UI_STRING
};

unsigned uiLanguageFromConfig(uint32_t stored) {
	if ((stored & UINT32_C(0xFFFFFF00)) == UI_LANGUAGE_TAG &&
		(stored & 255) < UI_LANGUAGE_COUNT) {
		return stored & 255;
	}
	return UI_LANGUAGE_ENGLISH;
}

uint32_t uiLanguageToConfig(uint32_t stored, unsigned language) {
	/* Preserve unknown legacy contents until the user chooses a UI translation. */
	bool knownPreference = (stored & UINT32_C(0xFFFFFF00)) == UI_LANGUAGE_TAG &&
		(stored & 255) < UI_LANGUAGE_COUNT;
	if ((language > UI_LANGUAGE_ENGLISH && language < UI_LANGUAGE_COUNT) || knownPreference) {
		return UI_LANGUAGE_TAG | (language < UI_LANGUAGE_COUNT ? language : 0);
	}
	return stored;
}

const char *uiTranslate(const char *text) {
	if (!text || gUiLanguage == UI_LANGUAGE_ENGLISH || gUiLanguage >= UI_LANGUAGE_COUNT) {
		return text;
	}
	for (unsigned i = 0; i < sizeof(translations) / sizeof(translations[0]); i++) {
		if (!strcmp(text, translations[i].english)) {
			return gUiLanguage == UI_LANGUAGE_JAPANESE ? translations[i].japanese : translations[i].chinese;
		}
	}
	return text;
}

uint32_t uiNextCodepoint(const char **text) {
	const unsigned char *start = (const unsigned char *)*text;
	const unsigned char *next = start;
	uint32_t code = *next++;
	if (!code) {
		return 0;
	}
	if (code < 128) {
		*text = (const char *)next;
		return code;
	}
	unsigned remaining;
	uint32_t minimum;
	if (code >= 0xC2 && code <= 0xDF) {
		remaining = 1;
		minimum = 0x80;
		code &= 0x1F;
	} else if (code >= 0xE0 && code <= 0xEF) {
		remaining = 2;
		minimum = 0x800;
		code &= 0x0F;
	} else if (code >= 0xF0 && code <= 0xF4) {
		remaining = 3;
		minimum = 0x10000;
		code &= 7;
	} else {
		*text = (const char *)(start + 1);
		return 0xFFFD;
	}
	for (unsigned i = 0; i < remaining; i++) {
		unsigned byte = *next;
		if (byte < 0x80 || byte > 0xBF) {
			*text = (const char *)(start + 1);
			return 0xFFFD;
		}
		next++;
		code = (code << 6) | (byte & 0x3F);
	}
	if (code < minimum || code > 0x10FFFF ||
		(code >= 0xD800 && code <= 0xDFFF)) {
		*text = (const char *)(start + 1);
		return 0xFFFD;
	}
	*text = (const char *)next;
	return code;
}

int uiTextColumns(const char *text) {
	int columns = 0;
	while (text && *text) {
		if (uiNextCodepoint(&text) >= 32) {
			columns++;
		}
	}
	return columns;
}
