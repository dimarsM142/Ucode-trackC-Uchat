#include "client.h"

int send_picture(SSL *ssl, char *image) {
    FILE *picture;
    int size;
    char *send_buffer;

    picture = fopen(image, "r");

    if (picture == NULL) {
        size = 0;
        SSL_write(ssl, &size, sizeof(int));
        return 0;
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    SSL_write(ssl, &size, sizeof(int));

    send_buffer = malloc(size);
    fread(send_buffer, 1, size, picture);

    SSL_write(ssl, send_buffer, size);
    return 1;
}
