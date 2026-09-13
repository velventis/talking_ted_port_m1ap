#!/bin/bash
set -euo pipefail

SRC="$(pwd)"
BUILD="/tmp/m1ap_fb"
CC="$HOME/m1ap_toolchain/gp2-s7-mipsel-lg-gcc-4.3.2-glibc-2.9-nptl/bin/mipsel-lg-linux-gnu-gcc"

rm -rf "$BUILD"
mkdir -p "$BUILD"
cp -a "$SRC/"* "$BUILD/"
cd "$BUILD"

"$CC" -std=gnu99 -I"$BUILD/include" -I"$BUILD/include/directfb" \
  -L"$HOME/m1ap_toolchain/_libs" \
  -Wl,-rpath-link,"$HOME/m1ap_toolchain/_libs" \
  "$BUILD/main.c" -o "$BUILD/build" \
  -ldirectfb -ldirect -llgncopenapi -lrt

cp "$BUILD/build" "$SRC/"
echo "Build succeeded: $SRC/build"