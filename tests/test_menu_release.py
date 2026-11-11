#!/usr/bin/env python3
"""Compile the actual menu-exit input barrier and release filter with key stubs."""
from pathlib import Path
import re,subprocess,tempfile
root=Path(__file__).resolve().parents[1];source=(root/'source/Menu.c').read_text()
fn=re.search(r'static u32 filterMenuRelease\(u32 held\) \{.*?\n\}',source,re.S);assert fn
exit_body=source[source.index('void exitUI() {'):]
barrier=re.search(r'u32 held = keysHeld\(\);.*?EMUinput = convertInput\(filterMenuRelease\(held\) & ~KEY_TOUCH\);',exit_body,re.S);assert barrier
program='''#include <stdint.h>
#include <assert.h>
#include <stdio.h>
typedef uint32_t u32;
#define KEY_A (1u<<0)
#define KEY_B (1u<<1)
#define KEY_SELECT (1u<<2)
#define KEY_START (1u<<3)
#define KEY_RIGHT (1u<<4)
#define KEY_R (1u<<8)
#define KEY_L (1u<<9)
#define KEY_X (1u<<10)
#define KEY_Y (1u<<11)
#define KEY_TOUCH (1u<<12)
static u32 menuReleaseMask, rawHeld, EMUinput;
static int physicalMenu = 1;
static int menuUsesPhysicalKeys(void) {return physicalMenu;}
static u32 keysHeld(void) {return rawHeld;}
static u32 convertInput(u32 keys) {return keys;}
'''+fn.group()+'\nstatic void consume(void) {\n'+barrier.group()+'''\n}
int main(void) {
 const u32 actions[]={KEY_A,KEY_B,KEY_X,KEY_Y,KEY_START,KEY_SELECT,KEY_L,KEY_R};
 for(unsigned i=0;i<8;i++) {
  menuReleaseMask=0;rawHeld=actions[i]|KEY_RIGHT;EMUinput=rawHeld;consume();
  assert(EMUinput==KEY_RIGHT); /* Same-frame consumption, direction retained. */
  for(unsigned heldFrames=0;heldFrames<60;heldFrames++) assert(filterMenuRelease(rawHeld)==KEY_RIGHT);
  assert(filterMenuRelease(KEY_RIGHT)==KEY_RIGHT);
  assert(filterMenuRelease(rawHeld)==rawHeld); /* Released and pressed anew. */
 }
 for(unsigned i=0;i<8;i++)for(unsigned j=0;j<8;j++)if(i!=j) {
  menuReleaseMask=0;rawHeld=actions[i]|actions[j];consume();assert(!EMUinput);
  assert(!filterMenuRelease(actions[j]));
  assert(filterMenuRelease(actions[i]|actions[j])==actions[i]);
 }
 menuReleaseMask=0;rawHeld=0;consume();assert(!EMUinput && !menuReleaseMask);
 assert(filterMenuRelease(KEY_B)==KEY_B); /* Touch-only dismissal swallows no fresh key. */
 physicalMenu=0;rawHeld=KEY_TOUCH|KEY_B|KEY_RIGHT;consume();
 assert(EMUinput==(KEY_B|KEY_RIGHT) && !menuReleaseMask); /* Live menu dismisses by touch. */
 puts("PASS actual menu exit/release filter: same-frame suppression, held/repressed actions, independent releases, directions and touch");return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='ngpds-menu-release-') as d:
 p=Path(d);(p/'test.c').write_text(program)
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-g',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
