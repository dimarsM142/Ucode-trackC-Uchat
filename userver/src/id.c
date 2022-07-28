#include <stdlib.h>

/*
** Simply returns an ID from the database
*/
int id(void *UID, int argc, char **argv, char **azColName) {
    int *ptr;

    if (argc && azColName) {
    }

    ptr = UID;
    *ptr = strtol(argv[0], NULL, 10);
    return 0;
}
