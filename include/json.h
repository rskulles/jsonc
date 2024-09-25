//
// Created by rskul on 9/24/2024.
//

#ifndef JSON_H
#define JSON_H

enum JSON_ValueType {
    NUMBER,
    STRING,
    OBJECT,
    JSON_NULL,
    TRUE,
    FALSE,
    ARRAY,
};


struct JSON_ValueArray {
    void *array;
    unsigned int size;
    enum JSON_ValueType array_type;
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
    struct JSON_Member *members;
    unsigned int size ;
};

struct JSON_Object* JSON_Parse(const char *str, unsigned int len);
void  JSON_Print(const struct JSON_Object * object);

#endif //JSON_H
