#ifndef GOBU_COMPAT_H
#define GOBU_COMPAT_H

/* NativeExtension on top of Gobu's plugin interface: see gobu_compat.c. */

#include <gobu_plugin.h>
#include <nativeextension_interface.h>

void compat_set_api(const GobuPluginApi *p_api, GobuPluginHandle p_plugin);
NativeExtensionInterfaceFunctionPtr compat_get_proc_address(const char *p_function_name);

#endif // GOBU_COMPAT_H
