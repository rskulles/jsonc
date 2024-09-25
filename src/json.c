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
        }
    }
}

char Advance(const char *str) {
    return str[json_string_read_pos++];
}

void Seek(const size_t offset) {
    json_string_read_pos += offset;
}

char PeekNext(const char *str, size_t len) {
    if (json_string_read_pos < len) {
        return str[json_string_read_pos];
    }
    return '\0';
}

char PeekPrevious(const char *str) {
    if (json_string_read_pos == 0) return '\0';
    return str[json_string_read_pos - 1];
}

struct JSON_Object *JSON_Parse_Object(const char *str, const unsigned int len) {
    struct JSON_Object *obj = malloc(sizeof(struct JSON_Object));
    obj->size = 0;
    obj->members = NULL;
    unsigned int character_buff_pos_begin = 0;
    unsigned int finding_member_name = 0;
    unsigned int reading_member_name = 0;
    unsigned int reading_value_string = 0;
    unsigned int reading_value_number = 0;
    unsigned int finding_value = 0;
    unsigned int in_object = 0;

    while (json_string_read_pos < len) {
        const size_t i = json_string_read_pos;
        const char current_char = Advance(str);
        if (IsWhitespace(current_char)) {
            if (reading_value_number) {
                reading_value_number = 0;
                const size_t num_length = i - character_buff_pos_begin;
                char *num_str = malloc((num_length + 1) * sizeof(char));
                memcpy_s(num_str, num_length, str + character_buff_pos_begin, num_length);
                num_str[num_length] = '\0';
                const double number = strtod(num_str, NULL);
                obj->members[obj->size - 1].value.number = number;
                finding_member_name = 1;
                free(num_str);
            }
            continue;
        }

        switch (current_char) {
            case JSON_OBJECT_START: {
                if (in_object && finding_value) {
                    Seek(-1);
                    obj->members[obj->size - 1].value.object = JSON_Parse_Object(str, len);
                    obj->members[obj->size - 1].value.type = OBJECT;
                } else if (!in_object &&
                           !finding_value &&
                           !finding_member_name &&
                           !reading_member_name) {
                    in_object = 1;
                    finding_member_name = 1;
                }

                break;
            }
            case JSON_OBJECT_END: {
                if (reading_value_number) {
                    reading_value_number = 0;
                    const size_t num_length = i - character_buff_pos_begin;
                    char *num_str = malloc((num_length + 1) * sizeof(char));
                    memcpy_s(num_str, num_length, str + character_buff_pos_begin, num_length);
                    num_str[num_length] = '\0';
                    const double number = strtod(num_str, NULL);
                    obj->members[obj->size - 1].value.number = number;
                    finding_member_name = 1;
                    free(num_str);
                }

                return obj;
            }
            case JSON_ARRAY_START:
                //TODO
                break;
            case JSON_ARRAY_END:
                //TODO
                break;
            case JSON_STRING_DELIMITER:
                if (finding_member_name) {
                    finding_member_name = 0;
                    reading_member_name = 1;
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
                } else if (reading_member_name) {
                    reading_member_name = 0;
                    const size_t name_size = i - character_buff_pos_begin;
                    obj->members[obj->size - 1].name = malloc(sizeof(char) * name_size + 1);
                    memcpy_s(obj->members[obj->size - 1].name, name_size, str + character_buff_pos_begin,
                             name_size);
                    obj->members[obj->size - 1].name[name_size] = '\0';
                    reading_member_name = 0;
                } else if (finding_value) {
                    finding_value = 0;
                    reading_value_string = 1;
                    obj->members[obj->size - 1].value.type = STRING;
                    character_buff_pos_begin = i + 1;
                } else if (reading_value_string && str[i - 1] != JSON_STRING_ESCAPE_CHARACTER) {
                    reading_value_string = 0;
                    const size_t value_size = i - character_buff_pos_begin;
                    obj->members[obj->size - 1].value.string = malloc(sizeof(char) * value_size + 1);
                    memcpy_s(obj->members[obj->size - 1].value.string, value_size, str + character_buff_pos_begin,
                             value_size);
                    obj->members[obj->size - 1].value.string[value_size] = '\0';
                    finding_member_name = 1;
                }
                break;
            case JSON_MEMBER_NAME_VALUE_SEPARATOR:
                if (!finding_value) {
                    finding_value = 1;
                }
                break;
            case JSON_MEMBER_SEPARATOR: {
                if (reading_value_number) {
                    reading_value_number = 0;
                    const double number = strtod(str + character_buff_pos_begin, NULL);
                    obj->members[obj->size - 1].value.number = number;
                }
                if (!reading_value_string) {
                    finding_member_name = 1;
                }
                break;
            }
            default:
                if (finding_value && IsDigitOrSign(str[i])) {
                    finding_value = 0;
                    reading_value_number = 1;
                    character_buff_pos_begin = i;
                    obj->members[obj->size - 1].value.type = NUMBER;
                }
                break;
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
#ifdef _cplusplus
}
#endif
