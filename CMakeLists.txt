cmake_minimum_required(VERSION 3.15)

project(quickjs LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

option(BUILD_SHARED "Build shared library" ON)
option(BUILD_LOADER "Build loader utility library" ON)

option(BUILD_COMPILER "Build compiler" ON)
option(BUILD_INTERPRETER "Build interpreter" ON)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_ECMA262_TEST "Build the ECMA Test 262 Runner" OFF)
option(BUILD_LIB_DEFAULT_VISIBILITY "Build static library with default visibility" OFF)

option(USE_MSVC_STATIC_RUNTIME "Use MSVC static runtime" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(BUILD_LOADER OFF)

    set(BUILD_TESTING OFF)
    set(BUILD_COMPILER OFF)
    set(BUILD_INTERPRETER OFF)
    set(BUILD_ECMA262_TEST OFF)

    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Oz -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Oz -g -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-Oz -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-Oz -DNDEBUG")

    add_compile_options(-fsigned-char)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(BUILD_LOADER OFF)

    set(BUILD_TESTING OFF)
    set(BUILD_ECMA262_TEST OFF)
    set(BUILD_COMPILER OFF)
    set(BUILD_INTERPRETER OFF)
    set(BUILD_SHARED OFF)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    if(USE_MSVC_STATIC_RUNTIME)
        message(STATUS "Using MSVC static runtime")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        message(STATUS "Using MSVC dynamic runtime")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
endif()

set(LIB_PUBLIC_HEADERS
    quickjs.h
    quickjs-version.h
    quickjs-libc.h
    quickjs-extern.h)

# source files
set(LIB_SOURCE_FILES 
    cutils.c
    cutils.h
    libbf.c
    libbf.h
    libregexp.c
    libregexp-opcode.h
    libregexp.h
    libunicode.c
    libunicode.h
    libunicode-table.h
    list.h
    quickjs.c
    quickjs.h
    quickjs-atom.h
    quickjs-libc.c
    quickjs-libc.h
    quickjs-opcode.h
    quickjs-extern.c
    quickjs-extern.h)

# function to add library
function(add_quickjs_library TARGET_NAME LIB_TYPE)
    add_library(${TARGET_NAME} ${LIB_TYPE} ${LIB_SOURCE_FILES})
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(${TARGET_NAME} PRIVATE QUICKJS_BUILD CONFIG_BIGNUM)
    target_compile_definitions(${TARGET_NAME} PUBLIC JS_NO_NAN_BOXING)
    set_property(TARGET ${TARGET_NAME} PROPERTY C_VISIBILITY_PRESET hidden)
    if(ANDROID AND (ANDROID_ABI MATCHES "armeabi-v7a") AND (LIB_TYPE STREQUAL "SHARED"))
        target_compile_definitions(${TARGET_NAME} PRIVATE QUICKJS_NO_LOG2)
    endif()

    if(WIN32)
        if (LIB_TYPE STREQUAL "SHARED")
            target_compile_definitions(${TARGET_NAME} PUBLIC QUICKJS_DLL)
        endif()
    else()
        if (LIB_TYPE STREQUAL "SHARED")
            target_compile_definitions(${TARGET_NAME} PRIVATE QUICKJS_BUILD_DEFAULT_VISIBILITY)
        endif()
        target_compile_definitions(${TARGET_NAME} PRIVATE _GNU_SOURCE)
        #target_compile_options(${TARGET_NAME} PRIVATE -Wnoimplicit-const-int-float-conversion)
    endif()

    if (UNIX AND NOT APPLE AND NOT IOS AND NOT ANDROID)
        find_library(MATH_LIBRARY m)
        if(MATH_LIBRARY)
            target_link_libraries(${TARGET_NAME} PUBLIC ${MATH_LIBRARY})
        endif()
    endif()

    install(TARGETS ${TARGET_NAME} 
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
endfunction()

add_quickjs_library(quickjs-static STATIC)
if (BUILD_LIB_DEFAULT_VISIBILITY)
    target_compile_definitions(quickjs-static PRIVATE QUICKJS_BUILD_DEFAULT_VISIBILITY)
endif()

install(FILES
    ${LIB_PUBLIC_HEADERS}
    DESTINATION include)

if(BUILD_SHARED)
    add_quickjs_library(quickjs SHARED)
endif()

if(BUILD_LOADER)
    add_library(quickjs-loader SHARED
        quickjs-loader.c)
    target_compile_definitions(quickjs-loader PRIVATE QUICKJS_LOADER_BUILD)
    install(TARGETS quickjs-loader
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
    if (UNIX)
        find_library(DL_LIBRARY dl REQUIRED)
        target_link_libraries(quickjs-loader PUBLIC ${DL_LIBRARY})
    endif()
endif()

if (BUILD_COMPILER)
    # qjsc executable
    add_executable(qjsc 
        qjsc.c)
    target_link_libraries(qjsc PUBLIC quickjs-static)
    install(TARGETS qjsc
            RUNTIME DESTINATION bin)

    # compiled js files
    function(target_compile_js TARGET_NAME INPUT_FILE)
        set(INPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE})
        set(OUTPUT_FILE ${INPUT_FILE}.c)
        set(OUTPUT_PATH ${OUTPUT_FILE})
        add_custom_command(
            OUTPUT ${OUTPUT_PATH}
            COMMAND qjsc -c -o ${OUTPUT_PATH} -m ${INPUT_PATH}
            DEPENDS qjsc ${INPUT_PATH}
            COMMENT "Generating ${OUTPUT_FILE}"
        )
        target_sources(${TARGET_NAME} PRIVATE ${OUTPUT_PATH})
    endfunction()
endif()

if (BUILD_INTERPRETER)
    # qjs executable
    add_executable(qjs 
        qjs.c)
    target_compile_js(qjs repl.js)
    target_link_libraries(qjs PUBLIC quickjs-static)
    install(TARGETS qjs
            RUNTIME DESTINATION bin)
endif()

if(BUILD_TESTING)
    add_executable(test-extern-static 
            test-extern.c)
    target_link_libraries(test-extern-static PUBLIC quickjs-static)
    add_test(NAME test-extern-static COMMAND $<TARGET_FILE:test-extern-static>)
    
    if(BUILD_SHARED)
        add_executable(test-extern 
            test-extern.c)
        target_link_libraries(test-extern PUBLIC quickjs)
        add_test(NAME test-extern COMMAND $<TARGET_FILE:test-extern>)
    endif()

    if(BUILD_ECMA262_TEST)
        if(WIN32)
            message(FATAL_ERROR "ECMA262 test runner is not supported on Windows")
        endif()

        add_executable(run-test262
            run-test262.c)
        target_link_libraries(run-test262 PUBLIC quickjs-static)
        if(BUILD_SHARED)
            add_executable(run-test262-shared 
                run-test262.c)
            target_link_libraries(run-test262-shared PUBLIC quickjs)
        endif()
    endif()
endif()
