from os import path
import os
import shutil
import sys

def copy_headers(src: str, dest: str):
    for filename in os.listdir(src):
        p = path.join(src, filename)
        dp = path.join(dest, filename)        

        if path.isdir(p):
            copy_headers(p, dp)
        else:
            try:
                dot = filename.rindex('.')
                ext = filename[dot: len(filename)].lower()
                if ext == '.h' or ext == '.hpp':
                    if not path.exists(dest):
                        os.makedirs(dest)

                    print(f'{p} -> {dp}')
                    shutil.copy(p, dp)
            except ValueError: pass

wd: str = os.getcwd()
dirs = ['ELAudio', 'ELCore', 'ELGraphics', 'ELMaths', 'ELPhys', 'ELSys', 'ELUI']
destDirName = 'Headers'

if len(sys.argv) <= 1:
    for dirname in dirs:
        p = path.join(wd, dirname)
        if path.isdir(p):
            copy_headers(p, path.join(wd, destDirName, dirname))

    copy_headers(path.join(wd, 'include'), path.join(wd, destDirName))
else:
    arg = sys.argv[1]
    if arg[len(arg) - 1] == '/' or arg[len(arg) - 1] == '\\':
        arg = arg[:len(arg) - 1]

    if path.isabs(arg):
        srcp = arg
        destp = path.join(wd, path.dirname(arg), destDirName)
    else:
        srcp = path.join(wd, sys.argv[1])
        destp = path.join(wd, destDirName)

    if '/root' in sys.argv:
        copy_headers(srcp, destp)
    else:
        copy_headers(srcp, path.join(destp, path.split(arg)[1]))
