#include "quickjs-loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else   // Unix-like
#include <dlfcn.h>
#endif

#if defined(_WIN32)

void* loader_load_library(char* lpFileName){
    return LoadLibrary(lpFileName);
}

void loader_free_library(void* hModule){
    FreeLibrary(hModule);
}

void* loader_get_proc_address(void* hModule, char* procedureName){
    return GetProcAddress(hModule, procedureName);
}

char* library_get_last_error(){
    DWORD error_value = GetLastError();

    if (error_value == 0) {
        return NULL;
    }

    LPVOID lpMsgBuf;

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error_value,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

    char* error_message = strdup((char*)lpMsgBuf);
    LocalFree(lpMsgBuf);

    return error_message;
}

#else   // Unix-like

void* loader_load_library(char* lpFileName){
    return dlopen(lpFileName, RTLD_LAZY);
}

void loader_free_library(void* hModule){
    dlclose(hModule);
}

void* loader_get_proc_address(void* hModule, char* procedureName){
    return dlsym(hModule, procedureName);
}

char* library_get_last_error(){
    char* dlerror_message = dlerror();

    if (dlerror_message == NULL) {
        return NULL;
    }

    char* error_message = strdup(dlerror_message);
    return error_message;
}

#endif

void library_free_string(void* ptr){
    free(ptr);
}
