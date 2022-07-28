#include "server.h"

/*
**show the ceritficates to client and match them
*/
static void certificates(SSL *ssl) {
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */

    if (cert != NULL) {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Server: %s\n", line); /*server certifcates*/
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("client: %s\n", line); /*client certificates*/
        free(line);
        X509_free(cert);
    } else {
        printf("No certificates.\n");
    }
}

/*
** Exit gracefully
*/
static void breakup(SSL *ssl) {
    int connection = SSL_get_fd(ssl); /* get socket connection */
    SSL_free(ssl);                    /* release SSL state */
    close(connection);                /* close connection */
}

/*
** prefill the sdvuc variable with default values
*/
static t_sdvuc *sdvucs(SSL *ssl, sqlite3 *db) {
    t_sdvuc *sdvuc;
    sdvuc = malloc(sizeof *sdvuc);
    sdvuc->ssl = ssl;
    sdvuc->db = db;
    sdvuc->view = VIEW_LOGIN;
    sdvuc->uid = -1;
    sdvuc->last_time = 0;
    return sdvuc;
}

/*
** Serve the connection -- threadable
** This function communicates with a particular client
*/
void servlet(SSL *ssl, sqlite3 *db) {
    char *buffer;
    t_sdvuc *sdvuc;

    sdvuc = sdvucs(ssl, db);

    if (SSL_accept(ssl) == FAIL) { /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
        breakup(ssl);
    }
    certificates(ssl);

    while (!(sdvuc->uid = login(ssl, db)))
        ;

    sdvuc->view = VIEW_OVERVIEW;

    while (true) {
        buffer = ssl_get_string(ssl); /* get request and read message from client*/
        if (buffer[0] == '\0')
            break;
        if (strncmp(buffer, "RECEIVE", 7) == 0) {
            receive(sdvuc, &buffer[7]);
            continue;
        }
        printf("%s", buffer);
        fflush(stdout);
        if (strncmp(buffer, "ENTER", 5) == 0) {
            enter_convo(sdvuc, &buffer[5]);
            continue;
        }
        if (strncmp(buffer, "CREATE ", 7) == 0) {
            create(sdvuc, &buffer[7]);
            continue;
        }
        if (strncmp(buffer, "MODIFY ", 7) == 0) {
            modify(sdvuc, &buffer[7]);
            continue;
        }
        append(sdvuc, buffer);
    }
    breakup(ssl);
}
