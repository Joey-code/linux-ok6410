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
	{ 0xd0a8ddc4, "module_layout" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0x779636b1, "device_create" },
	{ 0x4c38cbec, "__class_create" },
	{ 0x78713c2f, "__register_chrdev" },
	{ 0x98082893, "__copy_to_user" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0x6d4d6df5, "s3c_gpio_cfgpin" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xf9a482f9, "msleep" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0x8b6dfcfd, "class_destroy" },
	{ 0x21f3dd24, "device_unregister" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3137BB400F2C8D13E6B9D56");
