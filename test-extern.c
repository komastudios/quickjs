#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quickjs-extern.h"

#define ASSERT(expr, msg) do { \
    if(!(expr)) { \
        fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__, msg); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define ASSERT_EQ(val1, val2) do { \
    if((val1) != (val2)) { \
        fprintf(stderr, "%s:%d: Expected: %s == %s\n", \
                __FILE__, __LINE__, #val1, #val2); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define ASSERT_STREQ(str1, str2) do { \
    if((!(str1) || !(str2)) || strcmp((str1), (str2)) != 0) { \
        fprintf(stderr, "%s:%d: Expected: %s == %s\n", \
                __FILE__, __LINE__, #str1, #str2); \
        exit(EXIT_FAILURE); \
    } \
} while(0)

#define JS_LOAD_JSON(ctx, out, json) \
    ASSERT(js_json_parse_cstr(ctx, out, json) == JS_OK, "Failed to parse JSON.") \

void test_value_types(JSContext* ctx) {
    ASSERT_EQ(js_value_get_type(NULL), JS_VALUE_TYPE_UNINITIALIZED);

    JSValue* value = js_value_new(ctx);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_UNDEFINED);
    js_value_free(ctx, value);

    const void* fnPtr = (const void*)&test_value_types;

    // js_value_new_*
    value = js_value_new_uint32(ctx, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.0);
    js_value_free(ctx, value);

    value = js_value_new_uint64(ctx, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.0);
    js_value_free(ctx, value);

    value = js_value_new_int32(ctx, -42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), -42);
    ASSERT_EQ(js_value_get_float(value), -42.0);
    js_value_free(ctx, value);

    value = js_value_new_int64(ctx, -42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), -42);
    ASSERT_EQ(js_value_get_float(value), -42.0);
    js_value_free(ctx, value);

    value = js_value_new_bool(ctx, 0);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_BOOL);
    ASSERT_EQ(js_value_get_int(value), 0);
    ASSERT_EQ(js_value_get_float(value), 0.0);
    ASSERT_EQ(js_value_get_bool(value), 0);
    js_value_free(ctx, value);

    value = js_value_new_bool(ctx, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_BOOL);
    ASSERT_EQ(js_value_get_int(value), 1);
    ASSERT_EQ(js_value_get_float(value), 1);
    ASSERT_EQ(js_value_get_bool(value), 1);
    js_value_free(ctx, value);

    value = js_value_new_ptr(ctx, fnPtr);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_ptr(value), fnPtr);
    js_value_free(ctx, value);

    value = js_value_new_int64(ctx, (int64_t)INT32_MAX + 1ll);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_FLOAT64);
    ASSERT_EQ(js_value_get_int(value), (int64_t)INT32_MAX + 1ll);
    ASSERT_EQ(js_value_get_float(value), (double)((int64_t)INT32_MAX + 1ll));
    js_value_free(ctx, value);

    value = js_value_new_float(ctx, 42.5);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_FLOAT64);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.5);
    js_value_free(ctx, value);

    // js_value_load_*
    value = js_value_new(ctx);
    js_value_load_uint32(ctx, value, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.0);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_uint64(ctx, value, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.0);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_int32(ctx, value, -42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), -42);
    ASSERT_EQ(js_value_get_float(value), -42.0);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_int64(ctx, value, -42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_int(value), -42);
    ASSERT_EQ(js_value_get_float(value), -42.0);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_bool(ctx, value, 0);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_BOOL);
    ASSERT_EQ(js_value_get_int(value), 0);
    ASSERT_EQ(js_value_get_float(value), 0.0);
    ASSERT_EQ(js_value_get_bool(value), 0);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_bool(ctx, value, 42);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_BOOL);
    ASSERT_EQ(js_value_get_int(value), 1);
    ASSERT_EQ(js_value_get_float(value), 1);
    ASSERT_EQ(js_value_get_bool(value), 1);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_ptr(ctx, value, fnPtr);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    ASSERT_EQ(js_value_get_ptr(value), fnPtr);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_int64(ctx, value, (int64_t)INT32_MAX + 1ll);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_FLOAT64);
    ASSERT_EQ(js_value_get_int(value), (int64_t)INT32_MAX + 1ll);
    ASSERT_EQ(js_value_get_float(value), (double)((int64_t)INT32_MAX + 1ll));
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_float(ctx, value, 42.5);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_FLOAT64);
    ASSERT_EQ(js_value_get_int(value), 42);
    ASSERT_EQ(js_value_get_float(value), 42.5);
    js_value_free(ctx, value);
}

void test_string(JSContext* ctx) {
    JSValue* value = js_value_new(ctx);
    const char* str;
    ASSERT(value, "Failed to create JSValue.");

    str = js_str_from_value(ctx, value);
    ASSERT(str, "Failed to create string from JSValue.");
    ASSERT_STREQ(str, "undefined");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    const char* testStr = "Hello";
    size_t testStrLen = strlen(testStr);

    value = js_value_new_str(ctx, testStr, testStrLen);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new_cstr(ctx, "Hello, World!");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "Hello, World!");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_str(ctx, value, testStr, testStrLen);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_cstr(ctx, value, "Hello, World!");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "Hello, World!");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    // empty string
    value = js_value_new_str(ctx, NULL, 0);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new_cstr(ctx, "");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_str(ctx, value, NULL, 0);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_cstr(ctx, value, "");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "");
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    // test unicode
    testStr = "M\xc3\xabt\xc3\xa0l H\xc3\xab\xc3\xa0""d";
    testStrLen = strlen(testStr);

    value = js_value_new_str(ctx, testStr, testStrLen);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new_cstr(ctx, testStr);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_str(ctx, value, testStr, testStrLen);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);

    value = js_value_new(ctx);
    js_value_load_cstr(ctx, value, testStr);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, testStr);
    js_str_free(ctx, str);
    js_value_free(ctx, value);
}

void test_eval(JSContext* ctx) {
    JSValue* value = js_value_new(ctx);
    ASSERT(value, "Failed to create JSValue.");

    const char *evalStr = "40 + 2";
    int eval_flags = JS_EVAL_TYPE_GLOBAL;  // Use the correct flag based on your needs
    const char *filename = "<eval>";
    int err = js_eval(ctx, value, evalStr, strlen(evalStr), filename, eval_flags);
    ASSERT(err == 0, "Error in evaluation");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);

    const char* str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "42");
    js_str_free(ctx, str);

    /* Test js_eval_cstr */
    err = js_eval_cstr(ctx, value, "40.5 + 2", filename, eval_flags);
    ASSERT(err == 0, "Error in evaluation");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_FLOAT64);
    
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "42.5");
    js_str_free(ctx, str);

    /* Test js_eval_cstr throwing */
    err = js_eval_cstr(ctx, value, "throw 'error'", filename, eval_flags);
    ASSERT_EQ(err, JS_ERR_EXCEPTION);
    
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "error");
    js_str_free(ctx, str);

    js_value_free(ctx, value);
}

void test_globals(JSContext* ctx) {
    JSValue *value = js_value_new(ctx);
    JSValue *globals = js_value_new_ref_globals(ctx);
    ASSERT_EQ(js_value_get_type(globals), JS_VALUE_TYPE_OBJECT);
    ASSERT(js_value_get_obj_ref(globals), "Failed to get global object reference");

    js_value_load_ref_globals(ctx, value);
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_OBJECT);
    ASSERT_EQ(js_value_get_obj_ref(value), js_value_get_obj_ref(globals));

    js_value_free(ctx, globals);
    js_value_free(ctx, value);
}

void test_objects(JSContext* ctx) {
    JSValue *value = js_value_new(ctx);
    JSValue *obj;
    JSValue *temp;
    const char *str;

    obj = js_value_new_obj(ctx);
    ASSERT_EQ(js_value_get_type(obj), JS_VALUE_TYPE_OBJECT);
    ASSERT(js_value_get_obj_ref(obj), "Failed to get object reference");
    js_value_free(ctx, obj);

    obj = js_value_new(ctx);
    ASSERT_EQ(js_value_get_type(obj), JS_VALUE_TYPE_UNDEFINED);
    js_obj_create(ctx, obj);
    ASSERT_EQ(js_value_get_type(obj), JS_VALUE_TYPE_OBJECT);
    ASSERT(js_value_get_obj_ref(obj), "Failed to get object reference");
    js_value_free(ctx, obj);

    obj = js_value_new_obj(ctx);
    js_value_load_cstr(ctx, value, "Hello");
    js_obj_set_property(ctx, obj, "a", value);
    js_value_load_int32(ctx, value, 2);
    js_obj_set_property(ctx, obj, "b", value);

    // read property a
    js_obj_read_property(ctx, value, obj, "a");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_STRING);
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "Hello");
    js_str_free(ctx, str);

    // read property b
    js_obj_read_property(ctx, value, obj, "b");
    ASSERT_EQ(js_value_get_int(value), 2);

    // get property b
    temp = js_obj_get_property(ctx, obj, "b");
    ASSERT(temp, "Failed to get property b");
    ASSERT_EQ(js_value_get_int(temp), 2);
    js_value_free(ctx, temp);

    js_value_free(ctx, obj);

    js_value_free(ctx, value);
}

void test_json(JSContext* ctx) {
    JSValue* value = js_value_new(ctx);
    int err;

    const char *jsonOut;
    const char *jsonIn = "{\"a\":\"Hello\",\"b\":2}";

    err = js_json_parse_str(ctx, value, jsonIn, strlen(jsonIn));
    ASSERT_EQ(err, JS_OK);
    jsonOut = js_json_stringify(ctx, value);
    ASSERT_STREQ(jsonOut, jsonIn);
    js_str_free(ctx, jsonOut);

    err = js_json_parse_cstr(ctx, value, jsonIn);
    ASSERT_EQ(err, JS_OK);
    jsonOut = js_json_stringify(ctx, value);
    ASSERT_STREQ(jsonOut, jsonIn);
    js_str_free(ctx, jsonOut);

    JS_LOAD_JSON(ctx, value, jsonIn);
    jsonOut = js_json_stringify(ctx, value);
    ASSERT_STREQ(jsonOut, jsonIn);
    js_str_free(ctx, jsonOut);

    js_value_free(ctx, value);
}

void test_eval_ctx(JSContext* ctx)
{
    JSValue* value = js_value_new(ctx);
    ASSERT(value, "Failed to create JSValue.");

    JSValue* self = js_value_new(ctx);
    ASSERT(self, "Failed to create JSValue.");
    JS_LOAD_JSON(ctx, self, "{\"a\":\"Hello\",\"b\":2}");

    const char *evalStr = "this.b";
    int err = js_eval_this(ctx, self, value, evalStr, strlen(evalStr), "<eval>", JS_EVAL_TYPE_GLOBAL);
    ASSERT(err == 0, "Error in evaluation");
    ASSERT_EQ(js_value_get_type(value), JS_VALUE_TYPE_INT);
    const char* str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "2");
    js_str_free(ctx, str);

    err = js_eval_this_cstr(ctx, self, value, "this.a", "<eval>", JS_EVAL_TYPE_GLOBAL);
    ASSERT(err == 0, "Error in evaluation");
    str = js_str_from_value(ctx, value);
    ASSERT_STREQ(str, "Hello");
    js_str_free(ctx, str);

    js_value_free(ctx, self);
    js_value_free(ctx, value);
}

static int test_counter = 0;

static JSValue inc_counter(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data)
{
    printf("inc_counter called\n");

    void* userPtr = JS_VALUE_GET_PTR(func_data[0]);

    int* counter = (int*)userPtr;
    ++(*counter);

    return JS_UNDEFINED;
}

void test_cfunction(JSContext* ctx)
{
    JSValue* globals = js_value_new_ref_globals(ctx);
    // JSValue* user_ptr = js_value_new_ptr(ctx, &test_counter);

    int counter = test_counter;

//    JS_SetPropertyStr(ctx, globals, "inc_counter",
//        JS_NewCFunctionData(ctx, inc_counter, 0, 0, 1, user_ptr));
    js_value_set_property_func(ctx, globals, "inc_counter", inc_counter, 0, &test_counter);

    ASSERT_EQ(test_counter, counter);

    int err = js_eval_cstr(ctx, NULL, "inc_counter()", "<eval>", JS_EVAL_TYPE_GLOBAL);
    ASSERT(err == 0, "Error in evaluation");

    ASSERT_EQ(test_counter, counter+1);

    js_value_free(ctx, globals);
}

int main(int argc, char** argv) {
    fprintf(stderr, "Running tests...\n");
    fprintf(stderr, "QuickJS version: %s\n", js_version());

    /* Initialize the JS runtime and context */
    JSRuntime *rt = JS_NewRuntime();
    ASSERT(rt, "Failed to create JSRuntime.");

    JSContext *ctx = JS_NewContext(rt);
    ASSERT(ctx, "Failed to create JSContext.");

    test_value_types(ctx);
    test_string(ctx);
    test_eval(ctx);
    test_objects(ctx);
    test_globals(ctx);
    test_eval_ctx(ctx);
    test_cfunction(ctx);
    test_json(ctx);

    /* Clean up */
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    fprintf(stderr, "All tests passed.\n");

    return 0;
}
