#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/UiText.h"

static void decode(const char *s, uint32_t expected) {
	const char *p = s;
	assert(uiNextCodepoint(&p) == expected);
	assert(p > s);
}
int main(void) {
	assert(uiLanguageFromConfig(0) == UI_LANGUAGE_ENGLISH);
	assert(uiLanguageFromConfig(0x12345678) == UI_LANGUAGE_ENGLISH);
	assert(uiLanguageToConfig(0x12345678, UI_LANGUAGE_ENGLISH) == 0x12345678);
	assert(uiLanguageToConfig(0, UI_LANGUAGE_ZH_CN) == (UI_LANGUAGE_TAG | 1));
	assert(uiLanguageFromConfig(UI_LANGUAGE_TAG | 1) == UI_LANGUAGE_ZH_CN);
	assert(uiLanguageFromConfig(UI_LANGUAGE_TAG | 3) == UI_LANGUAGE_ENGLISH);
	assert(uiLanguageToConfig(UI_LANGUAGE_TAG | 3, UI_LANGUAGE_ENGLISH) == (UI_LANGUAGE_TAG | 3));
	assert(uiLanguageToConfig(UI_LANGUAGE_TAG | 1, UI_LANGUAGE_ENGLISH) == UI_LANGUAGE_TAG);
	assert(uiLanguageFromConfig(UI_LANGUAGE_TAG | 2) == UI_LANGUAGE_JAPANESE);
	assert(uiLanguageToConfig(0, UI_LANGUAGE_JAPANESE) == (UI_LANGUAGE_TAG | 2));
	gUiLanguage = UI_LANGUAGE_JAPANESE;
	assert(!strcmp(uiTranslate("General"), "一般"));
	assert(!strcmp(uiTranslate("Game Control in Menu:"), "メニュー中のゲーム操作："));
	assert(!strcmp(uiTranslate("Load Game"), "ゲームを開く"));
	gUiLanguage = UI_LANGUAGE_ENGLISH;
	assert(!strcmp(uiTranslate("General"), "General"));
	assert(!strcmp(uiTranslate("Game Control in Menu:"), "Game Control in Menu:"));
	assert(!strcmp(uiTranslate("Load Game"), "Load Game"));
	gUiLanguage = UI_LANGUAGE_ZH_CN;
	assert(!strcmp(uiTranslate("General"), "通用"));
	assert(!strcmp(uiTranslate("Game Control in Menu:"), "菜单中控制游戏："));
	assert(!strcmp(uiTranslate("Load Game"), "载入游戏"));
	assert(!strcmp(uiTranslate("Off"), "关"));
	assert(!strcmp(uiTranslate("UNKNOWN"), "UNKNOWN"));
	assert(uiTranslate(NULL) == NULL);
	decode("A", 'A'); decode("中", 0x4E2D); decode("é", 0xE9);
	decode("\xF0\x9F\x98\x80", 0x1F600);
	const char *invalid[] = {"\x80", "\xC0\xAF", "\xED\xA0\x80", "\xF4\x90\x80\x80", "\xF5\x80\x80\x80", "\xE4", "\xE4\xB8", "\xF0\x90\x80"};
	for (unsigned i = 0; i < sizeof(invalid)/sizeof(invalid[0]); i++) {
		/* Allocate only the exact string: ASan catches reads past the NUL. */
		char *copy = strdup(invalid[i]);
		decode(copy, 0xFFFD);
		free(copy);
	}
	const char *end = "";
	const char *p = end;
	assert(uiNextCodepoint(&p) == 0 && p == end);
	assert(uiTextColumns("界面语言： English") == 13);
	assert(uiTextColumns("自动保存游戏存档： 开") == 11);
	assert(uiTextColumns(NULL) == 0);
	puts("PASS UI language tagging, legacy preservation, translations and strict UTF-8 decoding");
	return 0;
}
