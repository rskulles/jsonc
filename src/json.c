//
// Created by rskul on 9/24/2024.
//
#include<json.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#ifdef _cplusplus
extern "C" {
#endif
#define JSON_OBJECT_START                  '{'
#define JSON_OBJECT_END                    '}'
#define JSON_ARRAY_START                   '['
#define JSON_ARRAY_END                     ']'
#define JSON_STRING_DELIMITER              '"'
#define JSON_MEMBER_NAME_VALUE_SEPARATOR   ':'
#define JSON_MEMBER_SEPARATOR              ','
#define JSON_WHITESPACE_SPACE              '\x20'
#define JSON_WHITESPACE_LINE_FEED          '\x0A'
#define JSON_WHITESPACE_CARRIAGE_RETURN    '\x0D'
#define JSON_WHITESPACE_TAB                '\x09'
#define JSON_STRING_ESCAPE_CHARACTER       '\\'


size_t json_string_read_pos;

int IsWhitespace(const char c) {
    return c == JSON_WHITESPACE_SPACE || c == JSON_WHITESPACE_LINE_FEED ||
           c == JSON_WHITESPACE_CARRIAGE_RETURN || c == JSON_WHITESPACE_TAB;
}

int IsDigitOrSign(const char c) {
    return c == '0' ||
           c == '1' ||
           c == '2' ||
           c == '3' ||
           c == '4' ||
           c == '5' ||
           c == '6' ||
           c == '7' ||
           c == '8' ||
           c == '9' ||
           c == '-' ||
           c == '+';
}

void print_padding(const size_t padding) {
    for (size_t i = 0; i < padding; i++) {
        printf(" ");
    }
}

void JSON_Print(const struct JSON_Object *object, const size_t pad) {
    for (unsigned int i = 0; i < object->size; ++i) {
        print_padding(pad);
        printf("NAME: %s\n", object->members[i].name);
        print_padding(pad);
        printf("VALUE: ");
        switch (object->members[i].value.type) {
            case NUMBER:
                printf("%f\n", object->members[i].value.number);
                break;
            case STRING:
                printf("%s\n", object->members[i].value.string);
                break;
            case OBJECT:
                wprintf(L"OBJECT\xAC\n");
                JSON_Print(object->members[i].value.object, pad + 2);
                break;
            case JSON_NULL:
                printf("%s\n", "NULL");
                break;
            case TRUE:
                printf("%s\n", "TRUE");
                break;
            case FALSE:
                printf("%s\n", "FALSE");
                break;
            case ARRAY:
                //TODO
                break;
            default:
                break;
        }
    }
}

char Advance(const char *str) {
    return str[json_string_read_pos++];
}

void Seek(const size_t offset) {
    json_string_read_pos += offset;
}

char PeekNext(const char *str, const size_t len) {
    if (json_string_read_pos < (len - 1)) {
        return str[json_string_read_pos + 1];
    }
    return '\0';
}

char PeekPrevious(const char *str) {
    if (json_string_read_pos == 0) return '\0';
    return str[json_string_read_pos - 1];
}

enum ParseState {
    ParseState_None,
    FINDING_MEMBER_NAME,
    READING_MEMBER_NAME,
    FINDING_VALUE,
    READING_VALUE,
    FINDING_ARRAY_VALUE,
    READING_ARRAY_VALUE,
    READ_UNTIL_MEMBER_SEPARATOR,
    READ_UNTIL_MEMBER_SEPARATOR_ARRAY
};

struct JSON_Object *JSON_Parse_Object(const char *str, const unsigned int len) {
    struct JSON_Object *obj = malloc(sizeof(struct JSON_Object));
    obj->size = 0;
    obj->members = NULL;
    unsigned int character_buff_pos_begin = 0;
    enum JSON_ValueType value_type = JSON_ValueType_None;
    enum ParseState state = ParseState_None;

    while (json_string_read_pos < len) {
        const size_t i = json_string_read_pos;
        const char current_char = Advance(str);

        switch (current_char) {
            case JSON_OBJECT_START: {
                if (state == FINDING_VALUE) {
                    Seek(-1);
                    obj->members[obj->size - 1].value.object = JSON_Parse_Object(str, len);
                    obj->members[obj->size - 1].value.type = OBJECT;
                } else if (state == FINDING_ARRAY_VALUE) {
                    Seek(-1);
                    //extend the value array and add the object to it.
                } else if (state == ParseState_None) {
                    state = FINDING_MEMBER_NAME;
                }

                break;
            }
            case JSON_OBJECT_END: {
                if (state == READING_VALUE && value_type == NUMBER) {
                    const size_t num_length = i - character_buff_pos_begin;
                    char *num_str = malloc((num_length + 1) * sizeof(char));
                    memcpy_s(num_str, num_length, str + character_buff_pos_begin, num_length);
                    num_str[num_length] = '\0';
                    const double number = strtod(num_str, NULL);
                    obj->members[obj->size - 1].value.number = number;
                    free(num_str);
                }

                return obj;
            }
            case JSON_ARRAY_START:
                if (state == FINDING_VALUE) {
                    state = FINDING_ARRAY_VALUE;
                    obj->members[obj->size - 1].value.type = ARRAY;
                } else if (state == FINDING_ARRAY_VALUE) {
                    //TODO Support Nested Arrays
                }
                break;
            case JSON_ARRAY_END:
                if (state == READING_ARRAY_VALUE || state == FINDING_ARRAY_VALUE) {
                    state = FINDING_MEMBER_NAME;
                    if(value_type==NUMBER) {

                    }
                }
                break;
            case JSON_STRING_DELIMITER:
                if (state == FINDING_MEMBER_NAME) {
                    obj->size++;

                    if (obj->size == 1) {
                        obj->members = malloc(sizeof(struct JSON_Member) * obj->size);
                    } else {
                        struct JSON_Member *members = malloc(sizeof(struct JSON_Member) * obj->size);
                        memcpy(members, obj->members, sizeof(struct JSON_Member) * (obj->size - 1));
                        free(obj->members);
                        obj->members = members;
                    }

                    character_buff_pos_begin = i + 1;
                    state = READING_MEMBER_NAME;
                } else if (state == READING_MEMBER_NAME) {
                    const size_t name_size = i - character_buff_pos_begin;
                    obj->members[obj->size - 1].name = malloc(sizeof(char) * name_size + 1);
                    memcpy_s(obj->members[obj->size - 1].name, name_size, str + character_buff_pos_begin,
                             name_size);
                    obj->members[obj->size - 1].name[name_size] = '\0';
                    state = FINDING_VALUE;
                } else if (state == FINDING_VALUE) {
                    state = READING_VALUE;
                    value_type = STRING;
                    obj->members[obj->size - 1].value.type = STRING;
                    character_buff_pos_begin = i + 1;
                } else if (state == READING_VALUE && value_type == STRING && str[i - 1] !=
                           JSON_STRING_ESCAPE_CHARACTER) {
                    state = FINDING_MEMBER_NAME;
                    value_type = JSON_ValueType_None;

                    const size_t value_size = i - character_buff_pos_begin;
                    obj->members[obj->size - 1].value.string = malloc(sizeof(char) * value_size + 1);
                    memcpy_s(obj->members[obj->size - 1].value.string, value_size, str + character_buff_pos_begin,
                             value_size);
                    obj->members[obj->size - 1].value.string[value_size] = '\0';
                } else if (state == FINDING_ARRAY_VALUE) {
                    state = READING_ARRAY_VALUE;
                    value_type = STRING;
                    // TODO: extend the value array and set the new member's value type to STRING
                } else if (state == READING_ARRAY_VALUE) {
                }
                break;
            case JSON_MEMBER_NAME_VALUE_SEPARATOR:
                if (state != FINDING_VALUE) {
                    state = FINDING_VALUE;
                }
                break;
            case JSON_MEMBER_SEPARATOR: {
                if (state == READING_VALUE) {
                    if (value_type == NUMBER) {
                        state = FINDING_MEMBER_NAME;
                        value_type = JSON_ValueType_None;
                        const double number = strtod(str + character_buff_pos_begin, NULL);
                        obj->members[obj->size - 1].value.number = number;
                    }
                } else if (state == READING_ARRAY_VALUE ||
                           state == READ_UNTIL_MEMBER_SEPARATOR_ARRAY) {
                    if (value_type == NUMBER) {
                        state = FINDING_ARRAY_VALUE;
                        value_type = JSON_ValueType_None;
                        const double number = strtod(str + character_buff_pos_begin, NULL);
                        const size_t array_size = obj->members[obj->size - 1].value.array.size;
                        obj->members[obj->size - 1].value.array.members[array_size-1].number= number;
                    }
                } else if (state == READ_UNTIL_MEMBER_SEPARATOR) {
                    state = FINDING_MEMBER_NAME;
                }
                break;
            }
            default: {
                if (IsDigitOrSign(str[i])) {
                    if (state == FINDING_VALUE) {
                        state = READING_VALUE;
                        value_type = NUMBER;
                        character_buff_pos_begin = i;
                        obj->members[obj->size - 1].value.type = NUMBER;
                    } else if (state == FINDING_ARRAY_VALUE) {
                        state = READING_ARRAY_VALUE;
                        value_type = NUMBER;
                        character_buff_pos_begin = i;
                        // TODO: Extend the array and set the value type of the new element
                    }
                }
                if (IsWhitespace(current_char)) {
                    if (state == READING_VALUE && value_type == NUMBER) {
                        state = FINDING_MEMBER_NAME;
                        value_type = JSON_ValueType_None;
                        const size_t num_length = i - character_buff_pos_begin;
                        char *num_str = malloc((num_length + 1) * sizeof(char));
                        memcpy_s(num_str, num_length, str + character_buff_pos_begin, num_length);
                        num_str[num_length] = '\0';
                        const double number = strtod(num_str, NULL);
                        obj->members[obj->size - 1].value.number = number;
                        free(num_str);
                    }
                } else {
                    if (state == FINDING_VALUE ||
                        state == FINDING_ARRAY_VALUE) {
                        //TODO add to the array if state is finding array value
                        switch (current_char) {
                            case 'n':
                                obj->members[obj->size - 1].value.type = JSON_NULL;
                                state = state == FINDING_VALUE
                                            ? READ_UNTIL_MEMBER_SEPARATOR
                                            : READ_UNTIL_MEMBER_SEPARATOR_ARRAY;
                                break;
                            case 't':
                                obj->members[obj->size - 1].value.type = TRUE;
                                state = state == FINDING_VALUE
                                            ? READ_UNTIL_MEMBER_SEPARATOR
                                            : READ_UNTIL_MEMBER_SEPARATOR_ARRAY;
                                break;
                            case 'f':
                                obj->members[obj->size - 1].value.type = FALSE;
                                state = state == FINDING_VALUE
                                            ? READ_UNTIL_MEMBER_SEPARATOR
                                            : READ_UNTIL_MEMBER_SEPARATOR_ARRAY;
                                break;
                            default:
                                break;
                        }
                    }
                }
                break;
            }
        }
    }
    return obj;
}

struct JSON_Object *JSON_Parse(const char *str, const size_t len) {
    if (len == 0) { return NULL; }
    json_string_read_pos = 0;
    struct JSON_Object *root = JSON_Parse_Object(str, len);
    return root;
}

void JSON_Free(struct JSON_Object *object) {
    free(object);
}
#ifdef _cplusplus
}
#endif
