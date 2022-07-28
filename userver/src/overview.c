#include "server.h"

/*
** Sends the image to the client
*/
static int send_image(SSL *ssl, char *image) {
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

/*
** Gets all chats from overview
*/
int overview(void *ssl, int argc, char **argv, char **azColName) {
    if (azColName) {
    }
    /* name */
    SSL_write(ssl, argv[0], strlen(argv[0]));

    /* nick */
    SSL_write(ssl, argv[1], strlen(argv[1]));

    /* status */
    if (argc < 4 || !argv[3]) /* if group chat */
        SSL_write(ssl, "|", 2);
    else /* if personal */
        SSL_write(ssl, argv[3], strlen(argv[3]));

    /* photo */
    if (argv[2] && argv[2][0] != '\0')
        send_image(ssl, argv[2]);
    else
        SSL_write(ssl, "|||ZERO|||", 11);

    return 0;
}
