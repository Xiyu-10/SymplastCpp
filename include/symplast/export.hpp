#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(SYMPLAST_BUILD_SHARED)
#if defined(SYMPLAST_EXPORTS)
#define SYMPLAST_EXPORT __declspec(dllexport)
#else
#define SYMPLAST_EXPORT __declspec(dllimport)
#endif
#else
#define SYMPLAST_EXPORT
#endif
#elif defined(__GNUC__) || defined(__clang__)
#if defined(SYMPLAST_BUILD_SHARED)
#define SYMPLAST_EXPORT __attribute__((visibility("default")))
#else
#define SYMPLAST_EXPORT
#endif
#else
#define SYMPLAST_EXPORT
#endif