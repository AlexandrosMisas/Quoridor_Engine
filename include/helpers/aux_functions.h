#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H

#include "quoridor_types.h"
#include "utils.h"

#define TIME_LIMIT 4500
#define MAX_DEPTH 8
#define ABORT (INT_MAX / 4)


#define TIME_OUT(start) ({							\
    int64_t diff = ((current_time()) - (start));	\
    diff >= TIME_LIMIT;								\
})


#define MAX(a, b) ({        \
    int a_ = (a);           \
    int b_ = (b);           \
    a_ > b_ ? a_ : b_;      \
})


#define MIN(a, b) ({        \
    int a_ = (a);           \
    int b_ = (b);           \
    a_ > b_ ? b_ : a_;      \
})



bool is_valid_move(State *state, Position new, Position old, player_t player);

Vector *get_legal_moves(State *state, player_t player, bool get_walls);

bool is_valid_wall(State *state, Move move);

bool path_exists(State *state, player_t player, uint32_t *min_steps, uint32_t target);

Move decide_move(State *state, player_t player);


#endif /* AUX_FUNCTIONS_H */

