#include "server.h"

/*
** wait for a client and create a connection
*/
int thread(int server, SSL_CTX *ctx, sqlite3 *db) {
    struct sockaddr_in addr; /*socket for server*/
    socklen_t len;
    int client;
    SSL *ssl;
    pid_t pid;

    len = sizeof(addr);
    client = accept(server, (struct sockaddr *)&addr,
                    &len); /* here we wait for client to connect */
    printf("Connection: %s:%d\n", inet_ntoa(addr.sin_addr),
           ntohs(addr.sin_port)); /*printing connected client information*/
    ssl = SSL_new(ctx);           /* get new SSL state with context */
    SSL_set_fd(ssl, client);      /* set connection socket to SSL state */

    pid = fork();
    if (pid == 0) {
        servlet(ssl, db); /* service connection */
        exit(0);
        return -1;
    } else
        return 1;

    return 0;
}
