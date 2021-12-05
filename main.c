#include <cc65.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <joystick.h>
#include <peekpoke.h>

#include "gfx.h"
#include "spritedata.h"
#include "players.h"

struct player player_one;
struct player player_two;

unsigned char screen_size_x, screen_size_y;

static const char title [] = "BattlePong!";

void poll_joysticks (void)
{
	player_one.joy = joy_read(JOY_2);
	player_two.joy = joy_read(JOY_1);

	if (JOY_UP    (player_one.joy)) player_one.pos_y--;
	if (JOY_DOWN  (player_one.joy)) player_one.pos_y++;
	if (JOY_LEFT  (player_one.joy)) player_one.pos_x--;
	if (JOY_RIGHT (player_one.joy)) player_one.pos_x++;

	if (JOY_UP    (player_two.joy)) player_two.pos_y--;
	if (JOY_DOWN  (player_two.joy)) player_two.pos_y++;
	if (JOY_LEFT  (player_two.joy)) player_two.pos_x--;
	if (JOY_RIGHT (player_two.joy)) player_two.pos_x++;
}

void draw_field (void)
{
	/* Top line */
	cputc (CH_ULCORNER);
	chline (screen_size_x - 2);
	cputc (CH_URCORNER);

	/* Vertical line, left side */
	cvlinexy (0, 1, screen_size_y - 2);

	/* Bottom line */
	cputc (CH_LLCORNER);
	chline (screen_size_x - 2);
	cputc (CH_LRCORNER);

	/* Vertical line, right side */
	cvlinexy (screen_size_x - 1, 1, screen_size_y - 2);

	/* Write the greeting in the mid of the screen */
	gotoxy ((screen_size_x - strlen (title)) / 2, screen_size_y / 2);
	cprintf ("%s", title);
}

int main (void)
{

	joy_install (joy_static_stddrv);

	screensize (&screen_size_x, &screen_size_y);
	clear_screen ();

	set_sprite_mode_mask(0b00000011);
	set_sprite_color_0 (COLOR_BLUE);
	set_sprite_color_1 (COLOR_ORANGE);

	set_text_color (COLOR_WHITE);
	set_border_color (COLOR_BLUE);
	set_background_color (COLOR_BLACK);

  load_sprite_to_block 	 (spacefox_sprite, 13);
  set_sprite_from_block  (0, 13);
	set_sprite_from_block  (1, 13);

	set_sprite_color (0, COLOR_WHITE);
	set_sprite_color (1, COLOR_CYAN);

	stretch_sprites (0b00000000, 0b00000000); // h_mask, v_mask

	set_sprite_enable_mask (0b00000011);
	set_sprite_coordinates (0, 64, 64);
	set_sprite_coordinates (1, 64, 64);

	set_sprite_priority (0b00000001);

	player_two.pos_x = 128;
	player_two.pos_y = 64;

	player_one.pos_x = 64;
	player_one.pos_y = 64;

	draw_field();

  while (1)
	{
		poll_joysticks();
		set_sprite_coordinates (0, player_one.pos_x, player_one.pos_y);
		set_sprite_coordinates (1, player_two.pos_x, player_two.pos_y);
		gotoxy (1, 1);
		cprintf("R: %d", PEEK(SPRITE_X_COORD_MSB));
		gotoxy (1, 2);
		cprintf("P: %d", player_one.pos_x);		
	}

	joy_uninstall ();
	clear_screen ();
  printf ("Exited\n");
  return EXIT_SUCCESS;
}
