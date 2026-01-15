#include "movement.h"
#include "controls.h"
#include <math.h>
// Player radius in tile units (0.25 = 8 pixels / 32 pixels per tile)
#define PLAYER_RADIUS 0.25f

void movement_process_player(Player_t *player, uint8_t input, SMF_TileMap_t *map,
                    float delta_time) {
  float move_speed = player->speed * delta_time;
  float rot_speed = 3.0f * delta_time;

  if (input & INPUT_ROT_LEFT) {
    player->angle -= rot_speed;
  }
  if (input & INPUT_ROT_RIGHT) {
    player->angle += rot_speed;
  }

  float new_x = player->pos_x;
  float new_y = player->pos_y;

  if (input & INPUT_FORWARD) {
    new_x += cos(player->angle) * move_speed;
    new_y += sin(player->angle) * move_speed;
  }
  if (input & INPUT_BACKWARD) {
    new_x -= cos(player->angle) * move_speed;
    new_y -= sin(player->angle) * move_speed;
  }
  if (input & INPUT_LEFT) {
    new_x += cos(player->angle - 3.14159f / 2.0f) * move_speed;
    new_y += sin(player->angle - 3.14159f / 2.0f) * move_speed;
  }
  if (input & INPUT_RIGHT) {
    new_x += cos(player->angle + 3.14159f / 2.0f) * move_speed;
    new_y += sin(player->angle + 3.14159f / 2.0f) * move_speed;
  }
  int tile_y_min = (int)(player->pos_y - PLAYER_RADIUS);
  int tile_y_max = (int)(player->pos_y + PLAYER_RADIUS);

  int can_move_x = 1;
  for (int ty = tile_y_min; ty <= tile_y_max; ty++) {
    if (ty >= 0 && ty < map->height) {
      int tile_x1 = (int)(new_x - PLAYER_RADIUS);
      int tile_x2 = (int)(new_x + PLAYER_RADIUS);

      if (tile_x1 >= 0 && tile_x1 < map->width &&
          map->tiles[ty][tile_x1].tile_id != 0) {
        can_move_x = 0;
        break;
      }
      if (tile_x2 >= 0 && tile_x2 < map->width &&
          map->tiles[ty][tile_x2].tile_id != 0) {
        can_move_x = 0;
        break;
      }
    }
  }
  if (can_move_x) {
    player->pos_x = new_x;
  }

  int tile_x_min = (int)(player->pos_x - PLAYER_RADIUS);
  int tile_x_max = (int)(player->pos_x + PLAYER_RADIUS);

  int can_move_y = 1;
  for (int tx = tile_x_min; tx <= tile_x_max; tx++) {
    if (tx >= 0 && tx < map->width) {
      int tile_y1 = (int)(new_y - PLAYER_RADIUS);
      int tile_y2 = (int)(new_y + PLAYER_RADIUS);

      if (tile_y1 >= 0 && tile_y1 < map->height &&
          map->tiles[tile_y1][tx].tile_id != 0) {
        can_move_y = 0;
        break;
      }
      if (tile_y2 >= 0 && tile_y2 < map->height &&
          map->tiles[tile_y2][tx].tile_id != 0) {
        can_move_y = 0;
        break;
      }
    }
  }

  if (can_move_y) {
    player->pos_y = new_y;
  }
}
