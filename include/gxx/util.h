#ifndef GX_X11_UTIL_H_
#define GX_X11_UTIL_H_ 1

#include "gx/keymap.h"

#include <stdint.h>

gx_keycode_t gxx_cvt_keycode(uint8_t keycode);

gx_btncode_t gxx_cvt_btncode(uint8_t btncode);

#endif /* GX_X11_UTIL_H_ */
