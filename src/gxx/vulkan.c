#include "handle.h"
#include "gx/attr.h"
#include "gx/vulkan.h"

#include <vulkan/vulkan.h>

GX_API
gx_fnret_t gx_create_vulkan_surface(VkSurfaceKHR *surface, gx_window_t *window,
				    VkInstance instance,
				    GX_VULKAN_CREATE_SURFACE_FP fp,
				    const VkAllocationCallbacks *allocator)
{
	if (!surface || !window)
		return GX_INVALID_ARGUMENT;

	VkResult vkres = VK_SUCCESS;

	VkXcbSurfaceCreateInfoKHR createinfo = {
		.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
		.pNext = NULL,
		.connection = window->connection,
		.window = window->window
	};

	vkres = fp(instance, &createinfo, allocator, surface);
	if (vkres != VK_SUCCESS)
		return GX_FAILURE;

	return GX_SUCCESS;
}
