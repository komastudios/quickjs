#ifndef QUICKJS_LOADER_H
#define QUICKJS_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#   ifdef QUICKJS_LOADER_BUILD
#       define QUICKJS_LOADER_EXPORT __declspec(dllexport)
#   else
#       define QUICKJS_LOADER_EXPORT __declspec(dllimport)
#   endif
#else
#   define QUICKJS_LOADER_EXPORT __attribute__((visibility("default")))
#endif

QUICKJS_LOADER_EXPORT void* loader_load_library(char* lpFileName);
QUICKJS_LOADER_EXPORT void loader_free_library(void* hModule);
QUICKJS_LOADER_EXPORT void* loader_get_proc_address(void* hModule, char* procedureName);
QUICKJS_LOADER_EXPORT char* library_get_last_error();
QUICKJS_LOADER_EXPORT void library_free_string(void* ptr);

#ifdef __cplusplus
}
#endif

#endif //QUICKJS_LOADER_H
