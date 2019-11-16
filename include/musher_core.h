#ifndef __MUSHER_CORE_H__
#define __MUSHER_CORE_H__

#ifdef _WIN32
#  if defined(MUSHER_EXPORT)
#    define MUSHER_API __declspec(dllexport)
#  else
#    define MUSHER_API __declspec(dllimport)
#  endif
#else
#  define MUSHER_API
#endif

#endif