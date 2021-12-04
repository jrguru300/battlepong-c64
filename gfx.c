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
  // unsigned char *bg;
  // bg = (unsigned char *) BORDER_COLOR;
  // *bg = color;
}

void set_text_color (unsigned char color)
{
  textcolor ( color );
}

unsigned char get_raster (void)
{
  return PEEK(RASTER_ADDRESS);
}

void clear_screen (void)
{
  clrscr ();
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


void set_sprite_enable_mask (unsigned char mask)
{
  POKE(VRAM_START + 21, mask);
}

void set_sprite_from_block (unsigned char spriteno, unsigned char blockno)
{
  POKE(SPRITE_POINT_START + spriteno, blockno); // set sprite pointer
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

void set_sprite_coordinates (unsigned char spriteno, unsigned char pos_x, unsigned char pos_y)
{
  POKE(VRAM_START + spriteno*2,     pos_x);
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

// TODO

/*
  Through manipulating the bits in address 53275/$D01B,
  sprites can be set to appear "behind" (bits set to "1") or "in front of" (bits set to "0") background graphics.
  The least significant bit affects sprite #0, and the most sigificant bit sprite #7.
*/

/*
  Polling for collisions
  There are two VIC registers that can be polled to see if a collision involving sprites have occured:

  Sprites involved in a collision with other sprites will have their respective bits set to "1" in address 53278/$D01E – all other sprites will have a "0" bit here.
  Sprites involved in a collision with background graphics will have their respective bits set to "1" in address 53279/$D01F – all other sprites will report a "0".
  As with all other "one bit per sprite" registers, the least significant bit affects sprite #0, and the most sigificant bit sprite #7.

  Interrupt on collision
  Both the interrupt event register (at address 53273/$D019) and interrupt enable register (at address 53274/$D01A) have provisions for raising IRQ-type interrupts at the CPU whenever collisions occur:

  Bit 2 (weight 4) in both registers concern sprite-to-sprite collisions, and
  Bit 1 (weight 2) in both registers concern sprite-to-background collisions.
*/
