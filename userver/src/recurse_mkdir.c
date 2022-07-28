#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

bool recurse_mkdir(const char *dirname) {
    const char SEP = '/';
    const char *p;
    char *temp;
    bool ret = true;

    temp = calloc(1, strlen(dirname) + 1);
    /* Skip Windows drive letter. */
    p = dirname;

    while ((p = strchr(p, SEP)) != NULL) {
        /* Skip empty elements. Could be a Windows UNC path or
           just multiple separators which is okay. */
        if (p != dirname && *(p - 1) == SEP) {
            p++;
            continue;
        }
        /* Put the path up to this point into a temporary to
           pass to the make directory function. */
        memcpy(temp, dirname, p - dirname);
        temp[p - dirname] = '\0';
        p++;

        if (mkdir(temp, 0774) != 0) {
            if (errno != EEXIST) {
                ret = false;
                break;
            }
        }
    }
    free(temp);
    return ret;
}
