// Tie together cmd.c and cmdcol.c into one working shell
#include "commando.h"

int main(int argc, char **argv) {
    // Setup
    setvbuf(stdout, NULL, _IONBF, 0);  // Important for testing purposes

    // Enable echoing if it flag passed in or env var set.
    int echo = 0;
    if (argc > 1 && strncmp(argv[1], "--echo", 6) == 0) {
        echo = 1;
    } else if (getenv("COMMANDO_ECHO") != NULL &&
            strncmp(getenv("COMMANDO_ECHO"), "1", 1) == 0) {
        echo = 1;
    }

    while (1) {
        // Main loop
        printf("@> ");

        // Collect input of a line
        char in[MAX_LINE+1];
        char *args[ARG_MAX+1];
        int num_args = 0;
        fgets(in, MAX_LINE, stdin);

        if (echo) {
            printf("%s", in);
        }

        parse_into_tokens(in, args, &num_args);

        if (num_args == 0) {
            continue;
        }

        if ( strncmp(args[0], "help", NAME_MAX) == 0 ) {
            printf("COMMANDO COMMANDS\n");
            printf("help               : show this message\n");
            printf("exit               : exit the program\n");
            printf("list               : list all jobs that have been started giving information on each\n");
            printf("pause nanos secs   : pause for the given number of nanseconds and seconds\n");
            printf("output-for int     : print the output for given job number\n");
            printf("output-all         : print output for all jobs\n");
            printf("wait-for int       : wait until the given job number finishes\n");
            printf("wait-all           : wait for all jobs to finish\n");
            printf("command arg1 ...   : non-built-in is run as a job\n");
        }
        else if ( strncmp(args[0], "exit", NAME_MAX) == 0 ) {
            break;
        }
    }
    return 0;
}
