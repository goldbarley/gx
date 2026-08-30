#include <gx/gx.h>
#include <vulkan/vulkan.h>

#include <stdio.h>

static uint64_t cnt = 0;

int event_handler(struct gx_event *evt)
{
	switch (evt->type)
	{
		case GX_EVENT_KEY_PRESS:
			printf("KEY PRESSED: %x\n", evt->key.id);
			fflush(stdout);
			break;
		case GX_EVENT_KEY_RELEASE:
			printf("KEY RELEASED: %x\n", evt->key.id);
			fflush(stdout);
			break;
		case GX_EVENT_BTN_PRESS:
			printf("BUTTON PRESSED: %x\n", evt->ptr.id);
			fflush(stdout);
			break;
		case GX_EVENT_BTN_RELEASE:
			printf("BUTTON RELEASED: %x\n", evt->ptr.id);
			fflush(stdout);
			break;
		case GX_EVENT_FOCUS_IN:
			printf("%lu FOCUSED: %i\n", cnt, gx_get_window_id(evt->window));
			fflush(stdout);
			break;
		case GX_EVENT_FOCUS_OUT:
			printf("%lu UNFOCUSED: %i\n", cnt, gx_get_window_id(evt->window));
			fflush(stdout);
			break;
		case GX_EVENT_PTR_MOVE:
			// printf("X: %.3f Y: %.3f\n", evt->ptr.x, evt->ptr.y);
			// fflush(stdout);
			break;
		case GX_EVENT_PTR_IN:
			printf("%lu POINTER IN: %i\n", cnt, gx_get_window_id(evt->window));
			fflush(stdout);
			break;
		case GX_EVENT_PTR_OUT:
			printf("%lu POINTER OU: %i\n", cnt, gx_get_window_id(evt->window));
			fflush(stdout);
			break;
		case GX_EVENT_WINDOW_CONFIGURE:
			puts("WINDOW RESIZE!");
			fflush(stdout);
			gx_sync_window_size(evt->window);
			break;
		case GX_EVENT_WINDOW_CLOSE:
			printf("WINDOW CLOSE: %i\n", gx_get_window_id(evt->window));
			gx_destroy_window(evt->window);
			break;
		default:
			return -1;
	}

	++cnt;

	return 0;
}

int main(void)
{
	gx_window_t *window1 = gx_create_window(1280, 720, "XWINDOW1");
	if (!window1)
		return -1;

	gx_window_t *window2 = gx_create_window(800, 600, "XWINDOW2");
	if (!window2)
		return -1;


	struct gx_event evt = {0};
	uint8_t actwins = gx_get_nwin();

	while (gx_get_nwin())
	{
		while (gx_poll_for_event(&evt))
			event_handler(&evt);
	}

	gx_shutdown();

	return 0;
}
