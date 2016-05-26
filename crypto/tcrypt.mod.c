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
	{ 0x41344088, "param_get_charp" },
	{ 0x6ad065f4, "param_set_charp" },
	{ 0x6980fe91, "param_get_int" },
	{ 0xff964b25, "param_set_int" },
	{ 0x108e8985, "param_get_uint" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0x90a1004a, "crypto_has_alg" },
	{ 0x5086ac3a, "alg_test" },
	{ 0x7a4497db, "kzfree" },
	{ 0x29075ba5, "crypto_ahash_final" },
	{ 0xec6a4d04, "wait_for_completion_interruptible" },
	{ 0xdbc6a2a0, "crypto_ahash_digest" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x5cad3ac9, "crypto_alloc_ahash" },
	{ 0x2196324, "__aeabi_idiv" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x60f71cfa, "complete" },
	{ 0x7d11c268, "jiffies" },
	{ 0x7dd01aab, "crypto_destroy_tfm" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0x9d669763, "memcpy" },
	{ 0xbe27dcf9, "crypto_alloc_base" },
	{ 0xea147363, "printk" },
	{ 0x6ec39390, "mem_map" },
	{ 0x3765caf8, "phys_offset" },
	{ 0x5f754e5a, "memset" },
	{ 0xc27487dd, "__bug" },
	{ 0xf88c3301, "sg_init_table" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

