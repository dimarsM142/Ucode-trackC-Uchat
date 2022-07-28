#include "server.h"

/*
** Selects a conversation to enter
*/
int enter_convo(t_sdvuc *sdvuc, char *buffer) {
    char *statement;
    char *errmsg;

    sdvuc->convo_id = -1;
    sdvuc->last_time = 0;

    if (buffer[0] == '\0')
        return 0; /* changed convo */

    buffer += sizeof *buffer;

    if (strncmp(buffer, "OVERVIEV", 9) == 0) {
        sdvuc->view = VIEW_OVERVIEW;
        return 1;
    }

    if (strncmp(buffer, "PUBLIC", 7) == 0) {
        sdvuc->view = VIEW_PUBLIC;
        return 1;
    }

    if (strncmp(buffer, "GROUP ", 6) == 0) {
        sdvuc->view = VIEW_GROUP;
        statement = sqlite3_mprintf("SELECT GID FROM " GROUPS_DB
                                    " WHERE Group_nick = '%s'",
                                    &buffer[6]);
        if (sqlite3_exec(sdvuc->db, statement, id, &sdvuc->convo_id, &errmsg) !=
            SQLITE_OK)
            fprintf(stderr, "Failed to read database:%s\n", errmsg);
    }

    if (strncmp(buffer, "PERSONAL ", 9) == 0) {
        sdvuc->view = VIEW_PERSONAL;
        sdvuc->convo_id = strtol(&buffer[9], NULL, 10);
        statement = sqlite3_mprintf(
            "SELECT UID FROM " USERS_DB " WHERE Nickname = '%s'", &buffer[9]);
        if (sqlite3_exec(sdvuc->db, statement, id, &sdvuc->convo_id, &errmsg) !=
            SQLITE_OK)
            fprintf(stderr, "Failed to read database:%s\n", errmsg);
    }

    printf("%d", sdvuc->convo_id);
    fflush(stdout);
    if (sdvuc->convo_id == -1) { /* did not change convo */
        sdvuc->view = VIEW_OVERVIEW;
        return 0;
    } else {
        return 1;
    }
}
