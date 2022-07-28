#include "server.h"

/*
** Function that signs the client in.
** Returns uid on success
*/
static int signin(SSL *ssl, sqlite3 *db) {
    char *result;
    char *nickname;
    char *hash;
    char *errmsg;
    int uid;

    uid = -1;
    nickname = ssl_get_string(ssl);
    hash = ssl_get_string(ssl);

    result = sqlite3_mprintf("SELECT UID FROM " USERS_DB
                             " WHERE Nickname = '%s' AND Password_hash = '%s'",
                             nickname, hash);
    if (sqlite3_exec(db, result, id, &uid, &errmsg) != SQLITE_OK)
        fprintf(stderr, "Failed to read database:%s\n", errmsg);
    sqlite3_free(result);
    free(nickname);
    free(hash);

    if (uid == -1) {
        SSL_write(ssl, "NOT FOUND", 10);
        return 0;
    } else {
        SSL_write(ssl, "FOUND", 6);
        return uid;
    }
}

/*
** Function that signs up the client
** Returns uid on success
*/
static int signup(SSL *ssl, sqlite3 *db) {
    char *result;
    char *username;
    char *nickname;
    char *hash;
    char *pfp;
    char *errmsg;
    int uid;

pfp = "";
    username = ssl_get_string(ssl);
    nickname = ssl_get_string(ssl);
    hash = ssl_get_string(ssl);
    //pfp = receive_photo(ssl, "profile", nickname);
    uid = -1;

    result = sqlite3_mprintf(
        "SELECT UID FROM " USERS_DB " WHERE Nickname = '%s'", nickname);
    if (sqlite3_exec(db, result, id, &uid, &errmsg) != SQLITE_OK)
        fprintf(stderr, "Failed to read database:%s\n", errmsg);
    sqlite3_free(result);

    if (uid == -1) {
        result = sqlite3_mprintf("INSERT INTO " USERS_DB "("
                                 "Username, Nickname, Password_hash, Photo)"
                                 "VALUES("
                                 "'%s', '%s', '%s', '%s');"
                                 "SELECT UID FROM " USERS_DB
                                 " WHERE Nickname = '%s'",
                                 username, nickname, hash, pfp, nickname);
        if (sqlite3_exec(db, result, id, &uid, &errmsg) != SQLITE_OK)
            fprintf(stderr, "Failed to read database:%s\n", errmsg);
        sqlite3_free(result);
    } else {
        puts("already exists");
    }

    free(nickname);
    free(hash);
    return uid;
}

/*
** Returns UID on log in, 0 if unsuccessful.
** Called repeatedly until logged in correctly
*/
int login(SSL *ssl, sqlite3 *db) {
    char *buffer;

    buffer = ssl_get_string(ssl);
    while (strncmp(buffer, "LOGIN", 6) != 0) {
        free(buffer);
        buffer = ssl_get_string(ssl);
    }
    free(buffer);

    buffer = ssl_get_string(ssl);
    if (strncmp(buffer, "SIGNUP", 7) == 0) {
        return signup(ssl, db);
    }
    if (strncmp(buffer, "SIGNIN", 7) == 0) {
        return signin(ssl, db);
    }

    return 0;
}
