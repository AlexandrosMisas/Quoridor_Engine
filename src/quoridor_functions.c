#include "quoridor_functions.h"
#include "aux_functions.h"
#include "utils.h"

#define PRINT_CHAR(times, ch) for (int k = 0; k < (times); ++k) putchar((ch))


void known_command(const char **commands, uint32_t size, char *search) {
    if (search == NULL) {
        printf("? Error! No command was given\n"
            "Usage: known_command < command to be searched > \n\n");
        fflush(stdout);
        return;
    }

    bool found = false;
    for (size_t i = 0UL; i < size; ++i) {
        if (strcmp(commands[i], search) == 0) {
            found = true;
            break;
        }
    }
    printf("%s\n\n", found ? "= true" : "? false");
    fflush(stdout);
}


void list_commands(const char **commands, uint32_t size) {
    printf("= \n\n");
    
    for (size_t i = 0UL; i < size; ++i)
        printf("%s\n", commands[i]);
    
    printf("\n\n");
    fflush(stdout);
}


void walls(State *state, const char *num) {
    if (num == NULL) {
        printf("? Error! The number of walls was not given\n"
            "Usage: walls < number of walls to be set > \n");
        fflush(stdout);
        return;
    }

    if (!is_valid_number(num, &state->max_walls)) {
        printf("? Error! Not a proper arithmetic constant was given\n");
        fflush(stdout);
        return;   
    }
    state->white.walls = state->black.walls = state->max_walls;

    printf("= \n\n");
    fflush(stdout);
}


void undo(State *state, const char *times) {
    uint32_t times_;
    if (times != NULL && !is_valid_number(times, &times_)) {
        printf("? Error! Not a proper arithmetic constant was given\n");
        fflush(stdout);
        return;
    }

    times_ = times != NULL ? times_ : 1;
    int res = vector_size(state->history) - times_;
    if (res < 0) {
        printf("? Cannot undo\n\n");
        fflush(stdout);
        return;
    }

    uint32_t index = vector_size(state->history);    
    for (uint32_t i = 0; i < times_; ++i, --index) {
        Move *move = vector_get(state->history, index - 1);
        undo_action(state, move);
        vector_delete(state->history, index - 1);
    }
    printf("= \n\n");
    fflush(stdout);
}


void boardsize(State *state, const char *size) {
    if (size == NULL) {
        printf("? Error! The new size of the board was not given\n"
            "Usage: boardsize < size of the new board >\n");
        fflush(stdout);
        return;
    }
    
    uint32_t size_;
    if (!is_valid_number(size, &size_) || !is_valid_size(size_)) {
        printf("? Error! The size of the board must be an odd number that lies in [3, 25]\n");
        fflush(stdout);
        return;
    }
    
    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        free(state->board[i]);
    free(state->board);


    state->board_size = size_;
    state->black.win = 2 * size_ - 2;
    state->max_walls = (float)7 / 4 * size_ - (float)23 / 4;
    state->board = malloc(sizeof(char*) * (2 * state->board_size - 1));
    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        state->board[i] = malloc(sizeof(char) * (2 * state->board_size - 1));    

    printf("= \n\n");
    fflush(stdout);
}


void showboard(State *state) {
    printf("=\n\n");
    fflush(stdout);
    PRINT_CHAR(2, ' ');

    for (int i = 0; i < state->board_size; ++i) {
        PRINT_CHAR(3, ' ');
        putchar('A' + i);
    }
    putchar('\n');
    PRINT_CHAR(3, ' ');
    putchar('+');

    for (int i = 0; i < state->board_size; ++i)
        printf("---+");
    putchar('\n');

    int number = state->board_size;
    for (size_t i = 0UL; i <= 2 * state->board_size - 2; ++i) {
        if (i % 2 == 0) {
            printf("%d", number);
            PRINT_CHAR(number >= 10 ? 1 : 2, ' ');
            putchar('|');
            
            for (size_t j = 0UL; j <= 2 * state->board_size - 2; ++j) {
                if (j % 2 == 0)
                    printf(" %c ", state->board[i][j]);
                else 
                    putchar(state->board[i][j] == 'H' ? 'H' : '|');
            }
            putchar('|');
            PRINT_CHAR(2, ' '); 
            printf("%d", number);

            int offset = number-- - state->board_size;
            if (offset == 0 || offset == -1) {
                PRINT_CHAR(2, ' ');
                printf("%s walls: %u",
                    offset == 0 ? "black" : "white",
                    offset == 0 ? state->black.walls : state->white.walls);
            }
        } else {
            PRINT_CHAR(3, ' '); 
            putchar('+');
            for (size_t j = 0UL; j <= 2 * state->board_size - 2; ++j) {
                if (j % 2 == 0)
                    PRINT_CHAR(3, state->board[i][j] == '=' ? '=' : '-');
                else 
                    putchar(
                        state->board[i][j] == '=' || 
                        state->board[i][j] == 'H'
                            ? state->board[i][j]
                            : '+'
                    );
            }
            putchar('+');
        }
        putchar('\n');
    }
    PRINT_CHAR(3, ' '); 
    putchar('+');

    for (int i = 0; i < state->board_size; ++i)
        printf("---+");
        
    putchar('\n'); 
    PRINT_CHAR(2, ' ');
    for (int i = 0; i < state->board_size; ++i) {
        PRINT_CHAR(3, ' ');
        putchar('A' + i);
    }
    PRINT_CHAR(2, '\n');
}


void playmove(State *state, char *player, char *pos) {
    if (state->winner != NONE) {
        printf("A player has already won. Start a new game\n\n");
        fflush(stdout);
        return;
    }

    if (player == NULL || pos == NULL) {
        printf("? Error! No proper arguments were given\n"
            "Usage: playmove < player > < move >\n\n");
        fflush(stdout);
        return;
    }

    if (strcmp(player, "black") && strcmp(player, "b")
     && strcmp(player, "white") && strcmp(player, "w")) {
        printf("? Error! No proper color was given\n\n");
        fflush(stdout);
        return;
    }

    uint32_t col;
    if (!isalpha(pos[0]) || !is_valid_number(&pos[1], &col)) {
        printf("? Error! No proper move was given\n\n");
        fflush(stdout);
        return;
    }

    if (col <= 0 || col > state->board_size || pos[0] < 'a' 
     || pos[0] >= 'a' + state->board_size) {
        printf("? Error! Move is out of bounds\n\n");
        fflush(stdout);
        return;
    }

    Position new = { 
		.x = 2 * (state->board_size - col), 
		.y = 2 * (pos[0] - 'a') 
	};
	
    player_t player_ = player[0] == 'b' ? BLACK : WHITE;
    Position old = player_ == BLACK ? state->black.pos : state->white.pos;
    if (!is_valid_move(state, new, old, player_)) {
        printf("? Illegal move\n\n");
        fflush(stdout);
        return;
    }

    execute_action(state, create_move((Move){ 
        .move = PAWN_MOVE, .player = player_, .pos = new }));
    
    vector_insert(state->history, create_move((Move){ 
        .move = PAWN_MOVE, .player = player_, .pos = old }));

    if (is_terminal_state(state))
        state->winner = player_;
    
    
    printf("= \n\n");
    fflush(stdout);
}


void playwall(State *state, char *player, char *pos, char *wall_type) {
    if (state->winner != NONE) {
        printf("A player has already won. Start a new game\n\n");
        fflush(stdout);
        return;
    }


    if (player == NULL || pos == NULL || wall_type == NULL) {
        printf("? Error! No proper arguments were given\n"
            "Usage: playwall < player > < move > < type of wall >\n\n");
        fflush(stdout);
        return;
    }

    if (strcmp(player, "white") && strcmp(player, "w")
     && strcmp(player, "black") && strcmp(player, "b")) {
        printf("? Error! No proper color was given\n\n");
        fflush(stdout);
        return;
    }


    if ((player[0] == 'w' && state->white.walls == 0)
     || (player[0] == 'b' && state->black.walls == 0)) {
        printf("? Error! Not enough walls\n\n");
        fflush(stdout);
        return;
    }

    
    uint32_t col;
    if (!isalpha(pos[0]) || !is_valid_number(&pos[1], &col)) {
        printf("? Error! No proper move was given\n\n");
        fflush(stdout);
        return;
    }

    
    if (strcmp(wall_type, "horizontal") && strcmp(wall_type, "h")
     && strcmp(wall_type, "vertical") && strcmp(wall_type, "v")) {
        printf("? Error! No proper orientation was given\n\n");
        fflush(stdout);
        return;
    }

    
    if (col <= 1 || col > state->board_size || pos[0] < 'a' 
     || pos[0] >= state->board_size + 'a' - 1) {
        printf("Error! Move is out of bounds\n\n");
        fflush(stdout);
        return;
    }

    
    move_t move_ = wall_type[0] == 'h' ? HOR_WALL : VER_WALL;
    Position pos_ = {
        .x = 2 * (state->board_size - col) + (move_ == HOR_WALL),
        .y = 2 * (pos[0] - 'a') + (move_ != HOR_WALL)
    };

    
    player_t player_ = player[0] == 'w' ? WHITE : BLACK;
    Move move = { .move = move_, .player = player_, .pos = pos_ };
    if (!is_valid_wall(state, move)) {
        printf("? Cannot place wall\n\n");
        fflush(stdout);
        return;
    }
	
    execute_action(state, &move);
    vector_insert(state->history, create_move(move));

    printf("= \n\n");
    fflush(stdout);
}


void genmove(State *state, char *player) {
    if (state->winner != NONE) {
        printf("A player has already won. Start a new game\n\n");
        fflush(stdout);
        return;
    }

    if (player == NULL) {
        printf("? Error! No proper arguments were given\n"
            "Usage: genmove < player > \n\n");
        fflush(stdout);
        return;
    }

    if (strcmp(player, "white") && strcmp(player, "w")
     && strcmp(player, "black") && strcmp(player, "b")) {
        printf("? Error! No proper color was given\n\n");
        fflush(stdout);
        return;
    }

    Move best_move;
    player_t player_ = player[0] == 'b' ? BLACK : WHITE;
    Vector *vec = get_legal_moves(state, player_, false);
    for (size_t i = 0UL; i < vector_size(vec); ++i) {
        Move *move = vector_get(vec, i);

        if ((player_ == WHITE && move->pos.x == state->white.win)
         || (player_ == BLACK && move->pos.x == state->black.win)) {
            memcpy(&best_move, move, sizeof(Move));
            goto finalize;
        }
    }

    best_move = decide_move(state, player_);

    finalize: {
        execute_action(state, &best_move);
        Position pos = best_move.move == PAWN_MOVE
            ? (player_ == BLACK ? state->black.pos : state->white.pos)
            : best_move.pos; 

        vector_insert(state->history, create_move((Move){
            .player = player_, .move = best_move.move, .pos = pos}));
        
        if (is_terminal_state(state))
            state->winner = player_;


        printf("= %c%d %s\n\n",
            best_move.pos.y / 2 + 'A',
            state->board_size - best_move.pos.x / 2,
            best_move.move == HOR_WALL ? "horizontal" :
            best_move.move == VER_WALL ? "vertical"   : ""
        );
        vector_destroy(vec);
    }
}
