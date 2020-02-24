#include "commando.h"
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
    free(cmd->output);  // Free the command output
    free(cmd);
}

void cmd_start(cmd_t *cmd) {
    snprintf(cmd->str_status, STATUS_LEN+1, "RUN");
    pipe(cmd->out_pipe);  // set up pipe to catch stdout
    int pid = fork();
    if (pid == 0) {
        dup2(cmd->out_pipe[PWRITE], STDOUT_FILENO);
        close(cmd->out_pipe[PREAD]);
        if (execvp(cmd->name, cmd->argv) == -1) {
            printf("'%s' not found\n", cmd->name);
            exit(1);
        }
    } else {
        close(cmd->out_pipe[PWRITE]);
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
    }
    else if (status > 0) {
        if (WIFEXITED(cmd->status)) {
            cmd->status = WEXITSTATUS(cmd->status);
            snprintf(cmd->str_status, STATUS_LEN+1, "EXIT(%d)", cmd->status);
            cmd->finished = 1;
            cmd_fetch_output(cmd);
            printf("@!!! %s[#%d]: %s\n", cmd->name, cmd->pid, cmd->str_status);
        }
    }
}
char *read_all(int fd, int *nread) {
    int buf_size = 1024; // 1 KB initial buffer size
    char *buf = malloc(buf_size*sizeof(char));

    int bytes_read = 0;  // Num of bytes read in after each read() call
    int buf_pos = 0;  // Keep track of position in buffer
    int read_size = 1024;  // Start with read size 1 KB
    while (1) {
        bytes_read = read(fd, buf+buf_pos, read_size);
        buf_pos += bytes_read;
        if (bytes_read == 0) {
            break;
        }
        if (buf_pos == buf_size) {
            buf_size *= 2;
            read_size = buf_size / 2;
            buf = realloc(buf, buf_size);

            if (buf == NULL) {
                printf("Error allocating memory for read_all()\n");
                exit(1);
            }
        }
    }
    buf = realloc(buf, buf_size+1);
    buf[buf_pos] = '\0';

    if (buf == NULL) {
        printf("Error allocating memory for read_all()\n");
        exit(1);
    }

    *nread = buf_pos;
    return buf;
}
void cmd_fetch_output(cmd_t *cmd) {
    // Check if cmd finished
    if (cmd->finished != 1) {
        printf("%s[#%d] not finished yet\n", cmd->name, cmd->pid);
        return;
    }

    // Read from pipe and set fields in cmd
    cmd->output = read_all(cmd->out_pipe[PREAD], &cmd->output_size);

    // Close pipe
    close(cmd->out_pipe[PREAD]);
}
void cmd_print_output(cmd_t *cmd) {
    // If output is a nullptr, then cmd is not ready
    if (cmd->output == NULL) {
        printf("%s[#%d] : output not ready\n", cmd->name, cmd->pid);
        return;
    }
    // Otherwise, display the command's contents to the terminal
    write(STDOUT_FILENO, cmd->output, cmd->output_size);
}
