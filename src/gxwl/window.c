#include "handle.h"
#include <gx/gx.h>

#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <stdint.h>
#include <string.h>

static struct gxwl_winbuf
{
	struct gx_window winbuf[GX_MAX_WINDOW_COUNT];
	uint64_t freemask;
	uint64_t usedmask;
	gx_bool_t connected;
	uint8_t nwin;
	struct wl_compositor *comp;
} Gxwl_Winbuf = {
	.winbuf = {0},
	.freemask = UINT64_MAX
};

static struct gx_window *windump = NULL;

gx_window_t *gxwl_get_freeaddr(void)
{
	if (!Gxwl_Winbuf.freemask)
		return NULL;

	uint8_t fi = GX_CTZ64(Gxwl_Winbuf.freemask);
	GX_CLR_BIT64(Gxwl_Winbuf.freemask, fi);
	Gxwl_Winbuf.usedmask = ~Gxwl_Winbuf.freemask;

	struct gx_window *win = &Gxwl_Winbuf.winbuf[fi];
	memset(win, 0, sizeof(struct gx_window));

	win->wid = (int8_t)(fi);

	++Gxwl_Winbuf.nwin;

	return win;
}

void gxwl_remove_window(struct gx_window *window)
{
	if (Gxwl_Winbuf.freemask == UINT64_MAX || !window)
		return;

	uint8_t wi = (uint8_t)(window - Gxwl_Winbuf.winbuf);
	if (wi >= GX_MAX_WINDOW_COUNT)
		return;

	if (!(Gxwl_Winbuf.freemask & (UINT64_C(1) << wi)))
	{
		GX_SET_BIT64(Gxwl_Winbuf.freemask, wi);
		Gxwl_Winbuf.usedmask = ~Gxwl_Winbuf.freemask;
	}

	--Gxwl_Winbuf.nwin;
}

GX_API
gx_window_t *gx_create_window(const uint32_t width, const uint32_t height,
			      const char *title)
{
	register struct gx_window *window = gxwl_get_freeaddr();
	if (!window)
		return NULL;

	window->surface = wl_compositor_create_surface(Gxwl_Winbuf.comp);
	if (!window->surface)
		return NULL;

	wl_proxy_set_tag((struct wl_proxy *) window->surface, NULL);
	wl_surface_add_listener(window->surface, NULL, window);

	return window;
}
