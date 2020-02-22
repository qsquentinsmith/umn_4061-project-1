#include "commando.h" // Remove eventually
cmd_t *cmd_new(char *argv[]){

    cmd_t *cmd = malloc(sizeof(cmd_t));
    
    // Duplicate strings from main's argv
    // into the individual cmd's argv.
    int i;
    for(i = 0; argv[i] != NULL; i++){
        cmd->argv[i] = strdup(argv[i]); // Dynamically allocated!!
    }
    

    // set initial values
    strncpy(cmd->name, cmd->argv[0], NAME_MAX+1);  // Include null character 
    cmd->argv[i] = NULL;             // Add Null to end
    cmd->pid = -1;                  
    cmd->out_pipe[0] = -1;           
    cmd->out_pipe[1] = -1; 
    cmd->finished = -1;
    cmd->status = -1;
    snprintf(cmd->str_status, STATUS_LEN+1, "INIT");
    cmd->output = NULL;
    cmd->output_size = -1;

    return cmd;
}

void cmd_free(cmd_t *cmd) {
    for (int i = 0; cmd->argv[i] != NULL; i++) {
        free(cmd->argv[i]); // free all the strdup'd stuff!!
    }
    free(cmd);
}

void cmd_start(cmd_t *cmd) {
    snprintf(cmd->str_status, STATUS_LEN+1, "RUN");
    pipe(cmd->out_pipe);  // set up pipe to catch stdout
    int pid = fork();
    if (pid == 0) {
        dup2(cmd->out_pipe[PWRITE], STDOUT_FILENO);
        close(cmd->out_pipe[PREAD]);  // I feel like this is not right
        execvp(cmd->name, cmd->argv);
    } else {
        close(cmd->out_pipe[PWRITE]);  // I feel like this is not right
        cmd->pid = pid;  // Set child pid in the parent
    }
}

void cmd_update_state(cmd_t *cmd, int block) {
    // If cmd is already finished, no need to update!
    if (cmd->finished == 1) {
        return;
    }

    // Try to wait for the process based on the block method
    int status = waitpid(cmd->pid, &cmd->status, block);

    // Interpret the waitpid return value, if it is 0, there are no status
    // updates. If the value is -1, an error occurred and commando should
    // exit. Otherwise, a state change has occurred and we update cmd to
    // indicate that it has finished.
    if (status == 0) {
        return;
    } else if (status == -1) {
        printf("Error waiting for process %d to finish.\n", cmd->pid);
        exit(0);
    }
    else {
        if (WIFEXITED(cmd->status)) {
            cmd->status = WEXITSTATUS(cmd->status);
            snprintf(cmd->str_status, STATUS_LEN+1, "EXIT(%d)", cmd->status);
            cmd->finished = 1;
            cmd_fetch_output(cmd);
            printf("@!!! %s[#%d]: %s\n", cmd->name, cmd->pid, cmd->str_status);
        }
    }
}
char *read_all(int fd, int *nread);
void cmd_fetch_output(cmd_t *cmd);
void cmd_print_output(cmd_t *cmd);
