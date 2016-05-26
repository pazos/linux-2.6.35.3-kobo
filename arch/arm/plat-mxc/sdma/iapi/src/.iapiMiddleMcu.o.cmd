cmd_arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.o := arm-linux-gcc -Wp,-MD,arch/arm/plat-mxc/sdma/iapi/src/.iapiMiddleMcu.o.d  -nostdinc -isystem /home/george/Tools/CodeSourcery/Sourcery_G++_Lite/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -I/home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx5/include -Iarch/arm/plat-mxc/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Iarch/arm/plat-mxc/sdma/iapi/include -Iinclude/linux -DMCU -DOS=LINUX -DL_I_T_T_L_E_ENDIAN=0 -DB_I_G_ENDIAN=1 -DENDIANNESS=L_I_T_T_L_E_ENDIAN   -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(iapiMiddleMcu)"  -D"KBUILD_MODNAME=KBUILD_STR(iapiMiddleMcu)"  -c -o arch/arm/plat-mxc/sdma/iapi/src/.tmp_iapiMiddleMcu.o arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.c

deps_arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.o := \
  arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.c \
  arch/arm/plat-mxc/sdma/iapi/include/epm.h \
  arch/arm/plat-mxc/include/mach/hardware.h \
    $(wildcard include/config/arch/mx5.h) \
    $(wildcard include/config/arch/mx3.h) \
    $(wildcard include/config/arch/mx37.h) \
    $(wildcard include/config/arch/mx2.h) \
    $(wildcard include/config/mach/mx21.h) \
    $(wildcard include/config/mach/mx27.h) \
    $(wildcard include/config/arch/mx1.h) \
    $(wildcard include/config/arch/mx25.h) \
    $(wildcard include/config/arch/mxc91231.h) \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/sizes.h \
  arch/arm/plat-mxc/include/mach/mx5x.h \
    $(wildcard include/config/mxc/security/scc2.h) \
    $(wildcard include/config/snd/mxc/soc/iram.h) \
    $(wildcard include/config/usb/static/iram.h) \
    $(wildcard include/config/usb/static/iram/pph.h) \
    $(wildcard include/config/sdma/iram.h) \
  arch/arm/plat-mxc/include/mach/mxc.h \
    $(wildcard include/config/arch/mx31.h) \
    $(wildcard include/config/arch/mx35.h) \
    $(wildcard include/config/arch/mx51.h) \
    $(wildcard include/config/arch/mx53.h) \
    $(wildcard include/config/arch/mx50.h) \
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
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/george/Tools/CodeSourcery/Sourcery_G++_Lite/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include/stdarg.h \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/string.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiLow.h \
  arch/arm/plat-mxc/sdma/iapi/include/sdmaStruct.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiDefaults.h \
    $(wildcard include/config/override.h) \
  arch/arm/plat-mxc/sdma/iapi/include/epm.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiOS.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiLowMcu.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiMiddle.h \
  arch/arm/plat-mxc/sdma/iapi/include/iapiMiddleMcu.h \

arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.o: $(deps_arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.o)

$(deps_arch/arm/plat-mxc/sdma/iapi/src/iapiMiddleMcu.o):
