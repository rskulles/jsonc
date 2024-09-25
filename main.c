#include<stdio.h>
#include<json.h>
#include <string.h>
#include<stdlib.h>
#include<locale.h>

int main(void) {
    setlocale(LC_ALL,"");
    const char *json = "{\"name\" : \"John Smith\", \"age\" : 69, \"inner\" : { \"inner_name\" : \"inner John Smith\", \"inner_age\" : 420}}";
    printf("JSON IN: %s\n", json);
    struct JSON_Object *root = JSON_Parse(json, strlen(json));
    JSON_Print(root,0);
    free(root);
    return 0;
}
