#include "client.h"

void *ssl_get_bitmap(SSL *ssl, int size) {
    char *message;
    int total;

    total = 0;

    message = malloc(size + 1); /* max length of bitmap */
    while ((total += SSL_read(ssl, &message[total], size)) < size) {
    }
    message[size] = '\0';

    return message;
}
