#include "server.h"

/*
** Receives either group or user profile pic
*/
char *receive_photo(SSL *ssl, char *type, char *nick) {
    int size;
    char *imagearray;
    FILE *image;
    char *path;

    size = 0;
    path = malloc(1024);
    sprintf(path, "userver/resource/%s_photos/", type);
    recurse_mkdir(path);
    sprintf(path, "userver/resource/%s_photos/%s.png", type, nick);
    // Find the size of the image
    SSL_read(ssl, &size, sizeof(int));
    if (size <= 0) {
        printf("Error has occurred. Size less than or equal to 0\n");
        return NULL;
    }

    image = fopen(path, "w");

    if (image == NULL) {
        printf("Error has occurred. Image file could not be opened\n");
        return NULL;
    }

    imagearray = ssl_get_bitmap(ssl, size);
    fwrite(imagearray, 1, size, image);

    fclose(image);
    printf("Image successfully Received!\n");
    return path;
}
