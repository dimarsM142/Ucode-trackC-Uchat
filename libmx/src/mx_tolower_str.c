#include "libmx.h"

char *mx_tolower_str(char *str) {
    char *result = mx_strnew(mx_strlen(str));

    for (int i = 0; i < mx_strlen(str); i++) {
        result[i] = mx_tolower(str[i]);
    }
    return result;
}
