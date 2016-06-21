#!/bin/sh

for name in `ls *.c.s` ; do
  base=${name%.c.s}
  gcc $name ../mcc/lib.c -m32 --static -o bin/$base
done
