#include "gamemap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// for reference SMF stands for Simple Map Format

static char *map_read_file(const char *filename, size_t *out_size) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Error: Could not open file %s\n", filename);
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = (char *)malloc(size + 1);
  if (!buffer) {
    fclose(f);
    return NULL;
  }

  fread(buffer, 1, size, f);
  buffer[size] = '\0';
  fclose(f);

  if (out_size)
    *out_size = size;
  return buffer;
}

static int map_parse_key_value(const char *line, const char *key, int *value) {
  char key_buf[64];
  int val;
  if (sscanf(line, "%63[^=]=%d", key_buf, &val) == 2) {
    if (strcmp(key_buf, key) == 0) {
      *value = val;
      return 1;
    }
  }
  return 0;
}

SMF_TileMap_t *map_parse_tilemap(const char *filename) {
  size_t file_size;
  char *buffer = map_read_file(filename, &file_size);
  if (!buffer)
    return NULL;
  SMF_TileMap_t *map = (SMF_TileMap_t *)calloc(1, sizeof(SMF_TileMap_t));
  if (!map) {
    free(buffer);
    return NULL;
  }
  char *line = strtok(buffer, "\n");
  while (line) {
    if (line[0] == '#' || line[0] == '\0' || line[0] == '\r') {
      line = strtok(NULL, "\n");
      continue;
    }
    if (strstr(line, "[MAP]")) {
      line = strtok(NULL, "\n");
      continue;
    }
    if (map_parse_key_value(line, "width", &map->width) ||
        map_parse_key_value(line, "height", &map->height)) {
      line = strtok(NULL, "\n");
      continue;
    }
    if (map->width > 0 && map->height > 0 && map->tiles == NULL) {
      map->tiles = (SMF_Tile_t **)calloc(map->height, sizeof(SMF_Tile_t *));
      for (int y = 0; y < map->height; y++) {
        map->tiles[y] = (SMF_Tile_t *)calloc(map->width, sizeof(SMF_Tile_t));
      }
    }
    if (strstr(line, "[TILE_")) {
      int x, y;
      if (sscanf(line, "[TILE_%d_%d]", &x, &y) == 2) {
        if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
          SMF_Tile_t *tile = &map->tiles[y][x];
          for (int i = 0; i < 4; i++) {
            line = strtok(NULL, "\n");
            if (!line)
              break;
            map_parse_key_value(line, "id", &tile->tile_id);
            map_parse_key_value(line, "floor", &tile->height_floor);
            map_parse_key_value(line, "ceiling", &tile->height_ceiling);
            map_parse_key_value(line, "texture", &tile->texture_id);
          }
        }
      }
    }
    if (strstr(line, "[OBJECTS]")) {
      break;
    }
    line = strtok(NULL, "\n");
  }
  free(buffer);
  printf("Loaded tilemap: %dx%d\n", map->width, map->height);
  return map;
}

void map_free_tilemap(SMF_TileMap_t *map) {
  if (!map)
    return;
  if (map->tiles) {
    for (int y = 0; y < map->height; y++) {
      free(map->tiles[y]);
    }
    free(map->tiles);
  }
  if (map->objects) {
    free(map->objects);
  }
  free(map);
}
