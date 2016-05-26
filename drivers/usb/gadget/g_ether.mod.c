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
	{ 0x8b82bb5d, "device_remove_file" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xff178f6, "__aeabi_idivmod" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x97255bdf, "strlen" },
	{ 0x825b7b9a, "dev_set_drvdata" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0x2e616a1e, "netif_carrier_on" },
	{ 0x9b4e5aeb, "skb_clone" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0x8949858b, "schedule_work" },
	{ 0xed172d59, "netif_carrier_off" },
	{ 0x14f53ed6, "usb_gadget_unregister_driver" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x28b5fdb6, "skb_realloc_headroom" },
	{ 0x7d11c268, "jiffies" },
	{ 0x427d5cff, "skb_trim" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xafec1f55, "netif_rx" },
	{ 0x41344088, "param_get_charp" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x29a17868, "alloc_etherdev_mq" },
	{ 0x11089ac7, "_ctype" },
	{ 0xea147363, "printk" },
	{ 0xc7eff5f9, "ethtool_op_get_link" },
	{ 0xb0edcace, "free_netdev" },
	{ 0x9f2d613e, "param_set_bool" },
	{ 0xdfe33f64, "register_netdev" },
	{ 0x73e20c1c, "strlcpy" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x548e0728, "skb_push" },
	{ 0xa34f1ef5, "crc32_le" },
	{ 0xd2267400, "init_uts_ns" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xa011a83d, "skb_pull" },
	{ 0x37fb7b1b, "dev_kfree_skb_any" },
	{ 0x8f292be0, "device_create_file" },
	{ 0xe11cb6f0, "skb_queue_tail" },
	{ 0x16e9016e, "skb_copy_expand" },
	{ 0x9f984513, "strrchr" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0x20130621, "usb_gadget_register_driver" },
	{ 0xe1025242, "__alloc_skb" },
	{ 0x52ebb126, "param_get_ushort" },
	{ 0x108e8985, "param_get_uint" },
	{ 0x6ca05c22, "eth_type_trans" },
	{ 0x4336eda0, "dev_driver_string" },
	{ 0x6ad065f4, "param_set_charp" },
	{ 0x29dfcc70, "eth_validate_addr" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x801678, "flush_scheduled_work" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0x95d3d46f, "skb_dequeue" },
	{ 0x9214ed8a, "param_get_bool" },
	{ 0x5e1ab22c, "unregister_netdev" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x6317fd30, "__netif_schedule" },
	{ 0x58a51b6, "skb_put" },
	{ 0x831c8fe2, "eth_mac_addr" },
	{ 0xb019ebda, "skb_copy_bits" },
	{ 0x2fc251f5, "dev_get_drvdata" },
	{ 0x7c5dadd8, "dev_get_stats" },
	{ 0xe914e41e, "strcpy" },
	{ 0x313341a3, "_set_bit_le" },
	{ 0xe0bc24a1, "param_set_ushort" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=arcotg_udc";

