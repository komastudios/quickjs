#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "quickjs-extern.h"

static const char* JS_VERSION = QUICKJS_VERSION;

const char* js_version(void)
{
    return JS_VERSION;
}

static inline JSValue* js_value_alloc(JSContext* ctx, JSValue v)
{
    JSValue* value;
    
    value = (JSValue*)js_malloc(ctx, sizeof(JSValue));
    if (value) {
        *value = v;
    }

    return value;
}

#define js_value_load(ctx, out, load_expr) \
    do { \
        if (ctx && out) { \
            JS_FreeValue(ctx, *out); \
            *out = (load_expr); \
        } \
    } while (0)

#define js_make_ptr(ptr) \
    JS_MKPTR(JS_TAG_INT, (ptr))


static inline void js_value_move(JSContext* ctx, JSValue* out, JSValue val)
{
    if (out) {
        JS_FreeValue(ctx, *out);
        *out = val;
    } else {
        JS_FreeValue(ctx, val);
    }
}

JSValue* js_value_new(JSContext* ctx)
{
    return js_value_alloc(ctx, JS_UNDEFINED);
}

void js_value_free(JSContext* ctx, JSValue* value)
{
    if (value) {
        JS_FreeValue(ctx, *value);

        js_free(ctx, value);
    }
}

void js_value_load_int32(JSContext* ctx, JSValue* out, int32_t v)
{
    js_value_load(ctx, out, JS_NewInt32(ctx, v));
}

void js_value_load_int64(JSContext* ctx, JSValue* out, int64_t v)
{
    js_value_load(ctx, out, JS_NewInt64(ctx, v));
}

void js_value_load_bool(JSContext* ctx, JSValue* out, int32_t v)
{
    js_value_load(ctx, out, JS_NewBool(ctx, v));
}

void js_value_load_ptr(JSContext* ctx, JSValue* out, const void* v)
{
    js_value_load(ctx, out, js_make_ptr(v));
}

void js_value_load_uint32(JSContext* ctx, JSValue* out, uint32_t v)
{
    js_value_load(ctx, out, JS_NewUint32(ctx, v));
}

void js_value_load_uint64(JSContext* ctx, JSValue* out, uint64_t v)
{
    js_value_load(ctx, out, JS_NewInt64(ctx, (int64_t)v));
}

void js_value_load_float(JSContext* ctx, JSValue* out, double v)
{
    js_value_load(ctx, out, JS_NewFloat64(ctx, v));
}

JSValue* js_value_new_int32(JSContext* ctx, int32_t v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_int32(ctx, result, v);
    return result;
}

JSValue* js_value_new_int64(JSContext* ctx, int64_t v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_int64(ctx, result, v);
    return result;
}

JSValue* js_value_new_bool(JSContext* ctx, int32_t v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_bool(ctx, result, v);
    return result;
}

JSValue* js_value_new_ptr(JSContext* ctx, const void* ptr)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_ptr(ctx, result, ptr);
    return result;
}

JSValue* js_value_new_uint32(JSContext* ctx, uint32_t v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_uint32(ctx, result, v);
    return result;
}

JSValue* js_value_new_uint64(JSContext* ctx, uint64_t v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_uint64(ctx, result, v);
    return result;
}

JSValue* js_value_new_float(JSContext* ctx, double v)
{
    JSValue* result = js_value_new(ctx);
    js_value_load_float(ctx, result, v);
    return result;
}

void js_value_set_property_func(JSContext* ctx, JSValue* obj, const char* name, JSCFunctionData* func, int argc, const void* data_ptr)
{
    if (!ctx || !obj || JS_VALUE_GET_TAG(*obj) != JS_TAG_OBJECT || !name || !func) {
        return;
    }

    JSValue data = js_make_ptr(data_ptr);
    JS_SetPropertyStr(ctx, *obj, name,
                      JS_NewCFunctionData(ctx, func, argc, 0, 1, &data));
}

int js_value_get_type(JSValue* value)
{
    int result = JS_VALUE_TYPE_UNINITIALIZED;
    if (value) {
        switch (JS_VALUE_GET_TAG(*value)) {
            case JS_TAG_UNINITIALIZED:
                result = JS_VALUE_TYPE_UNINITIALIZED;
                break;
            case JS_TAG_INT:
                result = JS_VALUE_TYPE_INT;
                break;
            case JS_TAG_BOOL:
                result = JS_VALUE_TYPE_BOOL;
                break;
            case JS_TAG_NULL:
                result = JS_VALUE_TYPE_NULL;
                break;
            case JS_TAG_UNDEFINED:
                result = JS_VALUE_TYPE_UNDEFINED;
                break;
            case JS_TAG_CATCH_OFFSET:
                result = JS_VALUE_TYPE_CATCH_OFFSET;
                break;
            case JS_TAG_EXCEPTION:
                result = JS_VALUE_TYPE_EXCEPTION;
                break;
            case JS_TAG_FLOAT64:
                result = JS_VALUE_TYPE_FLOAT64;
                break;
            case JS_TAG_OBJECT:
                result = JS_VALUE_TYPE_OBJECT;
                break;
            case JS_TAG_FUNCTION_BYTECODE:
                result = JS_VALUE_TYPE_FUNCTION_BYTECODE;
                break;
            case JS_TAG_MODULE:
                result = JS_VALUE_TYPE_MODULE;
                break;
            case JS_TAG_STRING:
                result = JS_VALUE_TYPE_STRING;
                break;
            case JS_TAG_SYMBOL:
                result = JS_VALUE_TYPE_SYMBOL;
                break;
            case JS_TAG_BIG_FLOAT:
                result = JS_VALUE_TYPE_BIG_FLOAT;
                break;
            case JS_TAG_BIG_INT:
                result = JS_VALUE_TYPE_BIG_INT;
                break;
            case JS_TAG_BIG_DECIMAL:
                result = JS_VALUE_TYPE_BIG_DECIMAL;
                break;
            default:
                result = JS_VALUE_TYPE_FLOAT64;
                break;
        }
    }
    return result;
    
    //return value ? JS_VALUE_GET_TAG(*value) : JS_VALUE_TYPE_UNINITIALIZED;
}

int64_t js_value_get_int(JSValue* value)
{
    int64_t result = 0;
    if (value) {
        uint32_t tag = JS_VALUE_GET_NORM_TAG(*value);
        double d;
        switch(tag) {
            case JS_TAG_INT:
            case JS_TAG_BOOL:
                result = JS_VALUE_GET_INT(*value);
                break;
            case JS_TAG_FLOAT64:
                d = JS_VALUE_GET_FLOAT64(*value);
                if (!isnan(d)) {
                    if (d < (double)INT64_MIN) {
                        result = INT64_MIN;
                    }
                    else if (d > (double)INT64_MAX) {
                        result = INT64_MAX;
                    }
                    else {
                        result = (int64_t)d;
                    }
                }
                break;
            default:
                break;
        }
    }
    return result;
}

double js_value_get_float(JSValue* value)
{
    double result = 0.0;
    if (value) {
        uint32_t tag = JS_VALUE_GET_NORM_TAG(*value);
        switch(tag) {
            case JS_TAG_INT:
            case JS_TAG_BOOL:
                result = (double)JS_VALUE_GET_INT(*value);
                break;
            case JS_TAG_FLOAT64:
                result = JS_VALUE_GET_FLOAT64(*value);
                break;
            default:
                break;
        }
    }
    return result;
}

int32_t js_value_get_bool(JSValue* value)
{
    return js_value_get_int(value) != 0;
}

const void* js_value_get_ptr(JSValue* value)
{
    return value ? JS_VALUE_GET_PTR(*value) : NULL;
}

JSObject* js_value_get_obj_ref(JSValue* value)
{
    return value && JS_VALUE_GET_TAG(*value) == JS_TAG_OBJECT ? JS_VALUE_GET_OBJ(*value) : NULL;
}

int js_value_is_error(JSContext* ctx, JSValue* value)
{
    return ctx && value ? JS_IsError(ctx, *value) : 0;
}

const char* js_str_from_value(JSContext* ctx, JSValue* value)
{
    const char* result;

    if (ctx && value) {
        result = JS_ToCString(ctx, *value);
    } else {
        result = NULL;
    }

    return result;
}

void js_str_free(JSContext* ctx, const char* str)
{
    if (ctx && str) {
        JS_FreeCString(ctx, str);
    }
}

JSValue* js_value_new_ref_globals(JSContext* ctx)
{
    JSValue* result = js_value_alloc(ctx, JS_GetGlobalObject(ctx));
    return result;
}

void js_value_load_ref_globals(JSContext* ctx, JSValue* out)
{
    js_value_load(ctx, out, JS_GetGlobalObject(ctx));
}

JSValue* js_value_new_obj(JSContext* ctx)
{
    if (!ctx) {
        return NULL;
    }

    JSValue* result = js_value_alloc(ctx, JS_NewObject(ctx));
    return result;
}

void js_obj_create(JSContext* ctx, JSValue* out)
{
    js_value_load(ctx, out, JS_NewObject(ctx));
}

void js_obj_set_property(JSContext* ctx, JSValue* obj, const char* name, JSValue* value)
{
    if (!ctx || !obj || JS_VALUE_GET_TAG(*obj) != JS_TAG_OBJECT || !name || !value) {
        return;
    }

    JS_SetPropertyStr(ctx, *obj, name, JS_DupValue(ctx, *value));
}

JSValue* js_obj_get_property(JSContext* ctx, JSValue* obj, const char* name)
{
    JSValue* result = js_value_new(ctx);
    js_obj_read_property(ctx, result, obj, name);
    return result;
}

void js_obj_read_property(JSContext* ctx, JSValue* out, JSValue* obj, const char* name)
{
    if (ctx && out && obj && JS_VALUE_GET_TAG(*obj) == JS_TAG_OBJECT && name) {
        js_value_move(ctx, out, JS_GetPropertyStr(ctx, *obj, name));
    }
}

JSValue* js_value_new_str(JSContext* ctx, const char* str, size_t length)
{
    JSValue* result = NULL;
    if (ctx) {
        result = js_value_alloc(ctx, JS_NewStringLen(ctx, str, length));
    }
    return result;
}

JSValue* js_value_new_cstr(JSContext* ctx, const char* str)
{
    return js_value_new_str(ctx, str, str ? strlen(str) : 0);
}

void js_value_load_str(JSContext* ctx, JSValue* out, const char* str, size_t length)
{
    js_value_load(ctx, out, JS_NewStringLen(ctx, str, length));
}

void js_value_load_cstr(JSContext* ctx, JSValue* out, const char* str)
{
    js_value_load_str(ctx, out, str, str ? strlen(str) : 0);
}

int js_eval(JSContext* ctx, JSValue* out, const char* input, size_t length, const char *filename, int eval_flags)
{
    int result = JS_OK;
    JSValue val;
    
    val = JS_Eval(ctx, input, length, filename, eval_flags);
    if (JS_IsException(val)) {
        result = JS_ERR_EXCEPTION;
        val = JS_GetException(ctx);
    }

    js_value_move(ctx, out, val);
    return result;
}

int js_eval_cstr(JSContext* ctx, JSValue* out, const char* input, const char *filename, int eval_flags)
{
    size_t length;
    length = input ? strlen(input) : 0;

    return js_eval(ctx, out, input, length, filename, eval_flags);
}

int js_eval_this(JSContext* ctx, const JSValue* this_obj, JSValue* out, const char* input, size_t length, const char *filename, int eval_flags)
{
    int result = JS_OK;
    JSValue val;

    val = JS_EvalThis(ctx, this_obj ? *this_obj : JS_UNDEFINED, input, length, filename, eval_flags);
    if (JS_IsException(val)) {
        result = JS_ERR_EXCEPTION;
        val = JS_GetException(ctx);
    }

    js_value_move(ctx, out, val);
    return result;
}

int js_eval_this_cstr(JSContext* ctx, const JSValue* this_obj, JSValue* out, const char* input, const char *filename, int eval_flags)
{
    size_t length;
    length = input ? strlen(input) : 0;

    return js_eval_this(ctx, this_obj, out, input, length, filename, eval_flags);
}

int js_json_parse_str(JSContext* ctx, JSValue* out, const char* input, size_t length)
{
    int result = JS_OK;
    JSValue val;
    
    val = JS_ParseJSON(ctx, input, length, "<input>");
    if (JS_IsException(val)) {
        result = JS_ERR_EXCEPTION;
        val = JS_GetException(ctx);
    }
    
    js_value_move(ctx, out, val);
    return result;
}

int js_json_parse_cstr(JSContext* ctx, JSValue* out, const char* input)
{
    size_t length;
    length = input ? strlen(input) : 0;

    return js_json_parse_str(ctx, out, input, length);
}

const char* js_json_stringify(JSContext* ctx, JSValue* value)
{
    const char* result = NULL;
    JSValue val, space0;
    
    val = JS_JSONStringify(ctx, value ? *value : JS_UNDEFINED, JS_UNDEFINED, JS_UNDEFINED);

    if (!JS_IsException(val)) {
        result = JS_ToCString(ctx, val);
        JS_FreeValue(ctx, val);
    }

    return result;
}
