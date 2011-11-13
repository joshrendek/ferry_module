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
	{ 0x73ec1c47, "module_layout" },
	{ 0x43d57ce, "wake_up_process" },
	{ 0x46917606, "kthread_create_on_node" },
	{ 0xf63f8767, "__mutex_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0x3e7e5a8f, "mutex_unlock" },
	{ 0x61fb6b3e, "mutex_lock" },
	{ 0x50eedeb8, "printk" },
	{ 0x945d6ed8, "STUB_ferry_stop" },
	{ 0x1c4b2a61, "STUB_ferry_request" },
	{ 0x66957295, "STUB_ferry_start" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "E592D97392713C64A85044A");
