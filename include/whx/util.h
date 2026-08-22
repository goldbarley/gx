#ifndef WHIS_X11_UTIL_H_
#define WHIS_X11_UTIL_H_ 1

#include "whis/keymap.h"

#include <stdint.h>

wh_keycode_t whx_cvt_keycode(uint8_t keycode);

wh_btncode_t whx_cvt_btncode(uint8_t btncode);

#endif /* WHIS_X11_UTIL_H_ */
