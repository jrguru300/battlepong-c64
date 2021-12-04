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
  COLOR_DARKGREY      11
  COLOR_GREY          12
  COLOR_LIGHTGREEN    13
  COLOR_LIGHTBLUE     14
  COLOR_LIGHTGREY     15
*/

/* https://www.c64-wiki.com/wiki/Screen_RAM */
#define VRAM_START          0xD000
#define RASTER_ADDRESS      0xD012
#define BORDER_COLOR        0xD020
#define BACKGROUND_COLOR    0xD021
#define STRETCH_SPRITE_V    0xD017
#define STRETCH_SPRITE_H    0xD01D
#define SPRITE_COLOR_START  0xD027
#define SPRITE_COMMON_C_1   0xD025
#define SPRITE_COMMON_C_2   0xD026
#define SPRITE_MODE         0xD01C
#define SPRITE_POINT_START  0X07F8
#define SPRITE_BLOCK_SIZE   64
#define SPRITE_SIZE 	      5*12

unsigned char get_raster    (void);
void clear_screen           (void);
void set_background_color   (unsigned char color);
void set_border_color       (unsigned char color);
void set_text_color         (unsigned char color);
void set_sprite_mode_mask   (unsigned char mask);
void set_sprite_color       (unsigned char spriteno, unsigned char color);
void set_sprite_color_0     (unsigned char color);
void set_sprite_color_1     (unsigned char color);
void stretch_sprites        (unsigned char h_mask, unsigned char v_mask);
void load_sprite_to_block   (unsigned char *sprite, unsigned char blockno);
void set_sprite_from_block  (unsigned char spriteno, unsigned char blockno);
void set_sprite_enable_mask (unsigned char mask);
void set_sprite_coordinates (unsigned char spriteno, unsigned char pos_x, unsigned char pos_y);

#endif
