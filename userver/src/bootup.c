#include "server.h"

/*
** Opens a listener on a specific port
*/
static int OpenListener(int port) {
    int sd;
    struct sockaddr_in addr; /*creating the sockets*/

    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));  /*free output the garbage space in memory*/
    addr.sin_family = AF_INET;   /*getting ip address form machine */
    addr.sin_port = htons(port); /* converting host bit to n/w bit */
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) !=
        0) {                       /* assiging the ip address and port*/
        perror("can't bind port"); /* reporting error using errno.h library */
        abort();                   /*if error will be there then abort the process */
    }

    if (listen(sd, 10) != 0) { /*for listening to max of 10 clients in the queue*/
        perror("Can't configure listening port"); /* reporting error using
                                                     errno.h library */
        abort(); /*if erroor will be there then abort the process */
    }

    return sd;
}
/*
** creates and sets up ssl context structure
*/
static SSL_CTX *InitServerCTX(void) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms(); /* load & register all cryptos, etc */
    SSL_load_error_strings();     /* load all error messages */
    method = TLS_server_method(); /* create new server-method */
    ctx = SSL_CTX_new(method);    /* create new context from method */

    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

/*
** loads a certificate into an SSL_CTX structure
*/
static void LoadCertificates(SSL_CTX *ctx, char *CertFile, char *KeyFile) {

    /* set the local certificate from CertFile */
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    /* set the private key from KeyFile (may be the same as CertFile) */
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

/*
** wrapper for sqlite3_exec
*/
static void table(sqlite3 *db, char *name) {
    char *erroor;
    if (sqlite3_exec(db, name, 0, 0, &erroor) == SQLITE_OK)
        fprintf(stdout, "Table created successfully\n");
    else
        fprintf(stdout, "%s\n", erroor);
}

// static int select_all(void *NIL, int argc, char **argv, char **azColName) {
//     if (argc && azColName && NIL) {
//     } /*not needed*/

//     for (int i = 0; i < argc; i++)
//         printf("%s,", argv[i] ? argv[i] : "");
//     putchar('\n');
//     return 0;
// }

/*
** create database tables if they don't exist
*/
static void database_create(t_init *init) {
    table(init->db, "CREATE TABLE " USERS_DB "("
                    "UID           INTEGER NOT NULL PRIMARY KEY,"
                    "Username      TEXT,"
                    "Nickname      TEXT NOT NULL UNIQUE,"
                    "Bio           TEXT,"
                    "Password_hash TEXT,"
                    "Photo         TEXT,"
                    "Status        INTEGER NOT NULL DEFAULT 1);");

    table(init->db,
          "CREATE TABLE " GROUPS_DB "("
          "GID           INTEGER NOT NULL PRIMARY KEY,"
          "Owner         INTEGER,"
          "Group_name    TEXT,"
          "Group_nick    TEXT UNIQUE,"
          "Bio           TEXT,"
          "Photo         TEXT,"
          "FOREIGN KEY (Owner) REFERENCES " USERS_DB "(UID) ON DELETE CASCADE ON UPDATE CASCADE);");

    table(init->db,
          "CREATE TABLE " GROUP_MEMBERS_DB "("
          "Member_ID INTEGER NOT NULL PRIMARY KEY,"
          "GID       INTEGER,"
          "UID       INTEGER,"
          "Admin     BOOLEAN DEFAULT FALSE,"
          "FOREIGN KEY (GID) REFERENCES " GROUPS_DB "(GID) ON DELETE CASCADE ON UPDATE CASCADE,"
          "FOREIGN KEY (UID) REFERENCES " USERS_DB " (UID) ON DELETE CASCADE ON UPDATE CASCADE);");

    table(init->db,
          "CREATE TABLE " FRIEND_PAIRS_DB "("
          "Pair_ID     INTEGER NOT NULL PRIMARY KEY,"
          "UID         INTEGER,"
          "Friend_ID   INTEGER,"
          "FOREIGN KEY (UID) REFERENCES " USERS_DB " (UID) ON DELETE CASCADE ON UPDATE CASCADE);");

    table(init->db,
          "CREATE TABLE " PUBLIC_MESSAGES_DB "("
          "Message_ID  INTEGER NOT NULL PRIMARY KEY,"
          "UID         INTEGER,"
          "Time        INTEGER,"
          "Message     TEXT,"
          "ReID        INTEGER,"
          "FOREIGN KEY (UID) REFERENCES " USERS_DB " (UID) ON DELETE CASCADE ON UPDATE CASCADE);");

    table(init->db,
          "CREATE TABLE " PERSONAL_MESSAGES_DB "("
          "Message_ID  INTEGER NOT NULL PRIMARY KEY,"
          "UID         INTEGER,"
          "Time        INTEGER,"
          "Receiver    INTEGER,"
          "Message     TEXT,"
          "ReID        INTEGER,"
          "FOREIGN KEY (Receiver) REFERENCES " USERS_DB
          " (UID) ON DELETE CASCADE ON UPDATE CASCADE,"
          "FOREIGN KEY (UID) REFERENCES " USERS_DB " (UID) ON DELETE CASCADE ON UPDATE CASCADE);");

    table(init->db,
          "CREATE TABLE " GROUP_MESSAGES_DB "("
          "Message_ID  INTEGER NOT NULL PRIMARY KEY,"
          "UID         INTEGER,"
          "GID         INTEGER,"
          "Time        INTEGER,"
          "Message     TEXT,"
          "ReID        INTEGER,"
          "FOREIGN KEY (UID) REFERENCES " USERS_DB " (UID) ON DELETE CASCADE ON UPDATE CASCADE,"
          "FOREIGN KEY (GID) REFERENCES " GROUPS_DB " (GID) ON DELETE CASCADE ON UPDATE CASCADE);");

    // if (sqlite3_exec(init->db,
    //                  "SELECT UID, Username, Nickname, Photo FROM " USERS_DB,
    //                  select_all, 0, &errmsg) !=
    //     SQLITE_OK) { /* select message and writer from group messages
    //                   */
    //     fprintf(stderr, "Failed to read database:%s\n", errmsg);
    // }
    // if (sqlite3_exec(
    //         init->db,
    //         "SELECT U.Nickname, PM.Receiver, PM.Message, PM.Time FROM "
    //         USERS_DB " U, " PERSONAL_MESSAGES_DB " PM WHERE PM.UID = U.UID",
    //         select_all, 0, &errmsg) != SQLITE_OK) {
    //     fprintf(stderr, "Failed to read database:%s\n", errmsg);
    // }

    // if (sqlite3_exec(init->db, "SELECT * FROM " GROUPS_DB, select_all, 0,
    //                  &errmsg) != SQLITE_OK) {
    //     fprintf(stderr, "Failed to read database:%s\n", errmsg);
    // }
    // if (sqlite3_exec(init->db, "SELECT * FROM " GROUP_MEMBERS_DB, select_all,
    // 0,
    //                  &errmsg) != SQLITE_OK) {
    //     fprintf(stderr, "Failed to read database:%s\n", errmsg);
    // }
}

/*
** runs once on server startup
*/
t_init *bootup(int argc, char *argv[]) {
    t_init *init;

    puts("bootup started");

    if (argc != 2) {
        printf("Usage: %s \n", argv[0]); /*send the usage guide if syntax of
                                            setting port is different*/
        exit(0);
    }

    init = malloc(sizeof *init);

    if (sqlite3_open(DATABASE, &init->db)) { /* open database */
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(init->db));
        sqlite3_close(init->db);
        free(init);
        return NULL;
    }

    database_create(init);

    SSL_library_init(); /*load encryption and hash algo's in ssl*/
    init->portnum = argv[1];
    init->ctx = InitServerCTX();                      /* initialize SSL */
    LoadCertificates(init->ctx, CERTFILE, CERTFILE);  /* load certs */
    init->server = OpenListener(atoi(init->portnum)); /* create server socket */
    listen(init->server, 1);                          /*setting 5 clients at a time to queue*/
    puts("bootup ended");
    return init;
}
