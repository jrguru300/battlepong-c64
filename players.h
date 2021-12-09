#ifndef _battlepong_players_h_
#define _battlepong_players_h_

typedef struct player
{
  unsigned char joy;
  unsigned char last_joy;
  unsigned char id;
  unsigned char life;
  unsigned char last_life;
  unsigned char score;
  int pos_x;
  unsigned char pos_y;
};

#endif
