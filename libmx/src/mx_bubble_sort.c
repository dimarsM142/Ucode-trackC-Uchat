#include "libmx.h"

int mx_bubble_sort(char **arr, int size) {
    char *temp;
    int counter = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            char *t_str1 = mx_strdup(arr[j]);
            char *t_str2 = mx_strdup(arr[j + 1]);
            if (mx_strcmp(t_str1, t_str2) > 0) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j + 1] = temp;
                counter++;
            }
            mx_strdel(&t_str1);
            mx_strdel(&t_str2);
        }
    }
    return counter;
}
