cmd_arch/arm/mach-mx5/suspend.o := arm-linux-gcc -Wp,-MD,arch/arm/mach-mx5/.suspend.o.d  -nostdinc -isystem /home/george/Tools/CodeSourcery/Sourcery_G++_Lite/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -I/home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx5/include -Iarch/arm/plat-mxc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float       -c -o arch/arm/mach-mx5/suspend.o arch/arm/mach-mx5/suspend.S

deps_arch/arm/mach-mx5/suspend.o := \
  arch/arm/mach-mx5/suspend.S \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/linkage.h \
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

arch/arm/mach-mx5/suspend.o: $(deps_arch/arm/mach-mx5/suspend.o)

$(deps_arch/arm/mach-mx5/suspend.o):
