#include <cc65.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <joystick.h>

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

	set_text_color (COLOR_WHITE);
	set_border_color (COLOR_BLACK);
	set_background_color (COLOR_BLACK);

  load_sprite_to_block 	 (balloon_sprite, 13);
  set_sprite_from_block  (0, 13);
	set_sprite_enable_mask (0b00000001);
	set_sprite_coordinates (0, 64, 64);

	player_one.pos_x = 64;
	player_one.pos_y = 64;

	draw_field();

  while (1)
	{
		poll_joysticks();
		set_sprite_coordinates (0, player_one.pos_x, player_one.pos_y);
	}

	joy_uninstall ();
	clear_screen ();
  printf ("Exited\n");
  return EXIT_SUCCESS;
}
