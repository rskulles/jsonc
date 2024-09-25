#include <stdio.h>
#include<json.h>
#include <string.h>
#include<stdlib.h>

int main(void) {
    const char *json = "{\"name\" : \"John Smith\", \"age\" : 69}";
    printf("JSON IN: %s\n", json);
    struct JSON_Object *root = JSON_Parse(json, strlen(json));
    JSON_Print(root);
    free(root);
    return 0;
}
