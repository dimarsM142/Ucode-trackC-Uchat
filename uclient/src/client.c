#include "client.h"


static int OpenConnection(const char *hostname, int port) {
    int sd;
    struct hostent *host;
    struct sockaddr_in addr; /*creating the sockets*/

    if ((host = gethostbyname(hostname)) == NULL) {
        perror(hostname);
        abort();
    }

    sd = socket(PF_INET, SOCK_STREAM, 0);
    /* setting the connection as tcp it creates endpoint for connection */
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)host->h_addr_list[0];

    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        /*initiate a connection on a socket*/
        close(sd);
        perror(hostname);
        abort();
    }

    return sd;
}

static SSL_CTX *InitCTX(void) {
    /*creating and setting up ssl context structure*/
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
    SSL_load_error_strings();     /* Bring in and register error messages */
    method = TLS_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method);    /* Create new context */

    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}



int main(int argc, char *argv[]) { /* getting port and ip as an argument*/
    SSL_CTX *ctx;
    SSL *ssl;
    int server;
    char *Nickname;

    Nickname = malloc(sizeof *Nickname * 1024);

    if (argc != 3) {
        printf("usage: %s  \n", argv[0]);
        exit(0);
    }
    gtk_init(&argc, &argv);
    
    

    SSL_library_init();                              /*load encryption and hash algo's in ssl*/
    ctx = InitCTX();                                 /* idk */
    server = OpenConnection(argv[1], atoi(argv[2])); /*converting ascii port to interger */
    ssl = SSL_new(ctx);                              /* create new SSL connection state */
    SSL_set_fd(ssl, server);                         /* attach the socket descriptor */

    if (SSL_connect(ssl) == FAIL) { /* perform the connection */
        ERR_print_errors_fp(stderr);
    } else {
        while (!login(ssl, Nickname))
            puts("no such user or password invalid,try again");
        run(ssl, Nickname);
    }

    close(server);     /* close socket */
    SSL_CTX_free(ctx); /* release context */

    return 0;
}
