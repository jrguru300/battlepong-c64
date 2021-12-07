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

static const char title [] = "[BattlePong]";

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
	cputc (CH_ULCORNER);
	chline (screen_size_x - 2);
	cputc (CH_URCORNER);
	cvlinexy (0, 1, screen_size_y - 2);
	cputc (CH_LLCORNER);
	chline (screen_size_x - 2);
	cputc (CH_LRCORNER);
	cvlinexy (screen_size_x - 1, 1, screen_size_y - 2);
	gotoxy ((screen_size_x - strlen (title)) / 2, 0);
	cprintf ("%s", title);
}

bool anim_sprite = false;
int ball_x = 0;

int main (void)
{

	joy_install (joy_static_stddrv);

	screensize (&screen_size_x, &screen_size_y);
	clear_screen ();

	// multicolor sprites
	set_sprite_mode_mask(0b00000111);

	// common colors for all multicolor sprites
	set_sprite_color_0 (COLOR_BLUE);
	set_sprite_color_1 (COLOR_ORANGE);

	// individual colors of sprites
	set_sprite_color (0, COLOR_WHITE);
	set_sprite_color (1, COLOR_CYAN);
	set_sprite_color (2, COLOR_CYAN);

	set_text_color (COLOR_WHITE);
	set_border_color (COLOR_BLUE);
	set_background_color (COLOR_BLACK);

  load_sprite_to_block 	 (spacefox_sprite_0, 13);
	load_sprite_to_block 	 (spacefox_sprite_1, 14);
	load_sprite_to_block 	 (ball_sprite, 15);
  set_sprite_from_block  (0, 13);
	set_sprite_from_block  (1, 14);
	set_sprite_from_block  (2, 15);


	stretch_sprites (0b00000000, 0b00000000); // 1 - stretched on (h_mask, v_mask)
	set_sprite_enable_mask (0b00000111); // 1 - enabled
	set_sprite_priority_mask (0b00000000); // 1 - behind background

	player_one.pos_x = 300;
	player_one.pos_y = 140;
	player_two.pos_x = 40;
	player_two.pos_y = 140;

	draw_field();

  while (1)
	{
		poll_joysticks();
		set_sprite_coordinates (0, player_one.pos_x, player_one.pos_y);
		set_sprite_coordinates (1, player_two.pos_x, player_two.pos_y);

		if (get_raster()==255){
			anim_sprite=!anim_sprite;
			if (anim_sprite){
				set_sprite_from_block  (1, 13);
				set_sprite_from_block  (0, 14);
			}
			else {
				set_sprite_from_block  (1, 14);
				set_sprite_from_block  (0, 13);
			}
		}

		set_sprite_coordinates (2, ball_x, 128);
		ball_x++; if (ball_x>366) ball_x = 0;

		stretch_sprite(0, false, (JOY_FIRE (player_one.joy)));
		stretch_sprite(1, false, (JOY_FIRE (player_two.joy)));

		if (sprite_collision(2)) {
			set_border_color (COLOR_WHITE);
		}
		else {
			set_border_color (COLOR_BLUE);
		}
	}

	joy_uninstall ();
	clear_screen ();
  printf ("Exited\n");
  return EXIT_SUCCESS;
}
