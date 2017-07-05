# -*- coding: utf8 -*-
# Created on 2017.3.30
# @author: jia
# @Desc:

import os
import shutil

def list_files(dir_path, skip_list):
    from fnmatch import fnmatch
    files = []
    dirs = [dir_path]
    while dirs:
        dirname = dirs.pop()
        for name in os.listdir(dirname):
            should_skip = False
            for skip in skip_list:
                if fnmatch(name, skip):
                    should_skip = True
                    break
            if should_skip:
                continue
            path = os.path.join(dirname, name)
            path = os.path.normpath(path)
            if os.path.isfile(path):
                files.append(path)
                # print('file:', name)
            if os.path.isdir(path):
                dirs.append(path)
                # print('dir:', name)
    return files

def get_bin_files():
    currpath = os.path.abspath(os.path.curdir)
    bindir = os.path.join(currpath, 'build/bin/debug')
    bindir = os.path.abspath(bindir)
    files = list_files(bindir, [])

    basenames_with_ext = [os.path.basename(x) for x in files]
    basenames = list(set([x.partition('.')[0] for x in basenames_with_ext]))
    skips = set([x + '.exe' for x in basenames]).intersection(set(basenames_with_ext))
    skips = [x.partition('.')[0] for x in skips]
    bin_files = [x for x in files if os.path.basename(x).partition('.')[0] not in skips]
    return bin_files

def update_bins():
    currpath = os.path.abspath(os.path.curdir)
    src_dir = os.path.join(currpath, 'build/bin/debug')
    src_dir = os.path.abspath(src_dir)
    des_dir = os.path.join(currpath, '../Assets/Plugins/x86_64')
    des_dir = os.path.normpath(des_dir)
    des_dir = os.path.abspath(des_dir)
    if os.path.exists(des_dir):
        shutil.rmtree(des_dir)
    os.makedirs(des_dir)
    print(des_dir)
    bin_files = get_bin_files()
    for src_path in bin_files:
        src_path = os.path.normpath(src_path)
        target_path = os.path.join(des_dir, src_path.replace(src_dir, '')[1:])
        target_path = os.path.normpath(target_path)
        shutil.copyfile(src_path, target_path)
        print('%s --> %s' % (src_path, target_path))

if __name__ == '__main__':
    update_bins()
