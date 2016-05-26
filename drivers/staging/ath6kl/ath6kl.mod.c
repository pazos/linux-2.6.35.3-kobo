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

MODULE_INFO(staging, "Y");

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x3972220f, "module_layout" },
	{ 0xc03bac33, "skb_queue_head" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0xf88c3301, "sg_init_table" },
	{ 0xff178f6, "__aeabi_idivmod" },
	{ 0x2b688622, "complete_and_exit" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x50560c2, "sysfs_remove_bin_file" },
	{ 0x6ec39390, "mem_map" },
	{ 0x6980fe91, "param_get_int" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xd8f795ca, "del_timer" },
	{ 0x97255bdf, "strlen" },
	{ 0x825b7b9a, "dev_set_drvdata" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x48dccf64, "sdio_writesb" },
	{ 0x4078ae2f, "sdio_enable_func" },
	{ 0xc7a4fbed, "rtnl_lock" },
	{ 0xc61c5317, "sdio_claim_irq" },
	{ 0xd6659eb8, "mmc_wait_for_cmd" },
	{ 0x2e616a1e, "netif_carrier_on" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0x7f6b296c, "skb_copy" },
	{ 0x43ab66c3, "param_array_get" },
	{ 0xb0bb9c02, "down_interruptible" },
	{ 0xed172d59, "netif_carrier_off" },
	{ 0x353e3fa5, "__get_user_4" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x74c86cc0, "init_timer_key" },
	{ 0x28118cb6, "__get_user_1" },
	{ 0xff964b25, "param_set_int" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x45947727, "param_array_set" },
	{ 0x7d11c268, "jiffies" },
	{ 0x427d5cff, "skb_trim" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xafec1f55, "netif_rx" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0x78052b64, "mmc_wait_for_req" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x5baaba0, "wait_for_completion" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0x29a17868, "alloc_etherdev_mq" },
	{ 0x2f5c5204, "netif_rx_ni" },
	{ 0x13622f3a, "dev_alloc_skb" },
	{ 0x3765caf8, "phys_offset" },
	{ 0xea147363, "printk" },
	{ 0x71c90087, "memcmp" },
	{ 0x328a05f1, "strncpy" },
	{ 0xdfe33f64, "register_netdev" },
	{ 0x51265e4c, "mmc_set_data_timeout" },
	{ 0x53207cf5, "wireless_send_event" },
	{ 0x548e0728, "skb_push" },
	{ 0x9a6221c5, "mod_timer" },
	{ 0x5ee1321b, "platform_driver_register" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xa011a83d, "skb_pull" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0x4b93c3cf, "sdio_readsb" },
	{ 0x1b1c7d20, "sdio_unregister_driver" },
	{ 0xe11cb6f0, "skb_queue_tail" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0xbe405dec, "sdio_release_irq" },
	{ 0x108e8985, "param_get_uint" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x8a7d1c31, "high_memory" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x6ca05c22, "eth_type_trans" },
	{ 0xc27487dd, "__bug" },
	{ 0xd7ffa5d1, "wake_up_process" },
	{ 0x4101bbde, "param_set_copystring" },
	{ 0xc3bc7a5e, "ether_setup" },
	{ 0xb9e52429, "__wake_up" },
	{ 0xd33cfdd9, "sdio_memcpy_toio" },
	{ 0x37a0cba, "kfree" },
	{ 0x4cc6ee2, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0x83c0c5ed, "sysfs_create_bin_file" },
	{ 0x8cf51d15, "up" },
	{ 0xa01cac38, "request_firmware" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0x95d3d46f, "skb_dequeue" },
	{ 0x5e1ab22c, "unregister_netdev" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x6317fd30, "__netif_schedule" },
	{ 0x49e182c0, "param_get_string" },
	{ 0xddd75269, "sdio_register_driver" },
	{ 0x261b697b, "consume_skb" },
	{ 0x282bb6be, "sdio_memcpy_fromio" },
	{ 0x496c08dc, "sdio_claim_host" },
	{ 0xe0068a7b, "platform_driver_unregister" },
	{ 0x85670f1d, "rtnl_is_locked" },
	{ 0x58a51b6, "skb_put" },
	{ 0x2fc251f5, "dev_get_drvdata" },
	{ 0x22ae8a2, "sdio_set_block_size" },
	{ 0x6db8d7b6, "release_firmware" },
	{ 0x6e720ff2, "rtnl_unlock" },
	{ 0xdeb64a90, "sdio_disable_func" },
	{ 0x9370db33, "sdio_release_host" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("sdio:c*v0271d0200*");
MODULE_ALIAS("sdio:c*v0271d0201*");
MODULE_ALIAS("sdio:c*v0271d0300*");
MODULE_ALIAS("sdio:c*v0271d0301*");
