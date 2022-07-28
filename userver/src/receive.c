#include "server.h"

/*
** Send a list of messages to client
*/
static int messages(void *sdvucv, int argc, char **argv, char **azColName) {
    char *message;
    t_sdvuc *sdvuc;

    sdvuc = sdvucv;
    if (argc && azColName) {
    } /*not needed*/
    message = malloc(sizeof *message * 1024);

    // if (argv[2][0] != '\0')
    //     printf("%s - %s re: %s\n", argv[0], argv[1], argv[2]);
    // else
    //     printf("%s - %s\n", argv[0], argv[1]);
    // fflush(stdout);
    SSL_write(sdvuc->ssl, argv[0], strlen(argv[0]));
    SSL_write(sdvuc->ssl, argv[1], strlen(argv[1]));
    if (argv[2][0] != '\0')
        SSL_write(sdvuc->ssl, argv[2], strlen(argv[2]));
    else
        SSL_write(sdvuc->ssl, "|||ZERO|||", 11);
    SSL_write(sdvuc->ssl, argv[3], strlen(argv[3]));
    if (sdvuc->last_time < strtol(argv[3], NULL, 10))
        sdvuc->last_time = strtol(argv[3], NULL, 10);
    return 0;
}

static int members(void *ssl, int argc, char **argv, char **azColName) {

    if (argc && azColName) {
    } /*not needed*/

    // printf("%s - %s\n", argv[0], argv[1]);
    // fflush(stdout);
    SSL_write(ssl, argv[0], strlen(argv[0]));
    SSL_write(ssl, argv[1], strlen(argv[1]));
    SSL_write(ssl, "|||ZERO|||", 11);
    return 0;
}

/*
** Send data to client depending on the context
*/
void receive(t_sdvuc *sdvuc, char *buffer) {
    char *query;
    char *timed;
    char *errmsg;
    int last_time = sdvuc->last_time;

    timed = malloc(sizeof *timed * 64);
    query = NULL;

    if (strncmp(buffer, " MEMBERS ", 9) == 0) {
        buffer += 9;
        query =
            sqlite3_mprintf("SELECT U.Username, U.Nickname FROM " USERS_DB " U, " GROUP_MEMBERS_DB
                            " GM WHERE (U.UID = GM.UID AND GM.GID IN (SELECT "
                            "GID FROM " GROUPS_DB " WHERE Group_nick = '%s'))",
                            buffer);
        if (sqlite3_exec(sdvuc->db, query, members, sdvuc->ssl, &errmsg) != SQLITE_OK)
            fprintf(stderr, "Failed to read database:%s\n", errmsg);
        return;
    }

    if (sdvuc->view == VIEW_OVERVIEW) { /* when not in any chat */
        query = sqlite3_mprintf(
            "SELECT G.Group_name, G.Group_nick, G.Photo FROM " GROUPS_DB " G, " GROUP_MEMBERS_DB
            " GM WHERE GM.UID = %d AND GM.GID = G.GID;"
            "SELECT U.Username,   U.Nickname,   U.Photo, U.Status FROM " USERS_DB " U WHERE "
            "U.UID IN ((SELECT PM.UID FROM " PERSONAL_MESSAGES_DB " PM WHERE PM.Receiver = %d ),"
            "(SELECT PM.Receiver FROM " PERSONAL_MESSAGES_DB " PM WHERE PM.UID = %d ))"
            "GROUP BY U.Username ORDER BY U.Username",
            sdvuc->uid, sdvuc->uid, sdvuc->uid); /* FIXME part after OR is questionable */

        if (sqlite3_exec(sdvuc->db, query, overview, sdvuc->ssl, &errmsg) != SQLITE_OK)
            fprintf(stderr, "Failed to read database:%s\n", errmsg);
        SSL_write(sdvuc->ssl, "EOF", 4);
        return;
    }

    if (last_time)
        sprintf(timed, "AND PM.Time > %d", last_time);
    else
        timed[0] = '\0';

    if (sdvuc->view == VIEW_PUBLIC) /* when in the public */
        query = sqlite3_mprintf("SELECT U.Username, PM.Message, PM.ReID, PM.Time FROM " USERS_DB
                                " U, " PUBLIC_MESSAGES_DB
                                " PM WHERE U.UID = PM.UID %s ORDER BY PM.Time",
                                timed);
    // printf("%s", query);
    // fflush(stdout);

    if (sdvuc->view == VIEW_GROUP) /* when in a group chat */ {
        query = sqlite3_mprintf("SELECT U.Username, GM.Message, GM.ReID, GM.Time FROM " USERS_DB
                                " U, " GROUP_MESSAGES_DB
                                " GM WHERE GM.GID = %d AND U.UID = GM.UID %s ORDER BY GM.Time",
                                sdvuc->convo_id, timed);
    }

    if (sdvuc->view == VIEW_PERSONAL) /* when in a personal chat */ {
        query = sqlite3_mprintf("SELECT U.Username, PM.Message, PM.ReID, PM.Time FROM " USERS_DB
                                " U, " PERSONAL_MESSAGES_DB " PM WHERE"
                                "((PM.Receiver = %d AND PM.UID = %d) OR "
                                "(PM.UID = %d AND PM.Receiver = %d)) AND U.UID = PM.UID %s ORDER "
                                "BY PM.Time",
                                sdvuc->convo_id, sdvuc->uid, sdvuc->convo_id, sdvuc->uid, timed);
    }

    if (sqlite3_exec(sdvuc->db, query, messages, sdvuc, &errmsg) != SQLITE_OK)
        fprintf(stderr, "Failed to read database:%s\n", errmsg);
    SSL_write(sdvuc->ssl, "EOF", 4);
    return;
}
