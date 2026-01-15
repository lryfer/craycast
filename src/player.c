#include "player.h"
static int player_current_id = 0;
Player_t player_init(float x, float y, float angle) {
  Player_t player;
  player.id = player_current_id;
  player_current_id += 1;
  player.pos_x = x;
  player.pos_y = y;
  player.angle = angle;
  player.speed = 3.0f;
  player.health = 100;
  return player;
}
