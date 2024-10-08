//
// Created by rskul on 9/24/2024.
//

#ifndef JSON_H
#define JSON_H

#include <stddef.h>


#ifdef _cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define JSONC_API __declspec(dllexport)
#else
#define JSONC_API
#endif

enum JSON_ValueType {
    JSON_ValueType_None,
    NUMBER,
    STRING,
    OBJECT,
    JSON_NULL,
    TRUE,
    FALSE,
    ARRAY,
};

struct JSON_ValueArray {
    struct JSON_Value *members;
    unsigned int size;
};

struct JSON_Value {
    enum JSON_ValueType type;

    union {
        double number;
        struct JSON_Object *object;
        struct JSON_ValueArray array;
        char *string;
    };
};

struct JSON_Member {
    char *name;
    struct JSON_Value value;
};

struct JSON_Object {
    unsigned int size;
    struct JSON_Member *members;
};

JSONC_API struct JSON_Object *JSON_Parse(const char *str, size_t len);
JSONC_API void JSON_Print(const struct JSON_Object *object, size_t pad);
JSONC_API void JSON_Free(struct JSON_Object *object);
#ifdef _cplusplus
}
#endif

#endif //JSON_H
