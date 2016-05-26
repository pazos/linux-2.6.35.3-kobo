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
	{ 0x5ee1321b, "platform_driver_register" },
	{ 0x60f71cfa, "complete" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x5f3b4552, "misc_register" },
	{ 0x825b7b9a, "dev_set_drvdata" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x2dfa9905, "__mxc_ioremap" },
	{ 0xadf42bd5, "__request_region" },
	{ 0x405ba67e, "platform_get_irq" },
	{ 0xfbc1bda7, "clk_get" },
	{ 0x6dc4bd73, "platform_get_resource" },
	{ 0xc27487dd, "__bug" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0xea147363, "printk" },
	{ 0xbc10dd97, "__put_user_4" },
	{ 0x586818d2, "wait_for_completion_interruptible_timeout" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0xfbc74f64, "__copy_from_user" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xff178f6, "__aeabi_idivmod" },
	{ 0xfa2a45e, "__memzero" },
	{ 0xc2b1d97, "clk_enable" },
	{ 0x1099eee8, "clk_get_rate" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0xf9a482f9, "msleep" },
	{ 0x54a2f065, "kill_fasync" },
	{ 0x4dba43c3, "clk_disable" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xdffb8c47, "fasync_helper" },
	{ 0x788fe103, "iomem_resource" },
	{ 0x909ae033, "misc_deregister" },
	{ 0x9bce482f, "__release_region" },
	{ 0x37a0cba, "kfree" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x2e1ca751, "clk_put" },
	{ 0x2fc251f5, "dev_get_drvdata" },
	{ 0xe0068a7b, "platform_driver_unregister" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

