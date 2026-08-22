#ifndef WHIS_WINDOW_H_
#define WHIS_WINDOW_H_ 1

#include "attr.h"
#include "event.h"
#include "types.h"

#define WHIS_MAX_WINDOW_COUNT (64U)

WHIS_EXPORT
wh_window_t *wh_create_window(const uint32_t width, const uint32_t height,
			      const char *title);

WHIS_EXPORT
void wh_destroy_window(wh_window_t *window);

WHIS_EXPORT
wh_bool_t wh_poll_for_event(struct wh_event *event);

WHIS_EXPORT
wh_bool_t wh_wait_for_event(struct wh_event *event);

WHIS_EXPORT
wh_fnret_t wh_get_framebuffer_size(wh_window_t *window, uint32_t *width,
				   uint32_t *height);

WHIS_EXPORT
wh_bool_t wh_window_t_in_focus(wh_window_t *window);

WHIS_EXPORT
wh_bool_t wh_ptr_in_window(wh_window_t *window);

WHIS_EXPORT
wh_fnret_t wh_resize_window(wh_window_t *window, const uint32_t width,
			    const uint32_t height);

WHIS_EXPORT
wh_fnret_t wh_sync_window_size(wh_window_t *window);

WHIS_EXPORT
int8_t wh_get_window_id(wh_window_t *window);

WHIS_EXPORT
uint8_t wh_get_nwin(void);

WHIS_EXPORT
void wh_shutdown(void);

#endif /* WHIS_WINDOW_H_ */
