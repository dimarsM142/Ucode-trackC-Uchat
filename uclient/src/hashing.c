#include "client.h"

char *hashing(char *buffer) { // function returns hashed password

    unsigned int hash = 0;
    for (; *buffer; buffer++) {
        hash += (unsigned char)(*buffer);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return mx_itoa(hash);
}
