#!/bin/sh
# Build the example against your own libraries.
#   put this folder next to your lib/ (so that ../../lib/Init.z80 exists, same layout as assets/test/<name>/)
#   ZAZY=path/to/zazy ./build.sh
set -e
ZAZY=${ZAZY:-../../zazy/zazy}          # zazy finds its lib/zazy.zh next to the executable
$ZAZY --init ../../lib/Init.z80 --math ../../lib/utils/Math.z80 \
      -o hello.z80 hello.zz
echo "hello.z80 written, assemble it with your assembler"
