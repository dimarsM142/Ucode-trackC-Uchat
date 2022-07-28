#include "server.h"

/*
** processes message in the format "[re: *MessageID* ]*Message*",
** re:... is optional
** return statement is not used for now
*/
int append(t_sdvuc *sdvuc, char *buffer) {
    char *errmsg;
    char *saved;
    char *re;

    re = NULL;
    if (strncmp(buffer, "re:", 3) ==
        0) { /* FIXME message cuts off on first word */
        re = strtok(&buffer[3], " ");
    }

    if (sdvuc->view == VIEW_PUBLIC) /* when in the public */
        saved = sqlite3_mprintf(
            "INSERT INTO " PUBLIC_MESSAGES_DB "("
            "UID, Message, ReID, Time)"
            "VALUES("
            "%d, '%s', '%s', %d);",
            sdvuc->uid, re ? &buffer[strlen(re) + 5] : buffer, re, time(NULL));

    if (sdvuc->view == VIEW_GROUP) /* when in the public */
        saved = sqlite3_mprintf("INSERT INTO " GROUP_MESSAGES_DB "("
                                "GID, UID, Message, ReID, Time)"
                                "VALUES("
                                "%d, %d, '%s', '%s', %d);",
                                sdvuc->convo_id, sdvuc->uid,
                                re ? &buffer[strlen(re) + 5] : buffer, re,
                                time(NULL));

    if (sdvuc->view == VIEW_PERSONAL) /* when in the public */
        saved = sqlite3_mprintf("INSERT INTO " PERSONAL_MESSAGES_DB "("
                                "UID, Receiver, Message, ReID, Time)"
                                "VALUES("
                                "%d, %d, '%s', '%s', %d);",
                                sdvuc->uid, sdvuc->convo_id,
                                re ? &buffer[strlen(re) + 5] : buffer, re,
                                time(NULL));

    if (sqlite3_exec(sdvuc->db, saved, 0, 0, &errmsg) != SQLITE_OK)
        fprintf(stderr, "Failed to insert:%s\n", errmsg);
    printf("MESSAGE FROM CLIENT:%s\n", buffer);
    return 1;
}
