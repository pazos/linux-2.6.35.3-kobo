cmd_drivers/video/console/font_8x8.o := arm-linux-gcc -Wp,-MD,drivers/video/console/.font_8x8.o.d  -nostdinc -isystem /home/george/Tools/CodeSourcery/Sourcery_G++_Lite/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -I/home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx5/include -Iarch/arm/plat-mxc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(font_8x8)"  -D"KBUILD_MODNAME=KBUILD_STR(font)"  -c -o drivers/video/console/.tmp_font_8x8.o drivers/video/console/font_8x8.c

deps_drivers/video/console/font_8x8.o := \
  drivers/video/console/font_8x8.c \
  include/linux/font.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/posix_types.h \

drivers/video/console/font_8x8.o: $(deps_drivers/video/console/font_8x8.o)

$(deps_drivers/video/console/font_8x8.o):