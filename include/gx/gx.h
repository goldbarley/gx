#ifndef GX_H_
#define GX_H_ 1

#include "attr.h"
#include "event.h"
#include "keymap.h"
#include "math.h"
#include "types.h"
#include "window.h"

#ifdef __linux__
#include "linux.h"
#if defined(GX_PLATFORM_X11)
#include "gxx/util.h"
#elif defined(GX_PLATFORM_WAYLAND)
#endif /* GX_PLATFORM_* */
#endif /* __linux__ */

#endif /* GX_H_ */
