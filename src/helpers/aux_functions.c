#include "aux_functions.h"
#include "queue.h"


typedef struct {
    int alpha;
    int beta;
    int depth;
    player_t player;
    bool is_maximizing;
} SearchArgs;


static Move minimax(State *state, int64_t start, player_t maximizer, SearchArgs args);
static int evaluate(State *state, player_t maximizer);



bool path_exists(State *state, player_t player, uint32_t *min_steps, uint32_t target) {
    Position pos = player == WHITE ? state->white.pos : state->black.pos;
    char op = player == WHITE ? 'B' : 'W';
    int dx[] = { -2, 2, 0, 0, -4, 4, 0, 0, -2, -2, 2, 2 };
    int dy[] = { 0, 0, -2, 2, 0, 0, -4, 4, 2, -2, 2, -2 };
    Move *move;


    Queue *moves = queue_create(free);
    bool **visited = malloc(sizeof(bool*) * (2 * state->board_size - 1));
    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        visited[i] = calloc(2 * state->board_size - 1, sizeof(bool));

    queue_insert(moves, create_move((Move){.move = PAWN_MOVE, 
										   .player = player, 
		                                   .pos = pos, 
		                                   .value = 0 }));
    visited[pos.x][pos.y] = true;
    while (queue_size(moves) > 0) {
        move = queue_value(queue_head(moves));
		if (move->pos.x == target)
			break;
        
        for (size_t i = 0UL; i < ARRAY_SIZE(dx); ++i) {
            int x = move->pos.x + dx[i];
            int y = move->pos.y + dy[i];

            if (!is_valid_coord((Position){x, y}, state->board_size)) 
				continue;
            if (visited[x][y]) 
				continue;

            if (is_valid_move(state, (Position){ .x = x, .y = y }, move->pos, player)) {
                visited[x][y] = true;
                queue_insert(moves, create_move((Move){ .move = PAWN_MOVE, 
                                                        .player = player, 
                                                        .pos = {.x = x, .y = y}, 
                                                        .value  = move->value + 1 }));
            } else if (0UL <= i && i < 4UL && state->board[x][y] == op) { 
                state->board[x][y] = ' ';
                if (is_valid_move(state, (Position){ .x = x, .y = y }, move->pos, player)) {
                    visited[x][y] = true;
                    queue_insert(moves, create_move((Move){ .move = PAWN_MOVE, 
                                                            .player = player, 
                                                            .pos = {.x = x, .y = y}, 
                                                            .value  = move->value + 1 }));
                }
                state->board[x][y] = op;
            }
        }
        queue_delete(moves);
    }

	bool result = move->pos.x == target;
    if (result && min_steps != NULL)
        *min_steps = move->value;

    for (size_t i = 0UL; i < 2 * state->board_size - 1UL; ++i)
        free(visited[i]);
    free(visited);
    queue_destroy(moves);

    return result;
}


bool is_valid_wall(State *state, Move move) {
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

    execute_action(state, &move);

    bool result = path_exists(state, WHITE, NULL, 0) && 
				  path_exists(state, BLACK, NULL, 2 * state->board_size - 2);

    undo_action(state, &move);

    return result;
}


Vector *get_legal_moves(State *state, player_t player, bool get_walls) {
    Vector *vec = vector_create(free);
    int dx[] = { -2, 2, 0, 0, -4, 4, 0, 0, -2, -2, 2, 2 };
    int dy[] = { 0, 0, -2, 2, 0, 0, -4, 4, 2, -2, 2, -2 };
    Player player_ = player == BLACK ? state->black : state->white;
    
    for (size_t i = 0UL; i < ARRAY_SIZE(dx); ++i) {
        int x = player_.pos.x + dx[i];
        int y = player_.pos.y + dy[i];

        if (is_valid_coord((Position){x, y}, state->board_size)
         && is_valid_move(state, (Position){x, y}, player_.pos, player))
            vector_insert(vec, create_move((Move){ .move = PAWN_MOVE, 
				                                   .player = player,
                                                   .pos = (Position){x, y} }));
    }


    if (player_.walls == 0 || !get_walls)
        return vec;
    
    for (size_t i = 0UL; i <= 2UL * state->board_size - 4UL; i += 2UL) {
        for (size_t j = 1UL; j <= 2UL * state->board_size - 3UL; j += 2UL) {
            Move move = { .move = VER_WALL, .player = player, .pos = { .x = i, .y = j } };
            if (is_valid_wall(state, move))
                vector_insert(vec, create_move(move));
        }
    }
        
    for (size_t i = 1UL; i <= 2UL * state->board_size - 3UL; i += 2UL) {
        for (size_t j = 0UL; j <= 2UL * state->board_size - 4UL; j += 2UL) {
            Move move = { .move = HOR_WALL, .player = player, .pos = { .x = i, .y = j } };
            if (is_valid_wall(state, move))
                vector_insert(vec, create_move(move));
        }
    }
    return vec;
}


Move decide_move(State *state, player_t player) {
    Move best_move;
    int64_t start = current_time();
    for (uint32_t depth = 1; (depth <= MAX_DEPTH && !TIME_OUT(start)); depth++) {
        Move temp = minimax(state, start, player, (SearchArgs){ .alpha = INT_MIN, 
                                                                .beta = INT_MAX,
                                                                .depth = depth,
                                                                .is_maximizing = true,
                                                                .player = player });
		if (temp.value != ABORT)
			memcpy(&best_move, &temp, sizeof(Move));

    }
    return best_move;
}


static int evaluate(State *state, player_t maximizer) {
	uint32_t w_1 = state->board_size < state->max_walls
		? state->board_size * state->max_walls
		: state->board_size / 3 + (state->board_size % 3 != 0);
	
	uint32_t w_2 = state->board_size < state->max_walls
		? state->max_walls
		: 1;

	Player max = maximizer == WHITE ? state->white : state->black;
	Player min = maximizer == WHITE ? state->black : state->white;

	
	uint32_t my_dist, op_dist;
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


static Move minimax(State *state, int64_t start, player_t maximizer, SearchArgs args) {
	if (TIME_OUT(start))
		return (Move){ .value = ABORT };
	
    if (args.depth == 0 || is_terminal_state(state))
		return (Move){ .value = evaluate(state, maximizer) };

    player_t next = args.player == BLACK ? WHITE : BLACK;
    Position pos = args.player == BLACK ? state->black.pos : state->white.pos; 
    Move best_move = { .value = args.is_maximizing ? INT_MIN : INT_MAX }, temp_move = {};
    Vector *vec = get_legal_moves(state, args.player, true);
    for (size_t i = 0UL; i < vector_size(vec); ++i) {
		if (TIME_OUT(start)) {
			temp_move.value = ABORT;
			break;
		}
		
        Move *test = vector_get(vec, i);
        execute_action(state, test);

        temp_move = minimax(state, start, maximizer, (SearchArgs){ .alpha = args.alpha, 
	                                                               .beta = args.beta,
                                                                   .depth = args.depth - 1,
                                                                   .is_maximizing = !args.is_maximizing,
                                                                   .player = next });
		
        undo_action(state, &(Move){.move = test->move, .player = args.player,
                .pos = test->move != PAWN_MOVE ? test->pos : pos});
		
		if (temp_move.value == ABORT)
			break;

        if (args.is_maximizing && temp_move.value > best_move.value) {
            memcpy(&best_move, test, sizeof(Move));
            best_move.value = temp_move.value;
            args.alpha = MAX(args.alpha, best_move.value);
        } else if (!args.is_maximizing && temp_move.value < best_move.value) {
            memcpy(&best_move, test, sizeof(Move));    
            best_move.value = temp_move.value;
            args.beta = MIN(args.beta, best_move.value);
        }

        if (args.alpha >= args.beta) 
			break;
    }
    vector_destroy(vec);

    return temp_move.value == ABORT ? temp_move : best_move; 
}



bool is_valid_move(State *state, Position new, Position old, player_t player) {
    char op = player == WHITE ? 'B' : 'W';
    Position enem = player == WHITE ? state->black.pos : state->white.pos;

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
