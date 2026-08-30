#ifndef GX_ATTR_H_
#define GX_ATTR_H_ 1

#if defined(_MSC_VER)
#ifdef GX_LIB_STATIC
#define GX_API
#elif defined(GX_LIB_SHARED)
#define GX_API __declspec(dllexport)
#else
#define GX_API __declspec(dllimport)
#endif
#define GX_INLINE static __forceinline
#else
#ifdef GX_LIB_SHARED
#define GX_API __attribute__((__visibility__("default")))
#else
#define GX_API
#endif /* GX_LIB_SHARED */
#define GX_INLINE static inline __attribute__((__always_inline__))
#endif /* Compilers */

#if defined(__GNUC__) || defined(__clang__)
#define GX_HOT __attribute__((__hot__))
#else
#define GX_HOT
#endif /* Compilers */

#endif /* GX_ATTR_H_ */
