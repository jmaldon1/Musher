#ifndef __MUSHER_CORE_H__
#define __MUSHER_CORE_H__

#if 0
#if defined(_WIN32) || defined(_WIN64)
#  if defined(MUSHER_EXPORT)
#    define MUSHER_API __declspec(dllexport)
#  else
#    define MUSHER_API __declspec(dllimport)
#  endif
#else /* Non windows OS */
#  define MUSHER_API
#endif /* _WIN32*/
#else
# define MUSHER_API
#endif


#endif /* __MUSHER_CORE_H__ */