#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>

#define MAX_PLAYERS 4

#define INPUT_FORWARD (1 << 0)
#define INPUT_BACKWARD (1 << 1)
#define INPUT_LEFT (1 << 2)
#define INPUT_RIGHT (1 << 3)
#define INPUT_ROT_LEFT (1 << 4)
#define INPUT_ROT_RIGHT (1 << 5)

extern uint8_t g_player_input[MAX_PLAYERS];

void controls_handle_input(int player_id);
void controls_clear_input(int player_id);

#endif
