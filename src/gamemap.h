#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <stdint.h>

typedef struct {
  int32_t tile_id;
  int32_t height_floor;
  int32_t height_ceiling;
  int32_t texture_id;
} SMF_Tile_t;

typedef struct {
  int32_t pos_x, pos_y;
  int32_t objtype;
  float z;
} SMF_Object_t;

typedef struct {
  int32_t width, height;
  SMF_Tile_t **tiles;
  SMF_Object_t *objects;
  int32_t object_count;
} SMF_TileMap_t;

// Map parser functions
SMF_TileMap_t* map_parse_tilemap(const char *filename);
void map_free_tilemap(SMF_TileMap_t *map);

#endif
