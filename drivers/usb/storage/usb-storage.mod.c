#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x3972220f, "module_layout" },
	{ 0xca8ff4b4, "scsi_get_host_dev" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x2b688622, "complete_and_exit" },
	{ 0x2f2e8539, "scsi_host_alloc" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x5604f657, "usb_sg_wait" },
	{ 0x97255bdf, "strlen" },
	{ 0xd0ee38b8, "schedule_timeout_uninterruptible" },
	{ 0x6a52cb60, "scsi_add_host_with_dma" },
	{ 0x38bd704c, "blk_queue_max_hw_sectors" },
	{ 0x825b7b9a, "dev_set_drvdata" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xeb91131f, "usb_reset_endpoint" },
	{ 0x20000329, "simple_strtoul" },
	{ 0x7ce6198a, "usb_kill_urb" },
	{ 0xd5152710, "sg_next" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x62b72b0d, "mutex_unlock" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x3e840148, "usb_unlink_urb" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0x5baaba0, "wait_for_completion" },
	{ 0x51f2f53, "scsi_eh_restore_cmnd" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0xa047006d, "usb_lock_device_for_reset" },
	{ 0x10dab524, "usb_deregister" },
	{ 0xdc798d37, "__mutex_init" },
	{ 0xea147363, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0x836608e8, "kthread_stop" },
	{ 0x71c90087, "memcmp" },
	{ 0xd1adb1c8, "kunmap" },
	{ 0xec6a4d04, "wait_for_completion_interruptible" },
	{ 0x778cd5ca, "usb_control_msg" },
	{ 0x1c22a61e, "scsi_scan_host" },
	{ 0xe16b893b, "mutex_lock" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x10a1c32b, "usb_free_coherent" },
	{ 0x5e98cfde, "scsi_host_put" },
	{ 0xe642241d, "blk_queue_update_dma_alignment" },
	{ 0x8f292be0, "device_create_file" },
	{ 0x70325a40, "blk_queue_bounce_limit" },
	{ 0x11a0f943, "usb_submit_urb" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0x2e243eec, "kmap" },
	{ 0x36b9f70e, "usb_reset_device" },
	{ 0x108e8985, "param_get_uint" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x35c2ba9e, "refrigerator" },
	{ 0x97435cd7, "usb_sg_cancel" },
	{ 0x586818d2, "wait_for_completion_interruptible_timeout" },
	{ 0x4336eda0, "dev_driver_string" },
	{ 0xd7ffa5d1, "wake_up_process" },
	{ 0x4101bbde, "param_set_copystring" },
	{ 0x54813886, "scsi_eh_prep_cmnd" },
	{ 0xb9e52429, "__wake_up" },
	{ 0xd2965f6f, "kthread_should_stop" },
	{ 0x37a0cba, "kfree" },
	{ 0x4cc6ee2, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0xea647464, "usb_sg_init" },
	{ 0xb409f1f8, "scsi_report_bus_reset" },
	{ 0xbd24394d, "usb_register_driver" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xaee9348c, "scsi_remove_host" },
	{ 0x60f71cfa, "complete" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x49e182c0, "param_get_string" },
	{ 0x1163f0a7, "blk_max_low_pfn" },
	{ 0xfb3fe784, "usb_alloc_coherent" },
	{ 0x2fc251f5, "dev_get_drvdata" },
	{ 0xc828ec99, "usb_free_urb" },
	{ 0x6558d3ad, "scsi_report_device_reset" },
	{ 0x975181c4, "usb_alloc_urb" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v03EBp2002d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v03EEp6906d0003dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v03F0p0107d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v03F0p070Cd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v03F0p4002d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0409p0040d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v040Dp6205d0003dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p001Cd0113dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0419p0100d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0419pAACEd0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0419pAAF5d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0419pAAF6d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0420p0001d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0019d059[2-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0019d0610dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0019d060*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p042Ed0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0433d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0434d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0444d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0446d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p044Ed0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p047Cd0610dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p047Cd03[7-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p047Cd060*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p047Cd0[4-5]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0492d045[2-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0492d04[6-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0492d0[5-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0492d[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p0495d0370dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0436p0005d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0451p5416d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0457p0150d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0457p0151d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v045EpFFFFd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v046BpFF40d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0482p0100d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0482p0101d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0482p0103d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0482p0107d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04A4p0004d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04A5p3010d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04B0p0301d0010dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04B3p4001d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04B8p0601d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04B8p0602d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04CBp0100d2210dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04CBp0100d220*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04CBp0100d2[0-1]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04CBp0100d[0-1]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04CEp0002d026Cdc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04DAp0901d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04DAp0901d01*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04DAp0D05d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04DAp2372d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04DAp2373d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0001d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0002d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0005d020[0-8]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0005d01*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0006d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0006d0205dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0007d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0007d01*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0009d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p000Ad0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p000Bd0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p000Cd0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E6p0101d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p507Cd0220dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04FCp80C2d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp0115d0133dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0525pA140d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0525pA4A5d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v052Bp1801d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v052Bp1804d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v052Bp1807d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v052Bp1905d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v052Bp1911d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d010[6-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d0450dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d01[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d04[0-4]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d0[2-3]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d0610dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d060*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0010d05*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0025d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Cd050[1-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Cd2000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Cd05[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Cd0[6-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Cd1*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Dd0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed010[6-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed0310dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed01[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed030*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed02*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp002Ed0500dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0032d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0058d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0069d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp006Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp0099d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v054Cp016Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v055Dp2020d0210dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v055Dp2020d020*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v055Dp2020d0[0-1]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v057Bp0000d0[0-2]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v057Bp0022d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v058Fp6387d0141dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0595p4343d2210dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0595p4343d220*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0595p4343d2[0-1]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0595p4343d[0-1]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v059Bp0001d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v059Fp0643d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ABp0060d110[4-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ABp0060d1110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ACp1202d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ACp1203d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ACp1204d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ACp1205d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05ACp120Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05C6p1000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05DCpB002d011[0-3]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05DCpB002d010*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05DCpB002d00*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05E3p0701d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05E3p0702d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05E3p0723d9451dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0636p0003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0644p0000d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v066Fp8000d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp1063d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp2317d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp2507d000[1-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp2507d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp2507d00[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp3507d000[1-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp3507d010[0-1]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v067Bp3507d00[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0686p4011d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0686p4017d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0693p0005d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v069Bp3004d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v071Bp3203d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v071Bp32BBd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v071Bp3203d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0727p0306d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0781p0001d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0781p0002d0009dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0781p0100d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07ABpFCCDd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AFp0004d013[0-3]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AFp0004d01[0-2]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AFp0005d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AFp0006d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07C4pA400d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07C4pA4A5d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07CFp1001d[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0839p000Ad0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0840p0082d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0840p0084d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0840p0085d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v084Dp0011d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0851p1543d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v08BDp1100d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v08CAp3103d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v090Ap1001d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v090Ap1050d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v090Ap1200d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v090Cp1132d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0A17p0004d1000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ACEp2011d0101dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ACEp20FFd0101dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p6971d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7401d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7501d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7701d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7706d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7901d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7A01d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p7A05d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p8300d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p8302d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0p8304d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pC100d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pD057d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pD058d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pD157d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pD257d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0AF0pD357d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BC2p3010d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D49p7310d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0C45p1060d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p410Ad000[1-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p410Ad00[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p410Ad0[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p410Ad[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p5200d000[1-9]dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p5200d0200dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p5200d00[1-9]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D96p5200d01*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DC4p0073d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DD8p1060d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DD8pD202d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DDAp0001d0012dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DDAp0301d0012dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E21p0520d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ED1p6660d0300dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ED1p6660d0[1-2]*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EA0p2168d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EA0p6828d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ED1p7636d0103dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0F19p0103d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0F19p0105d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCEpD008d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCEpD0E1d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCEpE030d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCEpE031d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCEpE092d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1019p0C55d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1019p0C55d010*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1019p0C55d00*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1058p0704d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10D6p2200d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1186p3E04d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1199p0FFFd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1210p0003d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1001d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1003d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1004d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1401d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1402d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1403d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1404d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1405d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1406d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1407d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1408d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1409d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Ad0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Bd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Cd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Dd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Ed0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p140Fd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1410d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1411d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1412d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1413d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1414d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1415d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1416d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1417d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1418d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1419d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Ad0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Bd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Cd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Dd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Ed0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p141Fd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1420d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1421d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1422d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1423d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1424d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1425d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1426d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1427d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1428d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1429d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Ad0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Bd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Cd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Dd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Ed0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p142Fd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1430d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1431d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1432d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1433d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1434d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1435d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1436d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1437d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1438d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p1439d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Ad0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Bd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Cd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Dd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Ed0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v12D1p143Fd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v132Bp000Bd0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1370p6828d0110dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14CDp6600d0201dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v152Dp2329d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1652p6600d0201dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1908p1315d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1908p1320d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2116p0320d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v22B8p3010d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v22B8p6426d0101dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2735p100Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v3340pFFFFd0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4102p1020d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4102p1059d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v4146pBA01d0100dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:vED06p4500d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:vED10p7636d0001dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc01ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc02ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc03ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc04ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc05ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc06ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc01ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc02ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc03ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc04ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc05ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc06ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc01ip50*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc02ip50*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc03ip50*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc04ip50*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc05ip50*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic08isc06ip50*");
