#ifndef QUORIDOR_TYPES_H
#define QUORIDOR_TYPES_H

#include "common_types.h"
#include "vector.h"



enum player_t {
    NONE,
    BLACK,
    WHITE
};


enum move_t {
    HOR_WALL,
    VER_WALL,
    PAWN_MOVE
};


struct point {
    int x;
    int y;
};


struct player {
    struct point pos;
    uint32_t walls;
    uint32_t win;
};


struct move {
    enum player_t player;
    enum move_t move;
    struct point pos;
    int value;
};


struct state {
    char **board;
    uint32_t board_size;
    uint32_t max_walls;
    enum player_t winner;
    struct move *history;
    struct player white;
    struct player black;
};


#endif /* QUORIDOR_TYPES_H */