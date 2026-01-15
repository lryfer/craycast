#include "controls.h"
#include "gamemap.h"
#include "movement.h"
#include "player.h"
#include <math.h>
#include <raylib.h>
#include <stdint.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "raycaster name still to be chosen"
// remove this if you want 3d
#define DEBUG2D
void gameloop(void);
void gamedraw(void);
void gamerender(void);
void render_3d_view(int offset_x, int view_width, int view_height);
void render_2d_view(int offset_x, int view_width, int view_height);

int16_t g_game_fps = 60;
uint8_t g_game_fov = 100;
int g_game_num_rays = WINDOW_WIDTH;
float g_game_step_size = 0.05f;
SMF_TileMap_t *g_game_tilemap;
Player_t g_game_players[MAX_PLAYERS];
int g_game_num_players = 1;
_Bool g_game_running = true;

int main(int argc, char **argv) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
  SetTargetFPS(g_game_fps);
  // will be argv in the future
  g_game_tilemap = map_parse_tilemap("./TILEMAP.SMF");

  // init players
  g_game_players[0] = player_init(5.0f, 6.0f, 0.0f);

  while (g_game_running) {
    gameloop();
    if (WindowShouldClose()) {
      g_game_running = false;
      map_free_tilemap(g_game_tilemap);
    }
  }
  CloseWindow();
  return 0;
}

void gameloop(void) {
  float delta_time = GetFrameTime();

  for (int i = 0; i < g_game_num_players; i++) {
    // singleplayer
    if (i == 0) {
      controls_handle_input(g_game_players[i].id);
    }
    // (local + network in future)
    movement_process_player(&g_game_players[i], g_player_input[g_game_players[i].id],
                   g_game_tilemap, delta_time);
  }

  gamedraw();
}
void gamedraw(void) {
  BeginDrawing();
  gamerender();
  EndDrawing();
}
void gamerender(void) {
  ClearBackground(BLACK);

#ifdef DEBUG2D
  // Split-screen mode: 2D on left, 3D on right
  int split_x = WINDOW_WIDTH / 2;
  render_2d_view(0, split_x, WINDOW_HEIGHT);
  render_3d_view(split_x, split_x, WINDOW_HEIGHT);

  // Draw separator line
  DrawLine(split_x, 0, split_x, WINDOW_HEIGHT, YELLOW);
#else
  // Full-screen 3D mode
  render_3d_view(0, WINDOW_WIDTH, WINDOW_HEIGHT);
#endif
}

void render_2d_view(int offset_x, int view_width, int view_height) {
  int32_t tile_size = 32;

  for (uint32_t y = 0; y < g_game_tilemap->height; y++) {
    for (uint32_t x = 0; x < g_game_tilemap->width; x++) {
      SMF_Tile_t *tile = &g_game_tilemap->tiles[y][x];
      int screen_position_x = offset_x + x * tile_size;
      int screen_position_y = y * tile_size;

      Color tile_color = tile->tile_id == 0 ? DARKGRAY : WHITE;
      DrawRectangle(screen_position_x, screen_position_y, tile_size, tile_size,
                    tile_color);
    }
  }

  for (int i = 0; i < g_game_num_players; i++) {
    int player_screen_x = offset_x + (int)(g_game_players[i].pos_x * tile_size);
    int player_screen_y = (int)(g_game_players[i].pos_y * tile_size);

    Color player_color = (i == 0) ? RED : BLUE;
    DrawCircle(player_screen_x, player_screen_y, 8, player_color);

    DrawLine(player_screen_x, player_screen_y,
             player_screen_x + (int)(cosf(g_game_players[i].angle) * 20),
             player_screen_y + (int)(sinf(g_game_players[i].angle) * 20),
             YELLOW);

    // Draw rays
    float fov_rad = (g_game_fov * 3.14159f) / 180.0f;

    for (int ray = 0; ray < g_game_num_rays; ray++) {
      float ray_angle = g_game_players[i].angle - fov_rad / 2.0f +
                        (ray * fov_rad / g_game_num_rays);

      float ray_dir_x = cosf(ray_angle);
      float ray_dir_y = sinf(ray_angle);

      float ray_x = g_game_players[i].pos_x;
      float ray_y = g_game_players[i].pos_y;
      while (true) {
        ray_x += ray_dir_x * g_game_step_size;
        ray_y += ray_dir_y * g_game_step_size;

        int tile_x = (int)ray_x;
        int tile_y = (int)ray_y;

        if (tile_x < 0 || tile_x >= g_game_tilemap->width || tile_y < 0 ||
            tile_y >= g_game_tilemap->height) {
          break;
        }

        if (g_game_tilemap->tiles[tile_y][tile_x].tile_id != 0) {
          int hit_x = offset_x + (int)(ray_x * tile_size);
          int hit_y = (int)(ray_y * tile_size);
          DrawLine(player_screen_x, player_screen_y, hit_x, hit_y, GREEN);
          break;
        }
      }
    }
  }
}

void render_3d_view(int offset_x, int view_width, int view_height) {
  // Draw sky and floor might refactor later(9 january 2026)
  DrawRectangle(offset_x, 0, view_width, view_height / 2,
                (Color){100, 150, 200, 255});
  DrawRectangle(offset_x, view_height / 2, view_width, view_height / 2,
                (Color){80, 80, 80, 255});

  Player_t *player = &g_game_players[0];

  float fov_rad = (g_game_fov * 3.14159f) / 180.0f;

  for (int ray = 0; ray < g_game_num_rays; ray++) {
    float ray_angle =
        player->angle - fov_rad / 2.0f + (ray * fov_rad / g_game_num_rays);

    float ray_dir_x = cosf(ray_angle);
    float ray_dir_y = sinf(ray_angle);

    float ray_x = player->pos_x;
    float ray_y = player->pos_y;
    float distance = 0.0f;

    while (true) {
      ray_x += ray_dir_x * g_game_step_size;
      ray_y += ray_dir_y * g_game_step_size;
      distance += g_game_step_size;

      int tile_x = (int)ray_x;
      int tile_y = (int)ray_y;

      if (tile_x < 0 || tile_x >= g_game_tilemap->width || tile_y < 0 ||
          tile_y >= g_game_tilemap->height) {
        break;
      }

      if (g_game_tilemap->tiles[tile_y][tile_x].tile_id != 0) {
        // Fish-eye correction which works like shit
        float corrected_distance = distance * cosf(ray_angle - player->angle);
        float wall_height = (view_height / corrected_distance) * 0.5f;
        int wall_top = (view_height / 2) - (int)(wall_height / 2);
        int wall_bottom = (view_height / 2) + (int)(wall_height / 2);
        int column_x = offset_x + (ray * view_width) / g_game_num_rays;
        int column_width = (view_width / g_game_num_rays) + 1;
        int brightness = (int)(255.0f / (1.0f + corrected_distance * 0.3f));
        brightness = brightness > 255 ? 255 : (brightness < 0 ? 0 : brightness);

        Color wall_color;
        switch (g_game_tilemap->tiles[tile_y][tile_x].tile_id) {
        case 1:
          wall_color = (Color){brightness, 0, 0, 255}; // Red
          break;
        case 2:
          wall_color = (Color){0, brightness, 0, 255}; // Green
          break;
        case 3:
          wall_color = (Color){0, 0, brightness, 255}; // Blue
          break;
        default:
          wall_color =
              (Color){brightness, brightness, brightness, 255}; // White
          break;
        }

        // TODO: Texture mapping - currently using solid colors
        // for texture mapping we need:
        // calculate hit_offset = where the ray hits on the tile (0.0 - 1.0)
        // texture_x = (int)(hit_offset * texture_width)

        // Draw wall slice
        DrawRectangle(column_x, wall_top, column_width, wall_bottom - wall_top,
                      wall_color);
        break;
      }
    }
  }

  // Draw FPS and info
  DrawText(TextFormat("FPS: %d", GetFPS()), offset_x + 10, 10, 20, YELLOW);
  DrawText(TextFormat("Pos: %.1f, %.1f", player->pos_x, player->pos_y),
           offset_x + 10, 35, 20, YELLOW);
  DrawText(TextFormat("Angle: %.1f", player->angle * 180.0f / 3.14159f),
           offset_x + 10, 60, 20, YELLOW);
}
