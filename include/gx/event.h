#ifndef GX_EVENT_H_
#define GX_EVENT_H_ 1

#include "keymap.h"
#include "types.h"

#include <stdbool.h>

enum gx_event_type
{
	GX_EVENT_UNKNOWN = 0,
	GX_EVENT_KEY_PRESS,
	GX_EVENT_KEY_RELEASE,
	GX_EVENT_BTN_PRESS,
	GX_EVENT_BTN_RELEASE,
	GX_EVENT_PTR_MOVE,
	GX_EVENT_SCROLL,
	GX_EVENT_FOCUS_IN,
	GX_EVENT_FOCUS_OUT,
	GX_EVENT_PTR_IN,
	GX_EVENT_PTR_OUT,
	GX_EVENT_WINDOW_CONFIGURE,
	GX_EVENT_WINDOW_CLOSE,
};

struct gx_event
{
	enum gx_event_type type;
	gx_window_t *window;

	union
	{
		struct
		{
			gx_keycode_t id;
		} key;

		struct
		{
			gx_btncode_t id;
			float x;
			float y;
		} ptr;

		struct
		{
			float dx;
			float dy;
		} scroll;
	};
};

typedef gx_fnret_t (*ptf_gx_evt_callback_t)(struct gx_event *, void *user_data);

#endif /* GX_EVENT_H_ */
