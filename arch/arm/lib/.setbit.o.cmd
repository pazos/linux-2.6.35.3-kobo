cmd_arch/arm/lib/setbit.o := arm-linux-gcc -Wp,-MD,arch/arm/lib/.setbit.o.d  -nostdinc -isystem /home/george/Tools/CodeSourcery/Sourcery_G++_Lite/bin/../lib/gcc/arm-none-linux-gnueabi/4.4.1/include -I/home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-mx5/include -Iarch/arm/plat-mxc/include -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables  -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float       -c -o arch/arm/lib/setbit.o arch/arm/lib/setbit.S

deps_arch/arm/lib/setbit.o := \
  arch/arm/lib/setbit.S \
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
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/george/build/trilogy/DragonNetronix/linux-2.6.35.3/arch/arm/include/asm/hwcap.h \
  arch/arm/lib/bitops.h \
    $(wildcard include/config/cpu/32v6k.h) \

arch/arm/lib/setbit.o: $(deps_arch/arm/lib/setbit.o)

$(deps_arch/arm/lib/setbit.o):