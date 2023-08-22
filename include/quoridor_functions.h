#ifndef QUORIDOR_FUNCTIONS_H
#define QUORIDOR_FUNCTIONS_H

#include "quoridor_types.h"

void known_command(const char **commands, uint32_t size, char *search);

void list_commands(const char **commands, uint32_t size);

void showboard(State *state);

void clear_board(State *state);

void boardsize(State *state, const char *size);

void walls(State *state, const char *num);

void undo(State *state, const char *times);

void boardsize(State *state, const char *size);

void playmove(State *state, char *player, char *pos);

void playwall(State *state, char *player, char *move, char *wall_type);

void playwall(State *state, char *player, char *pos, char *wall_type);

void genmove(State *state, char *player);

#endif /* QUORIDOR_FUNCTIONS_H */

