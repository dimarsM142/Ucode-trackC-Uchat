#ifndef SERVER_H
#define SERVER_H

#include "sqlite3.h"     /* sql server*/
#include <arpa/inet.h>   /* for using ascii to network bit*/
#include <errno.h>       /* USING THE ERROR LIBRARY FOR FINDING ERRORS*/
#include <malloc.h>      /* FOR MEMORY ALLOCATION */
#include <netinet/in.h>  /* network to asii bit */
#include <openssl/err.h> /* helps in finding out openssl errors*/
#include <openssl/ssl.h> /* using openssl function's and certificates and configuring them*/
#include <resolv.h>      /* server to find out the runner's IP address*/
#include <stdbool.h>     /* standard boolean*/
#include <stdio.h>       /* standard i/o*/
#include <string.h>      /* using fgets funtions for geting input from user*/
#include <strings.h>     /* using bzero*/
#include <sys/socket.h>  /* for creating sockets*/
#include <sys/stat.h>    /* umask */
#include <sys/types.h>   /* for using sockets*/
#include <unistd.h>      /* FOR USING FORK for at a time send and receive messages*/

#define BUFFER 1024 /* buffer for reading messages*/
#define CERTFILE "ssl_certificate.pem"
#define DATABASE "resource/database.db"
#define FAIL -1 /* for error output == -1 */
#define FRIEND_PAIRS_DB "Friend_pairs"
#define GROUPS_DB "Groups"
#define GROUP_MEMBERS_DB "Group_members"
#define GROUP_MESSAGES_DB "Group_messages"
#define PERSONAL_MESSAGES_DB "Personal_messages"
#define PUBLIC_MESSAGES_DB "Public_messages"
#define USERS_DB "Users"

enum {
    VIEW_LOGIN = 0x1,
    VIEW_SIGNUP = 0x2,
    VIEW_SIGNIN = 0x4,
    VIEW_OVERVIEW = 0x8,
    VIEW_PUBLIC = 0x10,
    VIEW_GROUP = 0x20,
    VIEW_PERSONAL = 0x40,
};

typedef struct s_sdvuc {
    SSL *ssl;
    sqlite3 *db;
    int view;
    int uid;
    int convo_id;
    int last_time;
} t_sdvuc;

typedef struct s_init {
    sqlite3 *db;
    sqlite3_stmt *database;
    SSL_CTX *ctx;
    int server;
    char *portnum;
} t_init;

char *ssl_get_string(SSL *ssl);
int login(SSL *ssl, sqlite3 *db);
void servlet(SSL *ssl, sqlite3 *db);
t_init *bootup(int argc, char *argv[]);
bool recurse_mkdir(const char *dirname);
int modify(t_sdvuc *sdvuc, char *buffer);
int create(t_sdvuc *sdvuc, char *buffer);
int append(t_sdvuc *sdvuc, char *buffer);
void *ssl_get_bitmap(SSL *ssl, int size);
void receive(t_sdvuc *sdvuc, char *buffer);
int enter_convo(t_sdvuc *sdvuc, char *buffer);
int thread(int server, SSL_CTX *ctx, sqlite3 *db);
int id(void *UID, int argc, char **argv, char **azColName);
char *receive_photo(SSL *ssl, char *type, char *identifier);
int overview(void *ssl, int argc, char **argv, char **azColName);

#endif // SERVER_H
