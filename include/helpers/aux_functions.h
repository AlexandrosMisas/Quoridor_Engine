#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H

#include "quoridor_types.h"
#include "utils.h"

#define TIME_LIMIT 4500
#define MAX_DEPTH  8
#define ABORT     (INT_MAX / 4)


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



bool is_valid_move(struct state *state, struct point new, struct point old, enum player_t player);

struct move *get_legal_moves(struct state *state, enum player_t player, bool get_walls);

bool is_valid_wall(struct state *state, struct move move);

bool path_exists(struct state *state, enum player_t player, uint32_t *min_steps, uint32_t target);

struct move decide_move(struct state *state, enum player_t player);


#endif /* AUX_FUNCTIONS_H */

