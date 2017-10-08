/*! exports.h */

#ifndef _UTAR_VERSION_H_
#define _UTAR_VERSION_H_

#if defined(UTAR_STATIC)
#  define UTAR_API
#elif defined(_WIN32) && !defined(__GCC__)
#  ifdef BUILDING_UTAR_SHARED
#    define UTAR_API __declspec(dllexport)
#  else
#    define UTAR_API __declspec(dllimport)
#  endif
#  ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#else
#  ifdef BUILDING_UTAR_SHARED
#    define UTAR_API __attribute__ ((visibility ("default")))
#  else
#    define UTAR_API 
#  endif
#endif

#if defined(__cplusplus)
#  define UTAR_EXTERN_C extern "C"
#  define UTAR_C_API UTAR_EXTERN_C UTAR_API
#else
#  define UTAR_EXTERN_C
#  define UTAR_C_API UTAR_API
#endif

#include <stddef.h>
#include <ctype.h>
#include <stdint.h>

#endif/*_UTAR_VERSION_H_*/
