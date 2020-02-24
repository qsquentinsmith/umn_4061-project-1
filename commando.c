// Tie together cmd.c and cmdcol.c into one working shell
#include "commando.h"

int main(int argc, char **argv) {
    // SETUP
    setvbuf(stdout, NULL, _IONBF, 0);  // Important for testing purposes

    // Enable echoing if it flag passed in or env var set.
    int echo = 0;
    if (argc > 1 && strncmp(argv[1], "--echo", 6) == 0) {
        echo = 1;
    } else if (getenv("COMMANDO_ECHO") != NULL &&
            strncmp(getenv("COMMANDO_ECHO"), "1", 1) == 0) {
        echo = 1;
    }

    // Setup cmdcol_t data
    cmdcol_t cmds;
    cmds.size = 0;

    // MAIN LOOP
    while (1) {
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
        else if ( strncmp(args[0], "list", NAME_MAX) == 0 ) {
            cmdcol_print(&cmds);
        }
        else if ( strncmp(args[0], "output-for", NAME_MAX) == 0 ) {
            int jobnum = atoi(args[1]);
            if ( jobnum >= cmds.size || jobnum < 0 ) {
                continue;
            }
            printf("@<<< Output for %s[#%d] (%d bytes):\n",
                    cmds.cmd[jobnum]->name, cmds.cmd[jobnum]->pid,
                    cmds.cmd[jobnum]->output_size);
            printf("----------------------------------------\n");
            cmd_print_output(cmds.cmd[jobnum]);
            printf("----------------------------------------\n");
        }
        else if ( strncmp(args[0], "output-all", NAME_MAX) == 0 ) {
            for (int i = 0; i < cmds.size; i++) {
                printf("@<<< Output for %s[#%d] (%d bytes):\n",
                        cmds.cmd[i]->name, cmds.cmd[i]->pid,
                        cmds.cmd[i]->output_size);
                printf("----------------------------------------\n");
                cmd_print_output(cmds.cmd[i]);
                printf("----------------------------------------\n");
            }
        }
        else if ( strncmp(args[0], "wait-for", NAME_MAX) == 0 ) {
            int jobnum = atoi(args[1]);
            if ( jobnum >= cmds.size || jobnum < 0 ) {
                continue;
            }
            cmd_update_state(cmds.cmd[jobnum], DOBLOCK);

        }
        else if ( strncmp(args[0], "wait-all", NAME_MAX) == 0 ) {
            cmdcol_update_state(&cmds, DOBLOCK);
        }
        else {
            cmd_t *cmd = cmd_new(args);
            cmd_start(cmd);
            cmdcol_add(&cmds, cmd);
        }
        cmdcol_update_state(&cmds, NOBLOCK);
    }
    cmdcol_freeall(&cmds);
    return 0;
}
