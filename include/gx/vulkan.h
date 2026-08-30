#ifndef GX_VULKAN_H_
#define GX_VULKAN_H_ 1

#include "attr.h"
#include "types.h"

#include <vulkan/vulkan.h>

#if defined(GX_PLATFORM_X11)
#define GX_VULKAN_CREATE_SURFACE_FP PFN_vkCreateXcbSurfaceKHR
#elif defined(GX_PLATFORM_WAYLAND)
#define GX_VULKAN_CREATE_SURFACE_FP PFN_vkCreateWaylandSurfaceKHR
#elif defined(GX_PLATFORM_WIN32)
#define GX_VULKAN_CREATE_SURFACE_FP PFN_vkCreateWin32SurfaceKHR
#else
#define GX_VULKAN_CREATE_SURFACE_FP ptf_gx_void_t
#endif /* GX_PLATFORM_* */

GX_API
gx_fnret_t gx_create_vulkan_surface(VkSurfaceKHR *surface, gx_window_t *window,
				    VkInstance instance,
				    GX_VULKAN_CREATE_SURFACE_FP fp,
				    const VkAllocationCallbacks *allocator);

#endif /* GX_VULKAN_H_ */
