#include "server.h"

/*
** gets an image from the client and writes it into an array
*/
void *ssl_get_bitmap(SSL *ssl, int size) {
    char *message;
    int total;

    total = 0;

    message = malloc(size + 2); /* max length of bitmap */
    while ((total += SSL_read(ssl, &message[total], size)) != size)
        ;
    message[size + 1] = '\0';

    return message;
}
