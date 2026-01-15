#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
  int id;
  float pos_x;
  float pos_y;
  float angle;
  float speed;
  int health;
} Player_t;

Player_t player_init(float x, float y, float angle);

#endif
