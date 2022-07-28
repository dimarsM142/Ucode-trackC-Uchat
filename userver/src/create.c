#include "server.h"

/*
** create a new group or personal chat
*/
int create(t_sdvuc *sdvuc, char *buffer) {
    char *errmsg;
    char *statement;
    char *group_nick;
    char *group_name;
    char *bio;
    void *photo;

    if (!buffer[0])
        return 0;
    strtok(buffer, " ");
    group_name = strtok(NULL, " ");
    group_nick = strtok(NULL, " ");
    bio = strtok(NULL, " ");
    photo = receive_photo(sdvuc->ssl, "group", group_nick);
    printf("%s,%s,%s,%s", group_name, group_nick, bio, (char *)photo);

    statement =
        sqlite3_mprintf("INSERT INTO " GROUPS_DB " ("
                        "Owner, Group_name, Group_nick, Bio, Photo)"
                        "VALUES ("
                        "%d, '%s', '%s', '%s', '%s');"
                        "INSERT INTO " GROUP_MEMBERS_DB //
                        "(GID, UID)"
                        "VALUES "
                        "((SELECT GID FROM " GROUPS_DB " WHERE Group_nick = '%s'), %d)",
                        sdvuc->uid, group_name, group_nick, bio, photo, group_nick, sdvuc->uid);
    if (sqlite3_exec(sdvuc->db, statement, 0, 0, &errmsg) != SQLITE_OK)
        fprintf(stderr, "Failed to insert:%s\n", errmsg);
    return 1;
}
