#ifndef GX_X11_HANDLE_H_
#define GX_X11_HANDLE_H_ 1

#include "gx/types.h"

#include <xcb/xcb.h>

#include <stdint.h>

struct gx_window
{
	xcb_connection_t *connection;
	xcb_window_t window;
	uint32_t width;
	uint32_t height;
	int8_t wid;
	gx_bool_t focused;
	gx_bool_t ptrin;
};

#endif /* GX_X11_HANDLE_H_ */
