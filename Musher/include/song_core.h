#ifndef __SONG_CORE_H__
#define __SONG_CORE_H__

#ifdef _WIN32
#  if defined(SONG_EXPORT)
#    define SONG_API __declspec(dllexport)
#  elif defined(SONG_IMPORT)
#    define SONG_API __declspec(dllimport)
#  else
#    define SONG_API
#endif
#else
#  define SONG_API
#endif

#endif