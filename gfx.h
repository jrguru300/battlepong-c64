#ifndef _battlepong_gfx_h_
#define _battlepong_gfx_h_

/*
  COLOR_BLACK         0
  COLOR_WHITE         1
  COLOR_RED           2
  COLOR_CYAN          3
  COLOR_MAGENTA       4
  COLOR_GREEN         5
  COLOR_BLUE          6
  COLOR_YELLOW        7
  COLOR_ORANGE        8
  COLOR_BROWN         9
  COLOR_PINK          10
  COLOR_DARK_GREY     11
  COLOR_GREY          12
  COLOR_LIGHT_GREEN   13
  COLOR_LIGHT_BLUE    14
  COLOR_LIGHT_GREY    15
*/

/* https://www.c64-wiki.com/wiki/Screen_RAM */
#define VRAM_START       0xD000
#define BORDER_COLOR     0xD020
#define BACKGROUND_COLOR 0xD021

#define SPRITE_SIZE 	   5*12

void clear_screen           (void);
void set_background_color   (unsigned char color);
void set_border_color       (unsigned char color);
void set_text_color         (unsigned char color);
void set_sprite_mode_mask   (unsigned char mask);
void set_sprite_color_m0    (unsigned char color);
void set_sprite_color_m1    (unsigned char color);
void load_sprite_to_block   (unsigned char *sprite, unsigned char blockno);
void set_sprite_from_block  (unsigned char spriteno, unsigned char blockno);
void set_sprite_enable_mask (unsigned char mask);
void set_sprite_coordinates (unsigned char spriteno, unsigned char pos_x, unsigned char pos_y);

#endif
