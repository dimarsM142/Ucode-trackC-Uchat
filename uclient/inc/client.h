#ifndef CLIENT_H
#define CLIENT_H

#include "libmx.h"
#include <errno.h> /*USING THE ERROR LIBRARY FOR FINDING ERRORS*/
#include <gtk/gtk.h>
#include <netdb.h>       /*definitions for network database operations */
#include <openssl/err.h> /* helps in finding out openssl errors*/
#include <openssl/ssl.h> /*using openssl function's and certificates and configuring them*/
#include <resolv.h>      /*server to find out the runner's IP address*/
#include <stdbool.h>     /* booleans */
#include <stdio.h>       /*standard i/o*/
#include <stdlib.h>      /*FOR MEMORY ALLOCATION */
#include <string.h>      /*using fgets funtions for geting input from user*/
#include <strings.h>     /*using bzero*/
#include <sys/socket.h>  /*for creating sockets*/
#include <time.h>
#include <unistd.h> /*FOR USING FORK for at a time send and receive messages*/

#define BUFFER 1024 /*buffer for reading messages*/
#define FAIL -1     /*for error output == -1 */

#define MX_FT_SCRATCH "~~"
#define MX_FT_BOLD "**"
#define MX_FT_IMPORTANT "``"
#define MX_FT_ITALIC "##"
#define MX_FT_UNDER "__"

/*
 * Formatting spans
 */
#define MX_OP_SCRATCH "<span strikethrough=\"true\">%s</span>"
#define MX_OP_BOLD "<span font_weight=\"bold\">%s</span>"
#define MX_OP_IMPORTANT "<span background=\"#FF698C7F\">%s</span>"
#define MX_OP_ITALIC "<span font_style=\"italic\">%s</span>"
#define MX_OP_UNDER "<span underline=\"single\">%s</span>"
#define MX_CLOSE_SPAN "</span>"

enum {
    VIEW_LOGIN = 0x1,
    VIEW_SIGNUP = 0x2,
    VIEW_SIGNIN = 0x4,
    VIEW_OVERVIEW = 0x8,
    VIEW_PUBLIC = 0x10,
    VIEW_GROUP = 0x20,
    VIEW_PERSONAL = 0x40,
    VIEW_NYI = 0x80,
};

struct s_callback {
    GtkWidget *message_entery;
    GtkWidget *search_entery;
    SSL *ssl;
    char *input;
    char *input_usr;
    int flag;
    int VIEW;
    char *Nickname;
} typedef t_callback;

struct s_callback_log {
    GtkWidget *login_entery;
    GtkWidget *nick_entery;
    GtkWidget *pass_entery;
    SSL *ssl;
    char *input_login;
    char *input_nick;
    char *input_pass;
} typedef t_callback_log;

int overview(SSL *ssl);
char *ssl_get_string(SSL *ssl);
int run(SSL *ssl, char *Nickname);
gchar *mx_format_text(gchar *text);
int login(SSL *ssh, char *Nickname);
int send_picture(SSL *ssl, char *image);
bool recurse_mkdir(const char *dirname);
void *ssl_get_bitmap(SSL *ssl, int size);
char *hashing(char *buffer);

#endif // CLIENT_H
