#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "player.h"
#include "gamemap.h"
#include <stdint.h>

void movement_process_player(Player_t *player, uint8_t input, SMF_TileMap_t *map, float delta_time);

#endif
