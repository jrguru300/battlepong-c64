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
    POKE((64*blockno) + n, sprite[n]);
  }
}


void set_sprite_enable_mask (unsigned char mask)
{
  POKE(VRAM_START + 21, mask);
}

void set_sprite_from_block (unsigned char spriteno, unsigned char blockno)
{
  POKE(2040 + spriteno, blockno); // set sprite pointer
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
  POKE(53276, mask); // high resolution mode (0), multicolor mode (1) - LSB sprite #0
}

void set_sprite_color_m0 (unsigned char color)
{
  POKE(53285, color);
}

void set_sprite_color_m1 (unsigned char color)
{
  POKE(53286, color);
}
