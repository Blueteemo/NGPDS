#!/usr/bin/env python3
"""Test the actual launch-only wrapper without ROMs, BIOS or an NDS SDK."""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[1]
source='\n'.join(l for l in (root/'source/StartupAudio.c').read_text().splitlines() if not l.startswith('#include'))
program=r'''
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
typedef unsigned mm_word;
typedef void *mm_addr;
typedef int mm_stream_formats;
typedef int16_t s16;
typedef int32_t s32;
static unsigned calls, shortBy;
static s16 original(unsigned i) {
 static const s16 pattern[]={-32768,32767,-1,1,0,16384,-16384,257,-513};
 return pattern[i%9];
}
static mm_word soundRender(mm_word n,mm_addr output,mm_stream_formats format) {
 assert(format==3);calls++;s16 *p=output;
 for(unsigned i=0;i<n*2;i++)p[i]=original(i);
 return n>shortBy?n-shortBy:0;
}
'''+source+r'''
static int scaled(int value,unsigned gain) {
 int64_t p=(int64_t)value*gain;
 return p>=0?p/32768:-((-p+32767)/32768);
}
int main(void) {
 s16 buffer[3202];unsigned emitted=0;
 while(startupFramesLeft) {
  unsigned before=startupFramesLeft;
  for(unsigned i=0;i<3202;i++)buffer[i]=12345;
  unsigned n=soundRenderStartup(1600,buffer+1,3);assert(n==1600);
  assert(buffer[0]==12345&&buffer[3201]==12345);
  for(unsigned i=0;i<n;i++)for(unsigned c=0;c<2;c++) {
   unsigned gain=emitted+i;int expected=gain<32768?scaled(original(i*2+c),gain):original(i*2+c);
   assert(buffer[1+i*2+c]==expected);
  }
  emitted+=n;assert(startupFramesLeft==(before>n?before-n:0));
 }
 unsigned left=startupFramesLeft;unsigned oldcalls=calls;
 assert(soundRenderStartup(1600,buffer+1,3)==1600);assert(calls==oldcalls+1);
 for(unsigned i=0;i<3200;i++)assert(buffer[1+i]==original(i));
 assert(startupFramesLeft==left);
 startupFramesLeft=32768;shortBy=1596;assert(soundRenderStartup(1600,buffer+1,3)==4);assert(startupFramesLeft==32764);
 for(unsigned i=8;i<3200;i++)assert(buffer[1+i]==original(i));
 shortBy=1600;left=startupFramesLeft;assert(soundRenderStartup(1600,buffer+1,3)==0&&startupFramesLeft==left);
 puts("PASS actual startup audio: exact ramp, guard bytes, gain limit, short/zero fills and bit-identical post-ramp output");return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='ngpds-startup-audio-') as d:
 p=Path(d);(p/'test.c').write_text(program)
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-g',str(p/'test.c'),'-o',str(p/'test')],check=True)
 subprocess.run([str(p/'test')],check=True)
