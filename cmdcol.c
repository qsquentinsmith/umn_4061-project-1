// cmdcol.c: functions related to cmdcol_t collections of commands.
#include "commando.h"  // REMOVE THIS LINE AFTER COMPLETE

void cmdcol_add(cmdcol_t *col, cmd_t *cmd) {
    // Check if a cmd can be added
    if (col->size == MAX_CMDS) {
        printf("Cannot exceed maximum number of commands\n");
        return;
    }

    // If all clear, add the cmd and increment the size
    col->cmd[col->size++] = cmd;
}
void cmdcol_print(cmdcol_t *col) {
    printf("JOB  #PID      STAT   STR_STAT OUTB COMMAND");
    for (int i = 0; i < col->size; i++) {
        cmd_t *cmd = col->cmd[i];
        printf("%-4d ", i);
        printf("#%-8d ", cmd->pid);
        printf("%4d ", cmd->status);
        printf("%10s ", cmd->str_status);
        printf("%4d ", cmd->output_size);
        
        for (int j = 0; cmd->argv[j] != NULL; j++) {
            printf("%s ", cmd->argv[j]);
        }
        printf("\n");
    }
}
void cmdcol_update_state(cmdcol_t *col, int block) {
    for (int i = 0; i < col->size; i++) {
        cmd_update_state(col->cmd[i], block);
    }
}
void cmdcol_freeall(cmdcol_t *col) {
    // Free each cmd_t within the array
    // using the earlier defined cmd_free()
    for (int i = 0; i < col->size; i++) {
        cmd_free(col->cmd[i]);
    }
}
