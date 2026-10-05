#!/bin/sh
# What DOOM on TINY-32 needs that isn't ours: doomgeneric (GPL-2, the portable DOOM source), seven soft-float helpers
# from LLVM's compiler-rt (Apache-2.0 with LLVM exception) and the shareware DOOM1.WAD (id Software's shareware
# licence: free to copy; from Debian's doom-wad-shareware package, md5 f0cefca49926d00903cf57551d901abe).
set -e
cd "$(dirname "$0")"
[ -d doomgeneric ] || git clone -q --depth 1 https://github.com/ozkl/doomgeneric.git
mkdir -p compiler-rt
B=https://raw.githubusercontent.com/llvm/llvm-project/llvmorg-19.1.0/compiler-rt/lib/builtins
for f in addsf3.c fp_add_impl.inc fp_lib.h int_lib.h int_types.h int_util.h int_endianness.h divsf3.c fp_div_impl.inc \
         mulsf3.c fp_mul_impl.inc extendsfdf2.c fp_extend.h fp_extend_impl.inc fixsfsi.c fp_fixint_impl.inc floatsisf.c \
         comparedf2.c fp_compare_impl.inc fp_mode.h fp_mode.c int_math.h; do
  [ -f compiler-rt/$f ] || curl -sfL -o compiler-rt/$f $B/$f
done
if [ ! -f doom1.wad ]; then
  T=$(mktemp -d)
  curl -sfL -o $T/w.deb http://deb.debian.org/debian/pool/non-free/d/doom-wad-shareware/doom-wad-shareware_1.9.fixed-5_all.deb
  (cd $T && ar x w.deb && tar xf data.tar.*)
  cp $T/usr/share/games/doom/doom1.wad .
  rm -rf $T
fi
[ "$(md5 -q doom1.wad 2>/dev/null || md5sum doom1.wad | cut -d' ' -f1)" = f0cefca49926d00903cf57551d901abe ] || { echo "doom1.wad: wrong file"; exit 1; }
echo ready
