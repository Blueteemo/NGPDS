#!/usr/bin/env python3
"""Host checks for the actual UI code; no NDS SDK, ROM or BIOS required."""
import argparse, os, shlex, subprocess, tempfile
from pathlib import Path

root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--sanitize',action='store_true')
args=parser.parse_args()
flags=['-std=c11','-D_POSIX_C_SOURCE=200809L','-Wall','-Wextra','-Werror']
if args.sanitize:flags+=['-g','-fsanitize=address,undefined']
compiler=shlex.split(os.environ.get('CC','cc'))
with tempfile.TemporaryDirectory(prefix='ngpds-ui-tests-') as directory:
    out=Path(directory)
    font_sources=[]
    for name in ['UiFont','UiFontJa']:
        font=(root/('data/'+name+'.bin')).read_bytes()
        (out/(name+'_bin.h')).write_text('#include <stdint.h>\nextern const uint8_t '+name+'_bin[];\n')
        f=out/(name+'.c')
        f.write_text('#include <stdint.h>\nconst uint8_t '+name+'_bin[]={\n'+','.join(str(v) for v in font)+'\n};\n')
        font_sources.append(f)
    tests=[('text',[root/'tests/test_ui_text.c',root/'source/UiTextCore.c']),
           ('render',[root/'tests/test_ui_canvas.c',root/'source/UiCanvas.c',root/'source/UiTextCore.c']+font_sources)]
    for name,files in tests:
        executable=out/name
        subprocess.run(compiler+flags+['-I'+str(out)]+[str(p) for p in files]+['-o',str(executable)],check=True)
        subprocess.run([str(executable)],check=True)
