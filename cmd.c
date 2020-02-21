#include "commando.h"
cmd_t *cmd_new(char *argv[]){

    cmd_t *cmd = malloc(sizeof(cmd_t));
    
    // Duplicate strings from main's argv
    // into the individual cmd's argv.
    int i;
    for(i = 0; argv[i] != NULL; i++){
        cmd->argv[i] = strdup(argv[i]);
    }
    

    //set initial values
    strncpy(cmd->name, cmd->argv[0], NAME_MAX+1);  // Include null character 
    cmd->argv[i] = NULL;             // Add Null to end
    cmd->pid = -1;                   // pid_t pid defaults to -1, will set later
    cmd->out_pipe[0] = -1;           // Are these good defaults for the pipe?
    cmd->out_pipe[1] = -1; 
    cmd->finished = -1;
    cmd->status = -1;
    cmd->str_status = snprintf("INIT");
    cmd->*output = NULL;
    cmd->output_size = -1;

    return cmd;
}


