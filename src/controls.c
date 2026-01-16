#include "controls.h"
#include <raylib.h>

uint8_t g_player_input[MAX_PLAYERS] = {0};

void controls_handle_input(int player_id) {
  if (player_id < 0 || player_id >= MAX_PLAYERS)
    return;

  g_player_input[player_id] = 0;

  if (IsKeyDown(KEY_W))
    g_player_input[player_id] |= INPUT_FORWARD;
  if (IsKeyDown(KEY_S))
    g_player_input[player_id] |= INPUT_BACKWARD;
  if (IsKeyDown(KEY_A))
    g_player_input[player_id] |= INPUT_LEFT;
  if (IsKeyDown(KEY_D))
    g_player_input[player_id] |= INPUT_RIGHT;
  if (IsKeyDown(KEY_LEFT))
    g_player_input[player_id] |= INPUT_ROT_LEFT;
  if (IsKeyDown(KEY_RIGHT))
    g_player_input[player_id] |= INPUT_ROT_RIGHT;
}

void controls_clear_input(int player_id) {
  if (player_id < 0 || player_id >= MAX_PLAYERS)
    return;
  g_player_input[player_id] = 0;
}
