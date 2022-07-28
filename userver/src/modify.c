#include "server.h"

/*
** Adds a user to the list of group members
*/
static int add(t_sdvuc *sdvuc, char *buffer) {
    char *group;
    char *user;
    char *query;
    char *errmsg;

    group = strtok(buffer, " ");
    if (!group)
        return 0;
    while ((user = strtok(NULL, " "))) {
        query = sqlite3_mprintf(
            "INSERT INTO " GROUP_MEMBERS_DB "("
            "GID, UID)"
            "VALUES("
            "(SELECT GID FROM " GROUPS_DB " WHERE Group_nick = '%s'), "
            "(SELECT UID FROM " USERS_DB "  WHERE Nickname   = '%s'));",
            group, user);
        printf("|%s|", group);
        fflush(stdout);
        if (sqlite3_exec(sdvuc->db, query, 0, 0, &errmsg) != SQLITE_OK)
            fprintf(stderr, "Failed to insert:%s\n", errmsg);
    }
    return 1;
}

/*
** Removes a user from the list of group members
*/
static int kick(t_sdvuc *sdvuc, char *buffer) {
    char *group;
    char *user;
    char *query;
    char *errmsg;

    group = strtok(buffer, " ");
    if (!group)
        return 0;
    while ((user = strtok(NULL, " "))) {
        query = sqlite3_mprintf(
            "DELETE FROM " GROUP_MEMBERS_DB " WHERE "
            " UID IN (SELECT UID FROM " USERS_DB " WHERE Nickname = '%s') AND"
            " GID IN (SELECT GID FROM " GROUPS_DB
            " WHERE Group_nick = '%s') AND"
            "(UID = %d OR EXISTS (SELECT Owner FROM " GROUPS_DB
            " WHERE Owner = %d));",
            user, group, sdvuc->uid, sdvuc->uid);
        if (sqlite3_exec(sdvuc->db, query, 0, 0, &errmsg) != SQLITE_OK)
            fprintf(stderr, "Failed to insert:%s\n", errmsg);
    }
    return 1;
}

/*
** Modifies the state of a user in a group
*/
int modify(t_sdvuc *sdvuc, char *buffer) {
    if (strncmp(buffer, "ADD ", 4) == 0)
        return add(sdvuc, &buffer[4]);
    if (strncmp(buffer, "KICK ", 5) == 0)
        return kick(sdvuc, &buffer[4]);
    return 0;
}
