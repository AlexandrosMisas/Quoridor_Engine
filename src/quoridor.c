#include "quoridor_functions.h"
#include "utils.h"

#define INITIAL_BOARDSIZE 7


const char *commands[] = {
    "name", "known_command", "list_commands", "quit", 
    "boardsize", "clear_board", "walls", "playmove", 
    "playwall", "genmove", "winner", "showboard", "undo"
};


int main(void) {
    struct state *state = state_create(INITIAL_BOARDSIZE);

    char *line = NULL;
    while ((read_line(&line, stdin), strcmp(line, "quit"))) {
        char *command = strtok(line, " ");
        
        if (strcmp(command, "list_commands") == 0) {
            list_commands(commands, ARRAY_SIZE(commands));
        } else if (strcmp(command, "known_command") == 0) {
            known_command(commands, ARRAY_SIZE(commands), 
                strtok(NULL, " "));
        } else if (strcmp(command, "clear_board") == 0) {
            vector_reset(state->history);
            initialize_board(state);  
            printf("= \n\n");
        } else if (strcmp(command, "showboard") == 0) {
            showboard(state);
        } else if (strcmp(command, "boardsize") == 0) {
            boardsize(state, strtok(NULL, " "));
        } else if (strcmp(command, "playmove") == 0) {
            char *player = strtok(NULL, " ");
            playmove(state, player, strtok(NULL, " "));
        } else if (strcmp(command, "playwall") == 0) {
            char *player = strtok(NULL, " ");
            char *move   = strtok(NULL, " ");
            playwall(state, player, move, strtok(NULL, " "));
        } else if (strcmp(command, "genmove") == 0) {
            genmove(state, strtok(NULL, " "));
        } else if (strcmp(command, "winner") == 0) {
            printf("= %s %s\n\n",
                state->winner == NONE  ? "false" : "true",
                state->winner == BLACK ? "black" :
                state->winner == WHITE ? "white" : "");
        } else if (strcmp(command, "walls") == 0) {
            walls(state, strtok(NULL, " "));
        } else if (strcmp(command, "undo") == 0) {
            undo(state, strtok(NULL, " "));
        } else if (strcmp(command, "name") == 0) {
            printf("= Alexandros\n\n");
        } else {
            printf("? Unknown command\n\n");
        }
        fflush(stdout);
    }
    printf("= \n\n");
    fflush(stdout);
    
    state_destroy(state);
    free(line);

    return 0;
}

