#ifndef QUICKJS_EXTERN_H
#define QUICKJS_EXTERN_H

#include <stdint.h>

#include "quickjs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JS_EVAL_TYPE_GLOBAL
#   define JS_EVAL_TYPE_GLOBAL   (0 << 0) /* global code (default) */
#   define JS_EVAL_TYPE_MODULE   (1 << 0) /* module code */
#   define JS_EVAL_TYPE_DIRECT   (2 << 0) /* direct call (internal use) */
#   define JS_EVAL_TYPE_INDIRECT (3 << 0) /* indirect call (internal use) */
#   define JS_EVAL_TYPE_MASK     (3 << 0)
#endif

enum {
    JS_OK = 0,
    JS_ERR_EXCEPTION = -1
};

enum {
    JS_VALUE_TYPE_UNINITIALIZED = 0,
    JS_VALUE_TYPE_INT = 1,
    JS_VALUE_TYPE_BOOL = 2,
    JS_VALUE_TYPE_NULL = 3,
    JS_VALUE_TYPE_UNDEFINED = 4,
    JS_VALUE_TYPE_CATCH_OFFSET = 5,
    JS_VALUE_TYPE_EXCEPTION = 6,
    JS_VALUE_TYPE_FLOAT64 = 7,

    /* all tags with a reference count have 0b1000 bit */
    JS_VALUE_TYPE_OBJECT = 8,
    JS_VALUE_TYPE_FUNCTION_BYTECODE = 9, /* used internally */
    JS_VALUE_TYPE_MODULE = 10, /* used internally */
    JS_VALUE_TYPE_STRING = 11,
    JS_VALUE_TYPE_SYMBOL = 12,
    JS_VALUE_TYPE_BIG_FLOAT = 13,
    JS_VALUE_TYPE_BIG_INT = 14,
    JS_VALUE_TYPE_BIG_DECIMAL = 15
};

QUICKJS_EXPORT const char* js_version(void);

QUICKJS_EXPORT JSValue* js_value_new(JSContext* ctx);

QUICKJS_EXPORT void js_value_free(JSContext* ctx, JSValue* value);

QUICKJS_EXPORT void js_value_load_int32(JSContext* ctx, JSValue* out, int32_t v);

QUICKJS_EXPORT void js_value_load_int64(JSContext* ctx, JSValue* out, int64_t v);

QUICKJS_EXPORT void js_value_load_bool(JSContext* ctx, JSValue* out, int32_t v);

QUICKJS_EXPORT void js_value_load_ptr(JSContext* ctx, JSValue* out, const void* v);

QUICKJS_EXPORT void js_value_load_uint32(JSContext* ctx, JSValue* out, uint32_t v);

QUICKJS_EXPORT void js_value_load_uint64(JSContext* ctx, JSValue* out, uint64_t v);

QUICKJS_EXPORT void js_value_load_float(JSContext* ctx, JSValue* out, double v);

QUICKJS_EXPORT JSValue* js_value_new_int32(JSContext* ctx, int32_t v);

QUICKJS_EXPORT JSValue* js_value_new_int64(JSContext* ctx, int64_t v);

QUICKJS_EXPORT JSValue* js_value_new_bool(JSContext* ctx, int32_t v);

QUICKJS_EXPORT JSValue* js_value_new_ptr(JSContext* ctx, const void* ptr);

QUICKJS_EXPORT JSValue* js_value_new_uint32(JSContext* ctx, uint32_t v);

QUICKJS_EXPORT JSValue* js_value_new_uint64(JSContext* ctx, uint64_t v);

QUICKJS_EXPORT JSValue* js_value_new_float(JSContext* ctx, double v);

QUICKJS_EXPORT void js_value_set_property_func(JSContext* ctx, JSValue* obj, const char* name, JSCFunctionData* func, int argc, const void* data_ptr);

QUICKJS_EXPORT int js_value_get_type(JSValue* value);

QUICKJS_EXPORT int64_t js_value_get_int(JSValue* value);

QUICKJS_EXPORT double js_value_get_float(JSValue* value);

QUICKJS_EXPORT int32_t js_value_get_bool(JSValue* value);

QUICKJS_EXPORT const void* js_value_get_ptr(JSValue* value);

QUICKJS_EXPORT JSObject* js_value_get_obj_ref(JSValue* value);

QUICKJS_EXPORT const char* js_str_from_value(JSContext* ctx, JSValue* value);

QUICKJS_EXPORT void js_str_free(JSContext* ctx, const char* str);

QUICKJS_EXPORT JSValue* js_value_new_ref_globals(JSContext* ctx);

QUICKJS_EXPORT void js_value_load_ref_globals(JSContext* ctx, JSValue* out);

QUICKJS_EXPORT JSValue* js_value_new_obj(JSContext* ctx);

QUICKJS_EXPORT void js_obj_create(JSContext* ctx, JSValue* out);

QUICKJS_EXPORT void js_obj_set_property(JSContext* ctx, JSValue* obj, const char* name, JSValue* value);

QUICKJS_EXPORT JSValue* js_obj_get_property(JSContext* ctx, JSValue* obj, const char* name);

QUICKJS_EXPORT void js_obj_read_property(JSContext* ctx, JSValue* out, JSValue* obj, const char* name);

QUICKJS_EXPORT JSValue* js_value_new_str(JSContext* ctx, const char* str, size_t length);

QUICKJS_EXPORT JSValue* js_value_new_cstr(JSContext* ctx, const char* str);

QUICKJS_EXPORT void js_value_load_str(JSContext* ctx, JSValue* out, const char* str, size_t length);

QUICKJS_EXPORT void js_value_load_cstr(JSContext* ctx, JSValue* out, const char* str);

QUICKJS_EXPORT int js_eval(JSContext* ctx, JSValue* out, const char* input, size_t length, const char *filename, int eval_flags);

QUICKJS_EXPORT int js_eval_cstr(JSContext* ctx, JSValue* out, const char* input, const char *filename, int eval_flags);

QUICKJS_EXPORT int js_eval_this(JSContext* ctx, const JSValue* this_obj, JSValue* out, const char* input, size_t length, const char *filename, int eval_flags);

QUICKJS_EXPORT int js_eval_this_cstr(JSContext* ctx, const JSValue* this_obj, JSValue* out, const char* input, const char *filename, int eval_flags);

QUICKJS_EXPORT int js_json_parse_str(JSContext* ctx, JSValue* out, const char* input, size_t length);

QUICKJS_EXPORT int js_json_parse_cstr(JSContext* ctx, JSValue* out, const char* input);

QUICKJS_EXPORT const char* js_json_stringify(JSContext* ctx, JSValue* value);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* QUICKJS_EXTERN_H */
