#include "client.h"

/*char *receive_photo(SSL *ssl, char *type, char *nick) {
    int size;
    char *imagearray;
    FILE *image;
    char *path;

    size = 0;
    path = malloc(1024);
    sprintf(path, "uclient/resource/%s_photos/", type);
    recurse_mkdir(path);
    sprintf(path, "uclient/resource/%s_photos/%s.png", type, nick);
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
}*/

int overview(SSL *ssl) {
    char *name;
    char *nick;
    //char *image;
    char *status;
    char *type;

    name = ssl_get_string(ssl);
    if (!mx_strncmp(name, "EOF", 4)) {
        return 0;
    }
    printf("%s, ", name);
    fflush(stdout);

    nick = ssl_get_string(ssl);
    printf("%s, ", nick);
    fflush(stdout);

    status = ssl_get_string(ssl);
    if (strncmp(status, "|", 2) == 0) {
        type = "groups";
    } else {
        type = "profile";
        printf("%s", status);
        fflush(stdout);
    }

    //image = receive_photo(ssl, type, nick);
    //if (!image || strncmp(image, "|||ZERO|||", 11) == 0)
      //  printf("NULL");
    //else
      //  printf("%s", image);

    printf("\n");

    fflush(stdout);

    free(name);
    //free(image);
    return 1;
}
