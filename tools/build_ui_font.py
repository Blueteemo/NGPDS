#!/usr/bin/env python3
"""Build native 12px BMP fonts and sparse Japanese overrides. No runtime scaling."""
from pathlib import Path
import gzip, struct, hashlib
ROOT=Path(__file__).resolve().parents[1]
FONTS=ROOT/'third_party/fusion-pixel-12px'

def load(face):
    p=FONTS/f'fusion-pixel-12px-monospaced-{face}.bdf.gz'
    lines=gzip.decompress(p.read_bytes()).decode().splitlines()
    glyphs={};i=0
    while i<len(lines):
        if not lines[i].startswith('STARTCHAR '):i+=1;continue
        code=-1;advance=12;box=(0,0,0,0);bits=[];i+=1
        while lines[i]!='ENDCHAR':
            line=lines[i]
            if line.startswith('ENCODING '):code=int(line.split()[1])
            elif line.startswith('DWIDTH '):advance=int(line.split()[1])
            elif line.startswith('BBX '):box=tuple(map(int,line.split()[1:]))
            elif line=='BITMAP':
                i+=1
                while lines[i]!='ENDCHAR':bits.append(lines[i]);i+=1
                break
            i+=1
        # The three UI languages need no Hangul syllables or Jamo.
        keep=32<=code<=65535 and not (0xAC00<=code<=0xD7FF or 0x1100<=code<=0x11FF or 0xA960<=code<=0xA97F)
        w,h,xoff,yoff=box
        # Exclude oversized presentation ligatures outside the UI cell format.
        if keep and 0<=advance<=12 and w<=12 and h<=12:
            packed=bytearray(18)
            for yy,rowhex in enumerate(bits):
                value=int(rowhex,16);rowbits=len(rowhex)*4
                for xx in range(w):
                    x=xoff+xx;y=10-yoff-h+yy
                    if 0<=x<12 and 0<=y<12 and value&(1<<(rowbits-1-xx)):
                        bit=y*12+x;packed[bit//8]|=0x80>>(bit%8)
            glyphs[code]=bytes([advance])+packed
        i+=1
    return glyphs

def write(name,glyphs):
    data=struct.pack('<H',len(glyphs))+b''.join(struct.pack('<H',c)+glyphs[c] for c in sorted(glyphs))
    (ROOT/'data'/name).write_bytes(data)
    print(name, 'glyphs',len(glyphs),'bytes',len(data),'sha256',hashlib.sha256(data).hexdigest())

base=load('zh_hans');ja=load('ja')
overrides={c:bits for c,bits in ja.items() if base.get(c)!=bits}
write('UiFont.bin',base);write('UiFontJa.bin',overrides)
