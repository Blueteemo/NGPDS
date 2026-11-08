#ifndef UITEXT_H
#define UITEXT_H

#include <stdbool.h>
#include <stdint.h>

#define UI_LANGUAGE_ENGLISH 0
#define UI_LANGUAGE_ZH_CN 1
#define UI_LANGUAGE_JAPANESE 2
#define UI_LANGUAGE_COUNT 3
/* Tagged use of the existing unused word; ConfigData remains 792 bytes. */
#define UI_LANGUAGE_TAG UINT32_C(0x55494C00)

extern unsigned char gUiLanguage;
unsigned uiLanguageFromConfig(uint32_t stored);
uint32_t uiLanguageToConfig(uint32_t stored, unsigned language);
const char *uiTranslate(const char *text);
uint32_t uiNextCodepoint(const char **text);
int uiTextColumns(const char *text);
void uiLanguageChanged(void);

#endif
