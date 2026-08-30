#include "handle.h"
#include "gx/attr.h"
#include "gx/event.h"
#include "gx/math.h"
#include "gx/types.h"
#include "gxx/util.h"
#include "gx/window.h"

#include <xcb/xcb.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

static struct gxx_winbuf
{
	struct gx_window winbuf[GX_MAX_WINDOW_COUNT];
	xcb_connection_t *connection;
	xcb_atom_t wm_protocols;
	xcb_atom_t wm_del_win;
	uint64_t freemask;
	uint64_t usedmask;
	gx_bool_t connected;
	uint8_t nwin;
} Gxx_Winbuf = {
	.winbuf = {0},
	.freemask = UINT64_MAX
};

static struct gx_window *windump = NULL;

gx_window_t *gxx_get_freeaddr(void)
{
	if (!Gxx_Winbuf.freemask)
		return NULL;

	uint8_t fi = GX_CTZ64(Gxx_Winbuf.freemask);
	GX_CLR_BIT64(Gxx_Winbuf.freemask, fi);
	Gxx_Winbuf.usedmask = ~Gxx_Winbuf.freemask;

	struct gx_window *win = &Gxx_Winbuf.winbuf[fi];
	memset(win, 0, sizeof(struct gx_window));

	win->wid = (int8_t)(fi);

	++Gxx_Winbuf.nwin;

	return win;
}

void gxx_remove_window(struct gx_window *window)
{
	if (Gxx_Winbuf.freemask == UINT64_MAX || !window)
		return;

	uint8_t wi = (uint8_t)(window - Gxx_Winbuf.winbuf);
	if (wi >= GX_MAX_WINDOW_COUNT)
		return;

	if (!(Gxx_Winbuf.freemask & (UINT64_C(1) << wi)))
	{
		GX_SET_BIT64(Gxx_Winbuf.freemask, wi);
		Gxx_Winbuf.usedmask = ~Gxx_Winbuf.freemask;
	}

	--Gxx_Winbuf.nwin;
}

GX_API
gx_window_t *gx_create_window(const uint32_t width, const uint32_t height,
			      const char *title)
{
	static xcb_connection_t *cnn;

	if (!Gxx_Winbuf.connected)
	{
		cnn = xcb_connect(NULL, NULL);
		if (xcb_connection_has_error(cnn))
			return NULL;

		xcb_intern_atom_cookie_t wm_protocols =
			xcb_intern_atom(cnn, 0, 12, "WM_PROTOCOLS");

		xcb_intern_atom_cookie_t wm_del_win =
			xcb_intern_atom(cnn, 0, 16, "WM_DELETE_WINDOW");

		xcb_intern_atom_reply_t *reply_protocols =
			xcb_intern_atom_reply(cnn, wm_protocols, NULL);

		xcb_intern_atom_reply_t *reply_del_win =
			xcb_intern_atom_reply(cnn, wm_del_win, NULL);

		if (!reply_protocols || !reply_del_win)
		{
			free(reply_protocols);
			free(reply_del_win);
			xcb_disconnect(cnn);

			return NULL;
		}

		Gxx_Winbuf.wm_protocols = reply_protocols->atom;
		Gxx_Winbuf.wm_del_win = reply_del_win->atom;

		free(reply_protocols);
		free(reply_del_win);

		Gxx_Winbuf.connected = GX_TRUE;
		Gxx_Winbuf.connection = cnn;
	}

	register struct gx_window *win = gxx_get_freeaddr();
	if (!win)
		return NULL;

	win->width = width;
	win->height = height;

	win->connection = cnn;

	const xcb_setup_t *setup = xcb_get_setup(cnn);
	xcb_screen_iterator_t scriter = xcb_setup_roots_iterator(setup);
	xcb_screen_t *scr = scriter.data;

	xcb_cw_t mask = XCB_CW_EVENT_MASK;

	static xcb_event_mask_t evtmask =
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_ENTER_WINDOW |
		XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_FOCUS_CHANGE |
		XCB_EVENT_MASK_KEY_PRESS |
		XCB_EVENT_MASK_KEY_RELEASE |
		XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY;

	xcb_window_t xwin = xcb_generate_id(cnn);
	win->window = xwin;

	xcb_create_window(cnn, XCB_COPY_FROM_PARENT, xwin, scr->root, 0, 0,
			  width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
			  scr->root_visual, mask, &evtmask);

	if (title)
		xcb_change_property(cnn, XCB_PROP_MODE_REPLACE, xwin,
				    XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
				    8, strlen(title), title);

	xcb_change_property(cnn, XCB_PROP_MODE_REPLACE, xwin,
			    Gxx_Winbuf.wm_protocols, XCB_ATOM_ATOM, 32, 1,
			    &Gxx_Winbuf.wm_del_win);

	xcb_map_window(cnn, xwin);
	xcb_flush(cnn);

	return win;
}

static struct gx_window *gxx_get_window(xcb_window_t xwin)
{
	uint64_t umask = Gxx_Winbuf.usedmask;
	uint8_t i = 0;

	while(umask)
	{
		i = GX_CTZ64(umask);
		if (Gxx_Winbuf.winbuf[i].window == xwin)
			return &Gxx_Winbuf.winbuf[i];
		GX_CLR_BIT64(umask, i);
	}

	return NULL;
}

GX_HOT
static gx_bool_t gxx_next_event(struct gx_event *event,
				xcb_generic_event_t *(*get_event)
				(xcb_connection_t *))
{
	if (!event || !Gxx_Winbuf.connected)
		return GX_FALSE;

	xcb_generic_event_t *evt;

	struct gx_window *window = NULL;
	register struct gx_event *revent = event;

	revent->window = NULL;
	revent->type = GX_EVENT_UNKNOWN;

	uint32_t rt = 0;
	while ((evt = get_event(Gxx_Winbuf.connection)))
	{
		rt = evt->response_type & ~(0x80);
		switch(rt)
		{
			case XCB_KEY_PRESS:
			case XCB_KEY_RELEASE:
			{
				xcb_key_press_event_t *kpevt =
					(xcb_key_press_event_t *)(evt);

				window = gxx_get_window(kpevt->event);
				if (!window)
					break;

				revent->window = window;
				revent->type = (rt == XCB_KEY_PRESS) ?
					GX_EVENT_KEY_PRESS :
					GX_EVENT_KEY_RELEASE;
				revent->key.id = gxx_cvt_keycode(kpevt->detail);

				break;
			}

			case XCB_BUTTON_PRESS:
			case XCB_BUTTON_RELEASE:
			{
				xcb_button_press_event_t *bpevt =
					(xcb_button_press_event_t *)(evt);

				window = gxx_get_window(bpevt->event);
				if (!window)
					break;

				revent->window = window;
				revent->type = (rt == XCB_BUTTON_PRESS) ?
					GX_EVENT_BTN_PRESS :
					GX_EVENT_BTN_RELEASE;
				revent->ptr.id = gxx_cvt_btncode(bpevt->detail);

				break;
			}

			case XCB_MOTION_NOTIFY:
			{
				xcb_motion_notify_event_t *mevt =
					(xcb_motion_notify_event_t *)(evt);

				window = gxx_get_window(mevt->event);
				if (!window)
					break;

				revent->window = window;
				revent->type = GX_EVENT_PTR_MOVE;
				revent->ptr.x = mevt->event_x;
				revent->ptr.y = mevt->event_y;

				break;
			}

			case XCB_FOCUS_IN:
			{
				xcb_focus_in_event_t *fievt =
					(xcb_focus_in_event_t *)(evt);

				window = gxx_get_window(fievt->event);
				if (!window)
					break;
				window->focused = GX_TRUE;

				revent->window = window;
				revent->type = GX_EVENT_FOCUS_IN;

				break;
			}
			case XCB_FOCUS_OUT:
			{
				xcb_focus_out_event_t *foevt =
					(xcb_focus_out_event_t *)(evt);

				window = gxx_get_window(foevt->event);
				if (!window)
					break;
				window->focused = GX_FALSE;

				revent->window = window;
				revent->type = GX_EVENT_FOCUS_OUT;

				break;
			}

			case XCB_ENTER_NOTIFY:
			{
				xcb_enter_notify_event_t *enevt =
					(xcb_enter_notify_event_t *)(evt);

				window = gxx_get_window(enevt->event);
				if (!window)
					break;
				window->ptrin = GX_TRUE;

				revent->window = window;
				revent->type = GX_EVENT_PTR_IN;

				break;
			}
			case XCB_LEAVE_NOTIFY:
			{
				xcb_leave_notify_event_t *lnevt =
					(xcb_leave_notify_event_t *)(evt);

				window = gxx_get_window(lnevt->event);
				if (!window)
					break;
				window->ptrin = GX_FALSE;

				revent->window = window;
				revent->type = GX_EVENT_PTR_OUT;

				break;
			}

			case XCB_CONFIGURE_NOTIFY:
			{
				xcb_configure_notify_event_t *cevt =
					(xcb_configure_notify_event_t *)(evt);

				window = gxx_get_window(cevt->event);
				if (!window)
					break;

				window->width = cevt->width;
				window->height = cevt->height;

				revent->window = window;
				revent->type = GX_EVENT_WINDOW_CONFIGURE;

				break;
			}

			case XCB_CLIENT_MESSAGE:
			{
				xcb_client_message_event_t *cmevt =
					(xcb_client_message_event_t *)(evt);

				window = gxx_get_window(cmevt->window);
				if (!window)
					break;

				revent->window = window;

				if (cmevt->data.data32[0] == Gxx_Winbuf.wm_del_win)
				{
					revent->type = GX_EVENT_WINDOW_CLOSE;

					break;
				}

				revent->type = GX_EVENT_UNKNOWN;

				break;
			}

			default:
				break;

		}
		free(evt);

		if (revent->type != GX_EVENT_UNKNOWN)
			return GX_TRUE;
	}

	return GX_FALSE;
}

GX_API GX_HOT
gx_bool_t gx_poll_for_event(struct gx_event *event)
{
	return gxx_next_event(event, xcb_poll_for_event);
}

GX_API GX_HOT
gx_bool_t gx_wait_for_event(struct gx_event *event)
{
	return gxx_next_event(event, xcb_wait_for_event);
}

GX_API
gx_fnret_t gx_get_framebuffer_size(gx_window_t *win, uint32_t *width,
				   uint32_t *height)
{
	if (!win || !width || !height)
		return GX_INVALID_ARGUMENT;

	*width = win->width;
	*height = win->height;

	return GX_SUCCESS;
}

GX_API
gx_bool_t gx_window_in_focus(gx_window_t *window)
{
	return window->focused;
}

GX_API
gx_bool_t gx_ptr_in_window(gx_window_t *window)
{
	return window->ptrin;
}

GX_API
gx_fnret_t gx_resize_window(gx_window_t *window, const uint32_t width,
			    const uint32_t height)
{
	if (!window)
		return GX_INVALID_ARGUMENT;

	register struct gx_window *rwin = window;
	const uint32_t values[2] = { width, height };


	xcb_configure_window(rwin->connection,
			     rwin->window,
			     XCB_CONFIG_WINDOW_WIDTH |
			     XCB_CONFIG_WINDOW_HEIGHT,
		             values);

	return GX_SUCCESS;
}

GX_API
gx_fnret_t gx_sync_window_size(gx_window_t *window)
{
	if (!window)
		return GX_INVALID_ARGUMENT;

	const uint32_t values[2] = { window->width, window->height };

	xcb_configure_window(window->connection,
			     window->window,
			     XCB_CONFIG_WINDOW_WIDTH |
			     XCB_CONFIG_WINDOW_HEIGHT,
			     values);

	return GX_SUCCESS;
}

GX_API
int8_t gx_get_window_id(gx_window_t *window)
{
	if (!window)
		return -1;

	return window->wid;
}

GX_API
uint8_t gx_get_nwin(void)
{
	return Gxx_Winbuf.nwin;
}

GX_API
void gx_destroy_window(gx_window_t *win)
{
	if (!win)
		return;

	xcb_destroy_window(win->connection, win->window);
	xcb_flush(win->connection);
	gxx_remove_window(win);
}

GX_API
void gx_shutdown(void)
{
	uint8_t i = 0;
	uint64_t umask = Gxx_Winbuf.usedmask;

	while (umask)
	{
		i = GX_CTZ64(umask);
		windump = &Gxx_Winbuf.winbuf[i];
		gx_destroy_window(windump);
		GX_CLR_BIT64(umask, i);
	}

	if (Gxx_Winbuf.connection)
		xcb_disconnect(Gxx_Winbuf.connection);

	memset(&Gxx_Winbuf, 0, sizeof(Gxx_Winbuf));
}
