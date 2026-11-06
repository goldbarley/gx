#ifndef GX_WAYLAND_HANDLE_H_
#define GX_WAYLAND_HANDLE_H_ 1

#include "gx/types.h"

#include <wayland-client.h>

#include <stdint.h>

struct gx_window
{
	struct wl_display *display;
	struct wl_surface *surface;
	uint32_t width;
	uint32_t height;
	int8_t wid;
	gx_bool_t focused;
	gx_bool_t ptrin;
};

#endif /* GX_WAYLAND_HANDLE_H_ */
