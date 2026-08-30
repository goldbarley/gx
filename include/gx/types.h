#ifndef GX_TYPES_H_
#define GX_TYPES_H_ 1

#include <stdint.h>

typedef enum gx_fnret
{
	GX_SUCCESS = 0,
	GX_INCOMPLETE = 1,
	GX_FAILURE = -1,
	GX_INVALID_ARGUMENT = -2
} gx_fnret_t;

typedef struct gx_window gx_window_t;

#define GX_FALSE (0)
#define GX_TRUE (1)

typedef uint8_t gx_bool_t;

typedef void *(*ptf_gx_void_t)(void);

#endif /* GX_TYPES_H_ */
