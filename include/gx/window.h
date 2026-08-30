#ifndef GX_WINDOW_H_
#define GX_WINDOW_H_ 1

#include "attr.h"
#include "event.h"
#include "types.h"

#define GX_MAX_WINDOW_COUNT (64U)

GX_API
gx_window_t *gx_create_window(const uint32_t width, const uint32_t height,
			      const char *title);

GX_API
void gx_destroy_window(gx_window_t *window);

GX_API GX_HOT
gx_bool_t gx_poll_for_event(struct gx_event *event);

GX_API GX_HOT
gx_bool_t gx_wait_for_event(struct gx_event *event);

GX_API
gx_fnret_t gx_get_framebuffer_size(gx_window_t *window, uint32_t *width,
				   uint32_t *height);

GX_API
gx_bool_t gx_window_t_in_focus(gx_window_t *window);

GX_API
gx_bool_t gx_ptr_in_window(gx_window_t *window);

GX_API
gx_fnret_t gx_resize_window(gx_window_t *window, const uint32_t width,
			    const uint32_t height);

GX_API
gx_fnret_t gx_sync_window_size(gx_window_t *window);

GX_API
int8_t gx_get_window_id(gx_window_t *window);

GX_API
uint8_t gx_get_nwin(void);

GX_API
void gx_shutdown(void);

#endif /* GX_WINDOW_H_ */
