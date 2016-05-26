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
	{ 0x66327268, "d_path" },
	{ 0x8b82bb5d, "device_remove_file" },
	{ 0xcbc29716, "kmalloc_caches" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xd5b037e1, "kref_put" },
	{ 0x2b688622, "complete_and_exit" },
	{ 0xab455eb0, "up_read" },
	{ 0xb05da734, "dequeue_signal" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x97255bdf, "strlen" },
	{ 0x825b7b9a, "dev_set_drvdata" },
	{ 0x82f79f9b, "bdev_read_only" },
	{ 0x20000329, "simple_strtoul" },
	{ 0xf7802486, "__aeabi_uidivmod" },
	{ 0x43ab66c3, "param_array_get" },
	{ 0xf7940de2, "filp_close" },
	{ 0x4e830a3e, "strnicmp" },
	{ 0x549d0f72, "vfs_fsync" },
	{ 0x14f53ed6, "usb_gadget_unregister_driver" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0x45947727, "param_array_set" },
	{ 0xc6d533e1, "down_read" },
	{ 0xf6288e02, "__init_waitqueue_head" },
	{ 0x41344088, "param_get_charp" },
	{ 0xe707d823, "__aeabi_uidiv" },
	{ 0x5baaba0, "wait_for_completion" },
	{ 0x4f18d75b, "vfs_read" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x335afd4c, "device_register" },
	{ 0x11089ac7, "_ctype" },
	{ 0xea147363, "printk" },
	{ 0x42224298, "sscanf" },
	{ 0x71c90087, "memcmp" },
	{ 0x9f2d613e, "param_set_bool" },
	{ 0xd2267400, "init_uts_ns" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0xadb3d81e, "up_write" },
	{ 0xb1b22911, "down_write" },
	{ 0x7a140364, "fput" },
	{ 0xd79b5a02, "allow_signal" },
	{ 0x8f292be0, "device_create_file" },
	{ 0x9f984513, "strrchr" },
	{ 0x68a3b589, "kmem_cache_alloc" },
	{ 0x20130621, "usb_gadget_register_driver" },
	{ 0x52ebb126, "param_get_ushort" },
	{ 0x108e8985, "param_get_uint" },
	{ 0x1000e51, "schedule" },
	{ 0x35c2ba9e, "refrigerator" },
	{ 0x4336eda0, "dev_driver_string" },
	{ 0xd7ffa5d1, "wake_up_process" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x6ad065f4, "param_set_charp" },
	{ 0x83800bfa, "kref_init" },
	{ 0x37a0cba, "kfree" },
	{ 0x4cc6ee2, "kthread_create" },
	{ 0x9d669763, "memcpy" },
	{ 0x443b426f, "send_sig_info" },
	{ 0x3285cc48, "param_set_uint" },
	{ 0x9775cdc, "kref_get" },
	{ 0x44a33c35, "invalidate_mapping_pages" },
	{ 0xe189afda, "device_unregister" },
	{ 0x9214ed8a, "param_get_bool" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0x60f71cfa, "complete" },
	{ 0x701d0ebd, "snprintf" },
	{ 0x99bb8806, "memmove" },
	{ 0x9f8a9667, "dev_set_name" },
	{ 0x2fc251f5, "dev_get_drvdata" },
	{ 0xaa94c694, "__init_rwsem" },
	{ 0xa06a4e7c, "vfs_write" },
	{ 0xe914e41e, "strcpy" },
	{ 0x8ef18a0c, "filp_open" },
	{ 0xe0bc24a1, "param_set_ushort" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=arcotg_udc";

