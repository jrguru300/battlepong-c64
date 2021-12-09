#ifndef _battlepong_gfx_h_
#define _battlepong_gfx_h_

#include <stdbool.h>

// #define COLOR_BLACK         0
// #define COLOR_WHITE         1
// #define COLOR_RED           2
// #define COLOR_CYAN          3
// #define COLOR_MAGENTA       4
// #define COLOR_GREEN         5
// #define COLOR_BLUE          6
// #define COLOR_YELLOW        7
// #define COLOR_ORANGE        8
// #define COLOR_BROWN         9

#define COLOR_PINK          10
#define COLOR_DARK_GREY     11
#define COLOR_GREY          12
#define COLOR_LIGHT_GREEN   13
#define COLOR_LIGHT_BLUE    14
#define COLOR_LIGHT_GREY    15

/* https://www.c64-wiki.com/wiki/Screen_RAM */

#define VRAM_START          0xD000
#define RASTER_ADDRESS      0xD012
#define BORDER_COLOR        0xD020
#define BACKGROUND_COLOR    0xD021
#define SPRITES_ENABLED     0xD015
#define CHAR_MULTICOLOR     0xD016
#define CHAR_COMMON_C_1     0xD022
#define CHAR_COMMON_C_2     0xD023
#define CHARACTER_SET       0xD018
#define STRETCH_SPRITE_V    0xD017
#define STRETCH_SPRITE_H    0xD01D
#define SPRITE_X_COORD_MSB  0xD010
#define SPRITE_PRIORITY     0xD01B
#define SPRITE_COLOR_START  0xD027
#define SPRITE_COMMON_C_1   0xD025
#define SPRITE_COMMON_C_2   0xD026
#define SPRITE_MODE         0xD01C
#define SPRITE_POINT_START  0X07F8
#define SPRITE_SP_COLLISON  0XD01E
#define SPRITE_BG_COLLISION 0xD01F
#define SPRITE_BLOCK_SIZE   64
#define SPRITE_SIZE 	      5*12

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

void raster_wait            (unsigned char line);
void clear_screen           (void);
void set_background_color   (unsigned char color);
void set_border_color       (unsigned char color);
void set_text_color         (unsigned char color);
void set_sprite_mode_mask   (unsigned char mask);
void set_sprite_color       (unsigned char spriteno, unsigned char color);
void set_sprite_color_0     (unsigned char color);
void set_sprite_color_1     (unsigned char color);
void stretch_sprites        (unsigned char h_mask, unsigned char v_mask);
void stretch_sprite         (unsigned char spriteno, bool horizontal, bool vertical);
void load_sprite_to_block   (unsigned char *sprite, unsigned char blockno);
void set_sprite_from_block  (unsigned char spriteno, unsigned char blockno);
void set_sprite_enable_mask (unsigned char mask);
void enable_sprite          (unsigned char spriteno, bool enabled );
void set_sprite_coordinates (unsigned char spriteno, int pos_x, unsigned char pos_y);
void set_sprite_priority_mask (unsigned char mask);
void enable_multicolor_chars (bool enable);
void set_character_color_0  (unsigned char color);
void set_character_color_1  (unsigned char color);
void set_character_color    (unsigned char color);
void fill_with_char (unsigned char c, unsigned char from_x, unsigned char from_y, unsigned char to_x, unsigned char to_y);
void load_char_array (unsigned char *array);
void change_character_set   (unsigned char set);
void poke_char_to (unsigned char c, char x, unsigned char y);
void text_reverse (bool enable);
void character_box (unsigned char from_x, unsigned char from_y, unsigned char to_x, unsigned char to_y);

unsigned char get_raster    (void);
unsigned char get_sprite_collision_mask (void);
unsigned char get_background_collision_mask (void);
bool sprite_collision (unsigned char spriteno);
bool background_collision (unsigned char spriteno);

#endif
