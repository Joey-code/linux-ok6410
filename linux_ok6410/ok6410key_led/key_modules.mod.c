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
	{ 0x6d4d6df5, "s3c_gpio_cfgpin" },
	{ 0x27e1a049, "printk" },
	{ 0x93d626c3, "misc_register" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x8893fa5d, "finish_wait" },
	{ 0x75a17bed, "prepare_to_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x98082893, "__copy_to_user" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0xb9e52429, "__wake_up" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x4ad15b1c, "misc_deregister" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2514CB2E88A9B9344CC88E0");
