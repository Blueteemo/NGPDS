#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../source/UiText.h"
#include "../source/UiCanvas.h"
#include "UiFont_bin.h"
#include "UiFontJa_bin.h"

static unsigned getPixel(int x, int y) {
	unsigned tile=(y/8)*32+x/8;
	return (uiCanvasPixels()[tile*8+(y&7)] >> ((x&7)*4))&15;
}
static void encode(unsigned code,char text[4]) {
	text[0]=0xE0|(code>>12);text[1]=0x80|((code>>6)&63);
	text[2]=0x80|(code&63);text[3]=0;
}
int main(void) {
	uiCanvasClear(UI_BG);
	for(unsigned i=0;i<768;i++)assert(uiCanvasDirty()[i]);
	memset(uiCanvasDirty(),0,768);
	uiFillRect(255,191,10,10,UI_VALUE);
	assert(getPixel(255,191)==UI_VALUE && uiCanvasDirty()[767]);
	for(unsigned i=0;i<767;i++)assert(!uiCanvasDirty()[i]);
	memset(uiCanvasDirty(),0,768);
	uiFillRect(255,191,10,10,UI_VALUE);
	for(unsigned i=0;i<768;i++)assert(!uiCanvasDirty()[i]);
	uiFillRect(-10,-10,20,20,UI_ACTIVE);
	assert(getPixel(0,0)==UI_ACTIVE && getPixel(9,9)==UI_ACTIVE);
	assert(getPixel(10,9)==UI_BG);
	uiFillRect(-100,200,10,10,UI_INK);
	gUiLanguage=UI_LANGUAGE_ENGLISH;
	assert(uiTextWidth("English")==42);
	assert(uiTextWidth("简体中文")==48);
	assert(uiTextWidth("日本語")==36);
	assert(uiTextWidth("\xF0\x9F\x98\x80")==uiTextWidth("?"));
	uiCanvasClear(UI_BG);
	uiText("A中",16,64,UI_INK,256);
	for(int y=0;y<192;y++)for(int x=0;x<256;x++)
		if(getPixel(x,y)!=UI_BG)assert(x>=16 && x<34 && y>=54 && y<66);
	uiCanvasClear(UI_BG);
	uiText("中文",0,16,UI_INK,0);
	for(unsigned i=0;i<768*8;i++)assert(uiCanvasPixels()[i]==0);
	/* A byte sequence cannot be clipped into a partial CJK glyph. */
	uiText("中",0,16,UI_INK,11);
	for(unsigned i=0;i<768*8;i++)assert(uiCanvasPixels()[i]==0);
	uint32_t chinese[768*8];
	unsigned count=UiFontJa_bin[0]|(UiFontJa_bin[1]<<8);
	bool changed=false;
	for(unsigned i=0;i<count && !changed;i++) {
		const uint8_t *record=UiFontJa_bin+2+i*21;
		unsigned code=record[0]|(record[1]<<8);
		if(code<0x4E00 || code>0x9FFF)continue;
		char text[4];encode(code,text);
		gUiLanguage=UI_LANGUAGE_ZH_CN;uiCanvasClear(0);uiText(text,0,16,UI_INK,256);
		memcpy(chinese,uiCanvasPixels(),sizeof(chinese));
		gUiLanguage=UI_LANGUAGE_JAPANESE;uiCanvasClear(0);uiText(text,0,16,UI_INK,256);
		changed=memcmp(chinese,uiCanvasPixels(),sizeof(chinese))!=0;
	}
	assert(changed);
	for(unsigned language=0;language<UI_LANGUAGE_COUNT;language++) {
		gUiLanguage=language;
		uiHeader("Settings",2,1,2);
		uiDrawEntry("Interface Language:",language==2?"日本語":language==1?"简体中文":"English",0,true);
		uiDrawEntry("Autosave Flash:","On",1,false);
		uiFooter(true);
		for(int y=186;y<192;y++)for(int x=0;x<256;x++)assert(getPixel(x,y)==UI_BG);
	}
	/* Raw names must not accidentally become translation keys. */
	gUiLanguage=UI_LANGUAGE_ZH_CN;
	uiCanvasClear(0);uiDrawFileEntry("Settings",0,false);
	memcpy(chinese,uiCanvasPixels(),sizeof(chinese));
	gUiLanguage=UI_LANGUAGE_ENGLISH;
	uiCanvasClear(0);uiDrawFileEntry("Settings",0,false);
	assert(!memcmp(chinese,uiCanvasPixels(),sizeof(chinese)));
	char longText[300];memset(longText,'W',299);longText[299]=0;
	uiTextEllipsis(longText,16,64,UI_INK,240);
	puts("PASS 12px canvas: clipping, dirty tiles, native advances, Japanese variants, raw filenames, three-locale layout and footer margin");
	return 0;
}
