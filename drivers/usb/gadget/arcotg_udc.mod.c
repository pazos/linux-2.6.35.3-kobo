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
	{ 0x7aa38994, "usb_debounce_id_vbus" },
	{ 0xadf42bd5, "__request_region" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x91921dc4, "fsl_platform_set_test_mode" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0xd8f795ca, "del_timer" },
	{ 0x788fe103, "iomem_resource" },
	{ 0x27bbf221, "disable_irq_nosync" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0x8949858b, "schedule_work" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x74c86cc0, "init_timer_key" },
	{ 0x62b72b0d, "mutex_unlock" },
	{ 0x4a37af33, "___dma_single_cpu_to_dev" },
	{ 0xe83bdb7, "dma_pool_destroy" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x7d11c268, "jiffies" },
	{ 0x5baaba0, "wait_for_completion" },
	{ 0x5f754e5a, "memset" },
	{ 0x335afd4c, "device_register" },
	{ 0x3765caf8, "phys_offset" },
	{ 0xea147363, "printk" },
	{ 0xdb3877d, "___dma_single_dev_to_cpu" },
	{ 0x58b48efb, "dma_free_coherent" },
	{ 0xe16b893b, "mutex_lock" },
	{ 0x6dc4bd73, "platform_get_resource" },
	{ 0x465f693b, "dma_pool_free" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x5ee1321b, "platform_driver_register" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xdfde7ac0, "dma_alloc_coherent" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0x31bd442e, "schedule_delayed_work" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x1000e51, "schedule" },
	{ 0x2dfa9905, "__mxc_ioremap" },
	{ 0x264582, "dma_pool_alloc" },
	{ 0x9bce482f, "__release_region" },
	{ 0xfcec0987, "enable_irq" },
	{ 0x37a0cba, "kfree" },
	{ 0x9d669763, "memcpy" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0xe189afda, "device_unregister" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x60f71cfa, "complete" },
	{ 0x405ba67e, "platform_get_irq" },
	{ 0xe0068a7b, "platform_driver_unregister" },
	{ 0x9f8a9667, "dev_set_name" },
	{ 0x1e6af3d1, "dma_pool_create" },
	{ 0xf20dabd8, "free_irq" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

