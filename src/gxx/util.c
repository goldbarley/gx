#include "gxx/util.h"
#include "gx/keymap.h"

#ifdef __linux__
#include "gx/linux.h"

gx_keycode_t gxx_cvt_keycode(uint8_t keycode)
{
	return keycode < 8 ? 0 : Gx_EvdevTo_Gx_Keymap[keycode - 8];
}

gx_btncode_t gxx_cvt_btncode(uint8_t btncode)
{
	switch (btncode)
	{
		case 1:
			return GX_BTN_1;
		case 2:
			return GX_BTN_3;
		case 3:
			return GX_BTN_2;
		case 8:
			return GX_BTN_4;
		case 9:
			return GX_BTN_5;
		default:
			if (btncode > 9)
				return GX_BTN_X;
		return 0;
	}
}
#endif /* __linux__ */
