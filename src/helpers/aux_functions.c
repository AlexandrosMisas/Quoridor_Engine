#include "aux_functions.h"
#include "queue.h"
#include "bitset.h"


struct search_args {
    int alpha;
    int beta;
    int depth;
    enum player_t player;
    bool is_maximizing;
};


static struct move minimax(struct state *state, int64_t start, 
                           enum player_t maximizer, struct search_args args);
static int evaluate(struct state *state, enum player_t maximizer);



bool path_exists(struct state *state, enum player_t player, uint32_t *min_steps, uint32_t target) {
    struct point pos = player == WHITE ? state->white.pos : state->black.pos;
    char op = player == WHITE ? 'B' : 'W';
    int dx[] = { -2, 2, 0, 0, -4, 4, 0, 0, -2, -2, 2, 2 };
    int dy[] = { 0, 0, -2, 2, 0, 0, -4, 4, 2, -2, 2, -2 };
    struct move move;



    struct move *moves =  queue_create(struct move, NULL, false);
    uint8_t **visited = malloc(sizeof(uint8_t*) * (2 * state->board_size - 1));
    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        visited[i] = bitset_create(2 * state->board_size - 1);

    queue_insert(moves, ((struct move){.move = PAWN_MOVE, 
                                       .player = player, 
                                       .pos = pos, 
                                       .value = 0 }));
    bitset_set(visited[pos.x], pos.y);
    while (queue_size(moves) > 0) {
        move = queue_head(moves);
		if (move.pos.x == target)
			break;
        
        for (size_t i = 0UL; i < ARRAY_SIZE(dx); ++i) {
            int x = move.pos.x + dx[i];
            int y = move.pos.y + dy[i];

            if (!is_valid_coord((struct point){x, y}, state->board_size)) 
				continue;
            if (bitset_check(visited[x], y)) 
				continue;

            if (is_valid_move(state, (struct point){ .x = x, .y = y }, move.pos, player)) {
                bitset_set(visited[x], y);
                queue_insert(moves, ((struct move){ .move = PAWN_MOVE, 
                                                    .player = player, 
                                                    .pos = {.x = x, .y = y}, 
                                                    .value  = move.value + 1 }));
            } else if (0UL <= i && i < 4UL && state->board[x][y] == op) { 
                state->board[x][y] = ' ';
                if (is_valid_move(state, (struct point){ .x = x, .y = y }, move.pos, player)) {
                    bitset_set(visited[x], y);
                    queue_insert(moves, ((struct move){ .move = PAWN_MOVE, 
                                                        .player = player, 
                                                        .pos = {.x = x, .y = y}, 
                                                        .value  = move.value + 1 }));
                }
                state->board[x][y] = op;
            }
        }
        queue_delete(moves);
    }


    if (move.pos.x == target && min_steps != NULL)
        *min_steps = move.value;

    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        bitset_destroy(visited[i]);
    free(visited);
    queue_destroy(moves);

    return move.pos.x == target;
}


bool is_valid_wall(struct state *state, struct move move) {
    if (move.move == HOR_WALL) {
        if (state->board[move.pos.x][move.pos.y] == '=' 
         || state->board[move.pos.x][move.pos.y + 1] == '=' 
         || state->board[move.pos.x][move.pos.y + 2] == '=')
            return false;

        if (state->board[move.pos.x - 1][move.pos.y + 1] == 'H'
         && state->board[move.pos.x + 1][move.pos.y + 1] == 'H'
         && state->board[move.pos.x][move.pos.y + 1] == 'H')
            return false;

    } else {
        if (state->board[move.pos.x][move.pos.y] == 'H' 
         || state->board[move.pos.x + 1][move.pos.y] == 'H' 
         || state->board[move.pos.x + 2][move.pos.y] == 'H')
            return false;

        if (state->board[move.pos.x + 1][move.pos.y - 1] == '='
         && state->board[move.pos.x + 1][move.pos.y + 1] == '='
         && state->board[move.pos.x + 1][move.pos.y] == '=')
            return false;
    }

    execute_action(state, move);

    bool result = path_exists(state, WHITE, NULL, state->white.win) && 
				  path_exists(state, BLACK, NULL, state->black.win);

    undo_action(state, move);

    return result;
}


struct move *get_legal_moves(struct state *state, enum player_t player, bool get_walls) {
    struct move *vec = vector_create(struct move, VEC_MIN_CAP, NULL, false);
    int dx[] = { -2, 2, 0, 0, -4, 4, 0, 0, -2, -2, 2, 2 };
    int dy[] = { 0, 0, -2, 2, 0, 0, -4, 4, 2, -2, 2, -2 };
    struct player player_ = player == BLACK ? state->black : state->white;
    
    
    for (size_t i = 0UL; i < ARRAY_SIZE(dx); ++i) {
        int x = player_.pos.x + dx[i];
        int y = player_.pos.y + dy[i];

        if (is_valid_coord((struct point){x, y}, state->board_size)
         && is_valid_move(state, (struct point){x, y}, player_.pos, player))
            vector_insert(vec, ((struct move){ .move = PAWN_MOVE, 
				                               .player = player,
                                               .pos = (struct point){x, y} }));
    }


    if (player_.walls == 0 || !get_walls)
        return vec;
    
    for (size_t i = 0UL; i <= 2UL * state->board_size - 4UL; i += 2UL) {
        for (size_t j = 1UL; j <= 2UL * state->board_size - 3UL; j += 2UL) {
            struct move move = { .move = VER_WALL, .player = player, .pos = { .x = i, .y = j } };
            if (is_valid_wall(state, move))
                vector_insert(vec, move);
        }
    }
        
    for (size_t i = 1UL; i <= 2UL * state->board_size - 3UL; i += 2UL) {
        for (size_t j = 0UL; j <= 2UL * state->board_size - 4UL; j += 2UL) {
            struct move move = { .move = HOR_WALL, .player = player, .pos = { .x = i, .y = j } };
            if (is_valid_wall(state, move))
                vector_insert(vec, move);
        }
    }
    return vec;
}


struct move decide_move(struct state *state, enum player_t player) {
    struct move best_move;
    int64_t start = current_time();
    for (uint32_t depth = 1; (depth <= MAX_DEPTH && !TIME_OUT(start)); ++depth) {
        struct move temp = minimax(state, start, player, (struct search_args){ .alpha = INT_MIN, 
                                                                               .beta = INT_MAX,
                                                                               .depth = depth,
                                                                               .is_maximizing = true,
                                                                               .player = player });
		if (temp.value != ABORT)
			memcpy(&best_move, &temp, sizeof(struct move));
    }
    return best_move;
}


static int evaluate(struct state *state, enum player_t maximizer) {
	uint32_t w_1 = state->board_size < state->max_walls
		? state->board_size * state->max_walls
		: state->board_size / 3 + (state->board_size % 3 != 0);
	
	uint32_t w_2 = state->board_size < state->max_walls
		? state->max_walls
		: 1;

	struct player max = maximizer == WHITE ? state->white : state->black;
	struct player min = maximizer == WHITE ? state->black : state->white;

	
	uint32_t my_dist;
    uint32_t op_dist;
    (void)path_exists(state, maximizer, &my_dist,
                      maximizer == WHITE ? 0 : 2 * state->board_size - 2);
	
    (void)path_exists(state, maximizer == BLACK ? WHITE : BLACK, 
                      &op_dist, maximizer == WHITE ? 2 * state->board_size - 2 : 0);
		

	int sum = w_1 * (op_dist - my_dist) + w_2 * (max.walls - min.walls) + 1;

	if (my_dist + state->board_size / 2 <= op_dist &&
		max.walls > 0 && my_dist < state->board_size / 4) {
		sum += op_dist / (state->board_size / 2);
	} else {
		if (maximizer == WHITE) {
			sum -= my_dist;
		} else {
			sum -= state->max_walls > state->board_size
				? my_dist < op_dist ? my_dist * state->board_size
				                    : my_dist / state->board_size
				: my_dist / state->board_size;
		}	
	}	

	if (op_dist <= 2 && max.walls > 0 && my_dist >= op_dist)
		sum += (op_dist - state->board_size) * (state->board_size / 2);  

	if (0 < max.walls && max.walls <= 2)
		sum += (op_dist - my_dist) * (state->board_size / 3);

	if (min.walls == 0 && max.walls >= 0)
		sum += maximizer == WHITE
			? state->board_size - my_dist
			: (state->black.pos.x - 
				state->board_size * state->board_size) / state->board_size;

	if (max.walls == 0)
		sum -= 4 * my_dist + 1;

	
	return sum;
}


static struct move minimax(struct state *state, int64_t start, 
                           enum player_t maximizer, struct search_args args) {
	if (TIME_OUT(start))
		return (struct move){ .value = ABORT };
	
    if (args.depth == 0 || is_terminal_state(state))
		return (struct move){ .value = evaluate(state, maximizer) };

    enum player_t next = args.player == BLACK ? WHITE : BLACK;
    struct point pos = args.player == BLACK ? state->black.pos : state->white.pos; 
    struct move best_move = { .value = args.is_maximizing ? INT_MIN : INT_MAX };
    struct move temp_move = {};
    struct move *vec = get_legal_moves(state, args.player, true);
    for (size_t i = 0UL; i < vector_size(vec); ++i) {
		if (TIME_OUT(start)) {
			temp_move.value = ABORT;
			break;
		}
		
        execute_action(state, vec[i]);

        temp_move = minimax(state, start, maximizer, (struct search_args){ .alpha = args.alpha, 
	                                                                       .beta = args.beta,
                                                                           .depth = args.depth - 1,
                                                                           .is_maximizing = !args.is_maximizing,
                                                                           .player = next });
		
        undo_action(state, (struct move){.move = vec[i].move, 
                                         .player = args.player,
                                         .pos = vec[i].move != PAWN_MOVE ? vec[i].pos : pos});
		
		if (temp_move.value == ABORT)
			break;

        if (args.is_maximizing && temp_move.value > best_move.value) {
            memcpy(&best_move, &vec[i], sizeof(struct move));
            best_move.value = temp_move.value;
            args.alpha = MAX(args.alpha, best_move.value);
        } else if (!args.is_maximizing && temp_move.value < best_move.value) {
            memcpy(&best_move, &vec[i], sizeof(struct move));    
            best_move.value = temp_move.value;
            args.beta = MIN(args.beta, best_move.value);
        }

        if (args.alpha >= args.beta) 
			break;
    }
    vector_destroy(vec);

    return temp_move.value == ABORT ? temp_move : best_move; 
}


bool is_valid_move(struct state *state, struct point new, struct point old, enum player_t player) {
    char op = player == WHITE ? 'B' : 'W';
    struct point enem = player == WHITE ? state->black.pos : state->white.pos;

    if (man_dist(new, old) == 2 && state->board[new.x][new.y] != op) { 
        return
            new.x == old.x - 2 ? state->board[old.x - 1][old.y] != '=' :
            new.x == old.x + 2 ? state->board[old.x + 1][old.y] != '=' :
            new.y == old.y + 2 ? state->board[old.x][old.y + 1] != 'H' :
            state->board[old.x][old.y - 1] != 'H';

    } else if (man_dist(new, old) == 4 && man_dist(old, enem) == 2) {
        if (new.x == old.x || new.y == old.y) {
            return
                new.x == old.x - 4 ? state->board[old.x - 2][old.y] == op &&
                                     state->board[old.x - 1][old.y] != '=' &&
                                     state->board[old.x - 3][old.y] != '='

            :   new.x == old.x + 4 ? state->board[old.x + 2][old.y] == op &&
                                     state->board[old.x + 1][old.y] != '=' &&
                                     state->board[old.x + 3][old.y] != '='
            
            :   new.y == old.y - 4 ? state->board[old.x][old.y - 2] == op &&
                                     state->board[old.x][old.y - 1] != 'H' &&
                                     state->board[old.x][old.y - 3] != 'H'

            :   state->board[old.x][old.y + 2] == op &&
                state->board[old.x][old.y + 1] != 'H' &&
                state->board[old.x][old.y + 3] != 'H';

        } else if (new.x == old.x - 2 && new.y == old.y + 2) {
            return
                old.x > enem.x ? state->board[old.x - 2][old.y] == op &&
                                 state->board[old.x - 1][old.y] != '=' &&
                                 state->board[old.x - 2][old.y + 1] != 'H' &&
                                (old.x - 3 == -1 || state->board[old.x - 3][old.y] == '=')
                    
            :   old.y < enem.y ? state->board[old.x][old.y + 2] == op &&
                                 state->board[old.x][old.y + 1] != 'H' &&
                                 state->board[old.x - 1][old.y + 2] != '=' &&
                                (old.y + 3 == 2 * state->board_size - 1 ||
                                 state->board[old.x][old.y + 3] == 'H')
            :   false;
        } else if (new.x == old.x - 2 && new.y == old.y - 2) { 
            return
                old.x > enem.x ? state->board[old.x - 2][old.y] == op &&
                                 state->board[old.x - 1][old.y] != '=' &&
                                 state->board[old.x - 2][old.y - 1] != 'H' &&
                                (old.x - 3 == -1 || state->board[old.x - 3][old.y] == '=')
                    
            :   old.y > enem.y ? state->board[old.x][old.y - 2] == op &&
                                 state->board[old.x][old.y - 1] != 'H' &&
                                 state->board[old.x - 1][old.y - 2] != '=' &&
                                (old.y - 3 == -1 || state->board[old.x][old.y - 3] == 'H')
            :   false;        
        } else if (new.x == old.x + 2 && new.y == old.y + 2) { 
            return
                old.x < enem.x ? state->board[old.x + 2][old.y] == op &&
                                 state->board[old.x + 1][old.y] != '=' &&
                                 state->board[old.x + 2][old.y + 1] != 'H' &&
                                (old.x + 3 == 2 * state->board_size - 1 ||
                                 state->board[old.x + 3][old.y] == '=')
                    
            :   old.y < enem.y ? state->board[old.x][old.y + 2] == op &&
                                 state->board[old.x][old.y + 1] != 'H' &&
                                 state->board[old.x + 1][old.y + 2] != '=' &&
                                (old.y + 3 == 2 * state->board_size - 1 ||
                                 state->board[old.x][old.y + 3] == 'H')
            :   false;
        } else if (new.x == old.x + 2 && new.y == old.y - 2) {
            return
                old.x < enem.x ? state->board[old.x + 2][old.y] == op &&
                                 state->board[old.x + 1][old.y] != '=' &&
                                 state->board[old.x + 2][old.y - 1] != 'H' &&
                                 (old.x + 3 == 2 * state->board_size - 1 ||
                                  state->board[old.x + 3][old.y] == '=')
                    
            :   old.y > enem.y ? state->board[old.x][old.y - 2] == op &&
                                 state->board[old.x][old.y - 1] != 'H' &&
                                 state->board[old.x + 1][old.y - 2] != '=' &&
                                 (old.y - 3 == -1 || state->board[old.x][old.y - 3] == 'H')
            :   false;
        } 
    }            
    return false;
}
