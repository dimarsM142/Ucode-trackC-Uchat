#include "server.h"

static void skeleton_daemon() {
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */

    /* Close all open file descriptors */
    close(STDIN_FILENO);
    freopen("log.txt", "a", stdout);
    freopen("errlog.txt", "a", stderr);
    /* Open the log file */
}

/*
** Main function of the program, nothing fancy.
** Gets port number as argument.
*/
int main(int argc, char *argv[]) {
    skeleton_daemon();
    {
        printf("%d,%s", argc, argv[1]);
        fflush(stdout);
        t_init *init;
        init = bootup(argc, argv);
        while (thread(init->server, init->ctx, init->db)) {
        }

        sqlite3_finalize(init->database);
        sqlite3_close(init->db);
        close(init->server);     /* close server socket */
        SSL_CTX_free(init->ctx); /* release context */
    }
}
