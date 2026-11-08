#!/usr/bin/env python3
"""Compile the actual idle-console predicate with synthetic BIOS pointers."""
from pathlib import Path
import os,re,shlex,subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source=(root/'source/Gui.c').read_text()
match=re.search(r'bool hasConsoleProgram\(void\) \{.*?\n\}',source,re.S)
assert match, 'Cannot find the actual console-program predicate'
program='''#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#define HW_NGPMONO 1
static bool gameInserted;
static unsigned gMachine;
static void *g_BIOSBASE_BNW, *g_BIOSBASE_COLOR;
'''+match.group()+'''
int main(void) {
 unsigned count=0;
 for(unsigned cart=0;cart<2;cart++) for(unsigned machine=1;machine<=2;machine++)
 for(unsigned mono=0;mono<2;mono++) for(unsigned color=0;color<2;color++) {
  gameInserted=cart;gMachine=machine;
  g_BIOSBASE_BNW=mono?&count:NULL;g_BIOSBASE_COLOR=color?&count:NULL;
  assert(hasConsoleProgram()==(cart || (machine==1?mono:color)));
  count++;
 }
 assert(count==16);puts("PASS actual hasConsoleProgram: 16 cartridge/machine/BIOS combinations");
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='ngpds-console-') as d:
 p=Path(d);(p/'test.c').write_text(program)
 subprocess.run(shlex.split(os.environ.get('CC','cc'))+['-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-g',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
