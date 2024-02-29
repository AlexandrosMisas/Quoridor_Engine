#ifndef QUORIDOR_FUNCTIONS_H
#define QUORIDOR_FUNCTIONS_H

#include "quoridor_types.h"

void known_command(const char **commands, uint32_t size, char *search);

void list_commands(const char **commands, uint32_t size);

void showboard(struct state *state);

void clear_board(struct state *state);

void boardsize(struct state *state, const char *size);

void walls(struct state *state, const char *num);

void undo(struct state *state, const char *times);

void boardsize(struct state *state, const char *size);

void playmove(struct state *state, char *player, char *pos);

void playwall(struct state *state, char *player, char *move, char *wall_type);

void playwall(struct state *state, char *player, char *pos, char *wall_type);

void genmove(struct state *state, char *player);

#endif /* QUORIDOR_FUNCTIONS_H */

