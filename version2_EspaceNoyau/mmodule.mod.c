#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xb3753869, "module_layout" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x78f44845, "cdev_del" },
	{ 0x7afe113a, "cdev_add" },
	{ 0xa3036ef8, "cdev_init" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xb8e7ce2c, "__put_user_8" },
	{ 0xb1e12d81, "krealloc" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x409bcb62, "mutex_unlock" },
	{ 0x2ab7989d, "mutex_lock" },
	{ 0x977f511b, "__mutex_init" },
	{ 0xf5cb25c8, "kmem_cache_alloc_trace" },
	{ 0x35216b26, "kmalloc_caches" },
	{ 0x38dab310, "filp_open" },
	{ 0xc5850110, "printk" },
	{ 0xceaad73f, "filp_close" },
	{ 0x37a0cba, "kfree" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3155D5BB1BAA47D864E788D");
