#ifndef QUORIDOR_TYPES_H
#define QUORIDOR_TYPES_H

#include "common_types.h"
#include "vector.h"

typedef enum {
    NONE,
    BLACK,
    WHITE
} player_t;

typedef enum {
    HOR_WALL,
    VER_WALL,
    PAWN_MOVE
} move_t;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    uint32_t walls;
    uint32_t win;
} Player;

typedef struct {
    player_t player;
    move_t move;
    Position pos;
    int value;
} Move;


typedef struct {
    char **board;
    uint32_t board_size;
    uint32_t max_walls;
    player_t winner;
    Vector *history;
    Player white;
    Player black;
} State;


#endif /* QUORIDOR_TYPES_H */

