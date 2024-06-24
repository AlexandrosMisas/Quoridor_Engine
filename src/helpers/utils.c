#include "utils.h"


void read_line(char **line, FILE *stream) {
    static size_t cap = 16UL;

    if (*line == NULL) 
        *line = malloc(cap * sizeof(char));

    size_t index = 0;
    memset(*line, 0, cap * sizeof(char));
    for (int ch = fgetc(stream); 
        ch != EOF && ch != '\n' && ch != '#'; 
        ch = fgetc(stream)) {

        if (index + 1 == cap) {
            cap *= 2;
            *line = realloc(*line, cap * sizeof(char));
        }
        (*line)[index++] = iscntrl(ch) ? ' ' : tolower(ch);
    }
    (*line)[index] = '\0';
}


void initialize_board(struct state *state) {
    uint32_t size = 2 * state->board_size - 2;
    for (size_t i = 0UL; i <= size; ++i)
        memset(state->board[i], ' ', sizeof(char) * (size + 1));

    state->black.pos.x = 0;
    state->black.pos.y = size >> 1;
    state->white.pos.x = size;
    state->white.pos.y = size >> 1;
    state->white.walls = state->black.walls = state->max_walls;
    state->winner = NONE;
    state->board[0][size >> 1] = 'B';
    state->board[size][size >> 1] = 'W';
}


struct state *state_create(uint32_t size) {
    struct state *state = calloc(1, sizeof(*state));
    state->history = vector_create(struct move, VEC_MIN_CAP, NULL, false);
    state->board_size = size;
    state->max_walls = (float)7 / 4 * size - (float)23 / 4;
    state->white.win = 0;
    state->black.win = 2 * size - 2;

    state->board = malloc(sizeof(char*) * (2 * size - 1));
    for (size_t i = 0UL; i < 2 * size - 1; ++i)
        state->board[i] = malloc(sizeof(char) * (2 * size - 1));

    initialize_board(state);

    return state;
}


void state_destroy(struct state *state) {
    for (size_t i = 0UL; i < 2 * state->board_size - 1; ++i)
        free(state->board[i]);
    free(state->board);

    vector_destroy(state->history);
    free(state);
}


void execute_action(struct state *state, struct move move) {
    struct player *_player = move.player == BLACK
        ? &state->black
        : &state->white;

    if (move.move == PAWN_MOVE) {
        state->board[move.pos.x][move.pos.y] = move.player == BLACK ? 'B' : 'W';
        state->board[_player->pos.x][_player->pos.y] = ' ';
        memcpy(&_player->pos, &move.pos, sizeof(struct point));
    } else {
        if (move.move == VER_WALL)
            for (size_t i = 0UL; i < 3; ++i)
                state->board[move.pos.x + i][move.pos.y] = 'H';
        else
            for (size_t i = 0UL; i < 3; ++i)
                state->board[move.pos.x][move.pos.y + i] = '=';
        
        _player->walls--;
    }
}


void undo_action(struct state *state, struct move move) {
    struct player *_player = move.player == BLACK
        ? &state->black
        : &state->white;

    if (move.move == PAWN_MOVE) {
        state->board[_player->pos.x][_player->pos.y] = ' ';
        state->board[move.pos.x][move.pos.y] = move.player == BLACK ? 'B' : 'W';
        memcpy(&_player->pos, &move.pos, sizeof(struct point));
    } else {
        if (move.move == VER_WALL)
            for (size_t i = 0UL; i < 3; ++i)
                state->board[move.pos.x + i][move.pos.y] = ' ';
        else 
            for (size_t i = 0UL; i < 3; ++i)
                state->board[move.pos.x][move.pos.y + i] = ' ';
        
        _player->walls++;
    }
}


bool is_valid_number(const char *s, uint32_t *num) {
    size_t len = strlen(s);
    uint32_t _num = 0;    
    for (size_t i = 0UL; i < len; ++i) {
        if (!isdigit(s[i]))
            return false;
        _num = 10 * _num + (s[i] - '0');
    }
    if (num != NULL) 
        *num = _num;
    
    return true;
}


bool is_valid_size(uint32_t size) {
    return size % 2 != 0 && (size >= 3 && size <= 25);
}


bool is_valid_coord(struct point pos, uint32_t size) {
    return pos.x >= 0 && pos.x <= 2 * size - 2 
        && pos.y >= 0 && pos.y <= 2 * size - 2;
}


uint32_t man_dist(struct point a, struct point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}


bool is_terminal_state(struct state *state) {
    return state->white.pos.x == state->white.win 
        || state->black.pos.x == state->black.win;
}


int64_t current_time(void) {
    struct timeval time;
    if (gettimeofday(&time, NULL) < 0) {
        perror("gettimeofday");
        exit(EXIT_FAILURE);
    }
    
    return (int64_t)(time.tv_sec) * 1000 + 
           (int64_t)(time.tv_usec / 1000);
}

