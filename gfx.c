#include <peekpoke.h>
#include <conio.h>

#include "gfx.h"

void set_background_color (unsigned char color)
{
  bgcolor ( color );
  // unsigned char *bg;
  // bg = (unsigned char *) BACKGROUND_COLOR;
  // *bg = color;
}

void set_border_color (unsigned char color)
{
  bordercolor ( color );
}

void set_text_color (unsigned char color)
{
  textcolor ( color );
}

/*
  Depending on the bit-pair combination, the color of each pixel will be:

  Transparent, or "background" if the bit pair is "00"
  The color indicated in address 53282/$D022 where the bit pair is "01"
  The color indicated in address 53283/$D023 where the bit pair is "10"
  The character's individual color (55296–56295/$D800–DBE7) where the bit pair is "11"

  The text screen in multicolor mode uses the most significant of the four bits in each character's
  individual color: This means that the screen can display both multicolor and high resolution
  characters side-by-side, but the disadvantage is that the individual color for each multicolor character
  can only be chosen from the second half (color codes 8 thru 15) of the normal 16-color palette.

  The two colors stored in 53282/$D022 and 53283/$D023 are common for all multicolor characters on screen,
  but may assume any of the 16 available color codes.
*/

void enable_multicolor_chars (bool enable)
{
  if (enable){
    POKE(CHAR_MULTICOLOR, PEEK(CHAR_MULTICOLOR) | 0x10);
  }
  else {
    POKE(CHAR_MULTICOLOR, PEEK(CHAR_MULTICOLOR) & 0xEF);
  }
}

void set_character_color_0  (unsigned char color)
{
  POKE(CHAR_COMMON_C_1, color); // 01
}

void set_character_color_1  (unsigned char color)
{
  POKE(CHAR_COMMON_C_2, color); // 10
}

void set_character_color (unsigned char color)
{
  // TODO: set char individual color $D800–DBE7
}

void poke_char_to (unsigned char c, char x, unsigned char y)
{
  POKE(1024+y*40+x, c);
}

void text_reverse (bool enable)
{
  POKE(0xC7, enable);
}

void character_box (unsigned char from_x, unsigned char from_y, unsigned char to_x, unsigned char to_y)
{
  gotoxy(from_x, from_y);
  cputc (CH_ULCORNER);
  chline (to_x);
  cputc (CH_URCORNER);
  cvlinexy (from_x, from_y+1, to_y);
  cputc (CH_LLCORNER);
  chline (to_x);
  cputc (CH_LRCORNER);
  cvlinexy (from_x+to_x+1, from_y+1, to_y);
}

void change_character_set (unsigned char set)
{
  switch (set){

    case 1:
      POKE(CHARACTER_SET, 23);
    break;

    case 2:
      POKE(CHARACTER_SET, 21);
    break;

    default:
    break;
  }
}

/*
  Display operations
*/

unsigned char get_raster (void)
{
  return PEEK(RASTER_ADDRESS);
}

void raster_wait(unsigned char line) {
	unsigned char raster;
	do { raster = get_raster(); } while (raster < line);
}

void clear_screen (void)
{
  clrscr ();
}

void fill_with_char (unsigned char c, unsigned char from_x, unsigned char from_y, unsigned char to_x, unsigned char to_y)
{
  unsigned char x = 0;
  unsigned char y = 0;
  for (x = from_x; x < to_x; x++){
    for (y = from_y; y < to_y; y++){
      gotoxy (x,y);
      cputc (c);
    }
  }
}

/* https://www.c64-wiki.com/wiki/Sprite */

/*
  Sprite block register
  (e.g. sprite 1 in datasette I/O buffer $033C: Blockno. = 13 (13 * 64 = 832, in hex: $0340))

  #0 $07F8	2040	Pointer for sprite 1 in memory block: Blockno.*64
  #1 $07F9	2041	Pointer for sprite 2 in memory block: Blockno.*64
  #2 $07FA	2042	Pointer for sprite 3 in memory block: Blockno.*64
  #3 $07FB	2043	Pointer for sprite 4 in memory block: Blockno.*64
  #4 $07FC	2044	Pointer for sprite 5 in memory block: Blockno.*64
  #5 $07FD	2045	Pointer for sprite 6 in memory block: Blockno.*64
  #6 $07FE	2046	Pointer for sprite 7 in memory block: Blockno.*64
  #7 $07FF	2047	Pointer for sprite 8 in memory block: Blockno.*64
*/

void load_sprite_to_block (unsigned char *sprite, unsigned char blockno)
{
  unsigned char n;
  for (n = 0; n < SPRITE_SIZE; n++) {
    POKE((SPRITE_BLOCK_SIZE * blockno) + n, sprite[n]);
  }
}

void set_sprite_from_block (unsigned char spriteno, unsigned char blockno)
{
  POKE(SPRITE_POINT_START + spriteno, blockno); // set sprite pointer
}

void set_sprite_enable_mask (unsigned char mask)
{
  POKE(SPRITES_ENABLED, mask);
}

void enable_sprite (unsigned char spriteno, bool enabled )
{
  unsigned char sprite_enable_mask;
  sprite_enable_mask = PEEK(SPRITES_ENABLED);
  if (enabled) {
    sprite_enable_mask |= 1UL << spriteno;
  }
  else {
    sprite_enable_mask &= ~(1UL << spriteno);
  }
  set_sprite_enable_mask (sprite_enable_mask);
}

/*
  	  x coordinate	y coordinate
  #0  53248/$D000   53249/$D001
  #1	53250/$D002	  53251/$D003
  #2	53252/$D004   53253/$D005
  #3	53254/$D006   53255/$D007
  #4	53256/$D008   53257/$D009
  #5	53258/$D00A   53259/$D00B
  #6	53260/$D00C   53261/$D00D
  #7	53262/$D00E   53263/$D00F
*/

/*
  The x coordinate addresses given in the table only holds the eight least significant bits of the x coordinates
  for the respective sprites. The ninth and most significant bit for each of the eight sprites are "gathered" in
  address 53264/$D010; the least significant bit here corresponds to sprite #0.
*/

void set_sprite_coordinates (unsigned char spriteno, int pos_x, unsigned char pos_y)
{
  unsigned char sprite_pos_x_mask;
  sprite_pos_x_mask = PEEK(SPRITE_X_COORD_MSB);
  if (pos_x > 255) {
    sprite_pos_x_mask |= 1UL << spriteno;
  }
  else {
    sprite_pos_x_mask &= ~(1UL << spriteno);
  }
  POKE(SPRITE_X_COORD_MSB, sprite_pos_x_mask);
  POKE(VRAM_START + spriteno*2,     pos_x & 0xFF);
  POKE(VRAM_START + (spriteno*2)+1, pos_y);
}

/*
  53285/$D025 is the common color displayed for "01" bit pairs in the sprite data, and
  53286/$D026 is the common color displayed for "11" bit pairs in the sprite data.
  %00	transparency
  %10	Sprite color register (normal/uni color; $D027-$D02E)
  %01	Multicolor register #0 ($D025)
  %11	Multicolor register #1 ($D026)
*/

void set_sprite_mode_mask (unsigned char mask)
{
  POKE(SPRITE_MODE, mask); // high resolution mode (0), multicolor mode (1) - LSB sprite #0
}

void set_sprite_color_0 (unsigned char color)
{
  POKE(SPRITE_COMMON_C_1, color); // common color "01"
}

void set_sprite_color_1 (unsigned char color)
{
  POKE(SPRITE_COMMON_C_2, color); // common color "11"
}

/*
  Sprite color registers

  #0	53287/$D027
  #1	53288/$D028
  #2	53289/$D029
  #3	53290/$D02A
  #4	53291/$D02B
  #5	53292/$D02C
  #6	53293/$D02D
  #7	53294/$D02E
*/

void set_sprite_color (unsigned char spriteno, unsigned char color)
{
  POKE(SPRITE_COLOR_START+spriteno, color); // unique sprite color
}

/*
  Registers 53277/$D01D and 53271/$D017,
  each sprite may be individually "stretched" to twice the width and/or twice the height.
  In both registers, the least significant bit affects sprite #0, and the most sigificant bit sprite #7.
*/

void stretch_sprites (unsigned char h_mask, unsigned char v_mask)
{
  POKE(STRETCH_SPRITE_H, h_mask);
  POKE(STRETCH_SPRITE_V, v_mask);
}

void stretch_sprite (unsigned char spriteno, bool horizontal, bool vertical)
{
  unsigned char h_mask = 0x00;
  unsigned char v_mask = 0x00;

  h_mask = PEEK(STRETCH_SPRITE_H);
  v_mask = PEEK(STRETCH_SPRITE_V);

  if (horizontal) {
    h_mask |= 1UL << spriteno;
  }
  else {
    h_mask &= ~(1UL << spriteno);
  }

  if (vertical) {
    v_mask |= 1UL << spriteno;
  }
  else {
    v_mask &= ~(1UL << spriteno);
  }
  stretch_sprites (h_mask, v_mask);
}

/*
  Through manipulating the bits in address 53275/$D01B,
  sprites can be set to appear "behind" (bits set to "1") or "in front of" (bits set to "0") background graphics.
  The least significant bit affects sprite #0, and the most sigificant bit sprite #7.
*/

void set_sprite_priority_mask (unsigned char mask)
{
  POKE(SPRITE_PRIORITY, mask);
}

/*
  Polling for collisions
  There are two VIC registers that can be polled to see if a collision involving sprites have occured:

  Sprites involved in a collision with other sprites will have their respective bits set to "1" in address 53278/$D01E – all other sprites will have a "0" bit here.
  Sprites involved in a collision with background graphics will have their respective bits set to "1" in address 53279/$D01F – all other sprites will report a "0".
  As with all other "one bit per sprite" registers, the least significant bit affects sprite #0, and the most sigificant bit sprite #7.
*/

unsigned char get_sprite_collision_mask ()
{
  return PEEK(SPRITE_SP_COLLISON);
}

unsigned char get_background_collision_mask ()
{
  return PEEK(SPRITE_BG_COLLISION);
}

bool sprite_collision (unsigned char spriteno)
{
  bool collision = false;
  collision = CHECK_BIT(get_sprite_collision_mask(), spriteno);
  return collision;
}

bool background_collision (unsigned char spriteno)
{
  bool collision = false;
  collision = CHECK_BIT(get_background_collision_mask(), spriteno);
  return collision;
}

// TODO

/*
  Interrupt on collision
  Both the interrupt event register (at address 53273/$D019) and interrupt enable register (at address 53274/$D01A) have provisions for raising IRQ-type interrupts at the CPU whenever collisions occur:

  Bit 2 (weight 4) in both registers concern sprite-to-sprite collisions, and
  Bit 1 (weight 2) in both registers concern sprite-to-background collisions.
*/
