


cmd_t *cmd_new(char *argv[]){

    cmd_t *cmd = malloc(sizeof(cmd_t));
    //  cmd -> argv = malloc(sizeof(char*) * ARG_MAX+1);
    
    int i;

    for(i = 0; argv[i] != NULL; i++){
        cmd->argv[i] = strdup(argv[i]);
    }
    

    //set inital values
    cmd->name = argv[0];
    cmd->argv[i] = NULL;    //Add Null to end
    cmd->pid = NULL;
    cmd->out_pipe[2] = -1;
    cmd->finished = 0;
    cmd->status = -1;
    cmd->str_status = snprintf("INIT");
    cmd->*output = NULL;
    cmd->output_size = -1;
}


