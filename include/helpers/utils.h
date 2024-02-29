#ifndef UTILS_H
#define UTILS_H

#include "quoridor_types.h"

void read_line(char **line, FILE *stream);

void initialize_board(struct state *state);

struct state *state_create(uint32_t size);

void state_destroy(struct state *state);

bool is_valid_number(const char *s, uint32_t *num);

bool is_terminal_state(struct state *state);

bool is_valid_size(uint32_t size);

bool is_valid_coord(struct point pos, uint32_t size);

uint32_t man_dist(struct point a, struct point b);

void execute_action(struct state *state, struct move *move);

void undo_action(struct state *state, struct move *move);

struct move *create_move(struct move move);

int64_t current_time(void);

#endif /* UTILS_H */
