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
	/* Top-level pages have no repeated heading; submenus keep their context. */
	uiHeader("", 3, 1, 1);
	assert(uiRowsTop() == 36);
	for(int y=24;y<36;y++)for(int x=0;x<256;x++)assert(getPixel(x,y)==UI_BG);
	uint32_t withoutEdges[768*8];
	for(unsigned language=0;language<UI_LANGUAGE_COUNT;language++) {
		gUiLanguage=language;
		for(int count=1;count<=5;count++)for(int selected=0;selected<count;selected++) {
			uiHeader("", 3, 1, 1);
			for(int i=0;i<count;i++)uiDrawEntry("Settings",NULL,i,i==selected);
			memcpy(withoutEdges,uiCanvasPixels(),sizeof(withoutEdges));
			uiHeader("", 3, 1, 1);uiListEdges(count);
			for(int i=0;i<count;i++)uiDrawEntry("Settings",NULL,i,i==selected);
			if(count<5)assert(!memcmp(withoutEdges,uiCanvasPixels(),sizeof(withoutEdges)));
			else {
				for(int y=24;y<168;y++)for(int x=8;x<248;x++) {
					unsigned tile=(y/8)*32+x/8;
					unsigned before=(withoutEdges[tile*8+(y&7)]>>((x&7)*4))&15;
					if(getPixel(x,y)!=before)assert((y==36||y==155)&&x>=16&&x<240);
				}
				assert(getPixel(128,36)!=UI_BG&&getPixel(128,155)!=UI_BG);
			}
		}
		uiHeader("Settings",2,1,1);assert(uiRowsTop()==48);
		for(int y=24;y<48;y++)for(int x=200;x<240;x++)assert(getPixel(x,y)==UI_BG);
		for(int y=24;y<48;y++)for(int x=0;x<16;x++)assert(getPixel(x,y)==UI_BG);
		uiHeader("",1,2,2);assert(uiRowsTop()==36);
		uiHeader("/",1,1,1);assert(uiRowsTop()==48);
	}
	for(unsigned language=0;language<UI_LANGUAGE_COUNT;language++) {
		gUiLanguage=language;
		for(int changing=0;changing<2;changing++) {
			uiCanvasClear(UI_BG);uiFooter(changing);
			for(int group=0;group<3;group++) {
				int lo=256,hi=-1;
				for(int y=173;y<186;y++)for(int x=8+group*80;x<88+group*80;x++) {
					if(getPixel(x,y)==UI_SECONDARY){if(x<lo)lo=x;if(x>hi)hi=x;}
				}
				assert(hi>=lo);
				int delta=lo+hi+1-2*(48+group*80);
				assert(delta>=-4&&delta<=4);
			}
		}
	}
	char longText[300];memset(longText,'W',299);longText[299]=0;
	uiTextEllipsis(longText,16,64,UI_INK,240);
	puts("PASS 12px canvas: clipping, dirty tiles, native advances, Japanese variants, raw filenames, three-locale layout, hierarchy, sparse-page preservation, five-row hairlines, centered footer columns and footer margin");
	return 0;
}
