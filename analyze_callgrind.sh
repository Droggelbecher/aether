#!/usr/bin/env sh

filename=$1
filename_base=${filename%.*}
gprof2dot -sw -f callgrind $filename > ${filename_base}.dot
dot -Tpng ${filename_base}.dot -o${filename_base}.png
cp ${filename_base}.png ~/host/Desktop/tmp/
echo ${filename_base}.png

