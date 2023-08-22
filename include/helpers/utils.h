#ifndef UTILS_H
#define UTILS_H

#include "quoridor_types.h"

void read_line(char **line, FILE *stream);

void initialize_board(State *state);

State *state_create(uint32_t size);

void state_destroy(State *state);

bool is_valid_number(const char *s, uint32_t *num);

bool is_terminal_state(State *state);

bool is_valid_size(uint32_t size);

bool is_valid_coord(Position pos, uint32_t size);

uint32_t man_dist(Position a, Position b);

void execute_action(State *state, Move *move);

void undo_action(State *state, Move *move);

Move *create_move(Move move);

int64_t current_time(void);

#endif /* UTILS_H */
