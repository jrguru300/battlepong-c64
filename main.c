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
#include "utils.h"
#include "title.h"

struct player player_one;
struct player player_two;

const char player_name[5][5] = {"Zero", "Nick", "Yodh", "Iris", "Pong"};

#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define BALL			 2

#define PLAYER_V_SPEED 2
#define PLAYER_H_SPEED 1

unsigned char screen_size_x, screen_size_y;

/* Read CIA joystick registers and apply to player behaviour */

void poll_joysticks (void)
{
	player_one.joy = joy_read(JOY_1);
	player_two.joy = joy_read(JOY_2);

	if (JOY_UP    (player_one.joy)) player_one.pos_y-=PLAYER_V_SPEED;
	if (JOY_DOWN  (player_one.joy)) player_one.pos_y+=PLAYER_V_SPEED;
	if (JOY_LEFT  (player_one.joy)) player_one.pos_x-=PLAYER_H_SPEED;
	if (JOY_RIGHT (player_one.joy)) player_one.pos_x+=PLAYER_H_SPEED;

	if (JOY_UP    (player_two.joy)) player_two.pos_y-=PLAYER_V_SPEED;
	if (JOY_DOWN  (player_two.joy)) player_two.pos_y+=PLAYER_V_SPEED;
	if (JOY_LEFT  (player_two.joy)) player_two.pos_x-=PLAYER_H_SPEED;
	if (JOY_RIGHT (player_two.joy)) player_two.pos_x+=PLAYER_H_SPEED;

	player_one.pos_x = constrain_int(player_one.pos_x, 30, 140);
	player_two.pos_x = constrain_int(player_two.pos_x, 200, 315);

	player_one.pos_y = constrain_char(player_one.pos_y, 50, 230);
	player_two.pos_y = constrain_char(player_two.pos_y, 50, 230);
}

/* Draw title screen */

void draw_title ()
{
	unsigned char n, i;
	for (i=0; i<4; i++){
		for (n=0; n<40; n++){
			poke_char_to(title[i*40+n], n, i);
		}
	}

	set_text_color(COLOR_WHITE);
	character_box (5, 6, 6, 7);
	set_sprite_coordinates (PLAYER_ONE, 84, 115);

	character_box (25, 6, 6, 7);
	set_sprite_coordinates (PLAYER_TWO, 244, 115);

	gotoxy(17, 10);
	printf("VS.");

	i = COLOR_WHITE;
	n = COLOR_GREEN;

	while (!(JOY_FIRE (player_two.joy)))
	{
		poll_joysticks();
		if (JOY_LEFT  (player_one.joy)) i--;
		if (JOY_RIGHT (player_one.joy)) i++;
		if (JOY_LEFT  (player_two.joy)) n--;
		if (JOY_RIGHT (player_two.joy)) n++;

		i = constrain_char(i, 1, 5);
		set_sprite_color (PLAYER_ONE, i);
		gotoxy(7,15);
		printf("%s",player_name[i-1]);

		n = constrain_char(n, 1, 5);
		set_sprite_color (PLAYER_TWO, n);
		gotoxy(27,15);
		printf("%s",player_name[n-1]);
	}

}

/* Draw the character based play field */

void draw_field (void)
{
	set_text_color(COLOR_LIGHT_BLUE);
	fill_with_char(123, 0, 0, 39, 24);
	gotoxy(0,0);
	set_text_color(COLOR_BLACK);
	cputc (CH_ULCORNER);
	chline (screen_size_x - 2);
	cputc (CH_URCORNER);
	cvlinexy (0, 1, screen_size_y - 2);
	cputc (CH_LLCORNER);
	chline (screen_size_x - 2);
	cputc (CH_LRCORNER);
	cvlinexy (screen_size_x - 1, 1, screen_size_y - 2);
	cvlinexy (screen_size_x/2, 2, screen_size_y - 4);
}

/* Player animations and sprite streching */

bool anim_players = false;

void animate_players ()
{
	if (get_raster()==255)
	{
		anim_players=!anim_players;
		if (anim_players){
			set_sprite_from_block  (PLAYER_ONE, 14);
			set_sprite_from_block  (PLAYER_TWO, 13);
		}
		else {
			set_sprite_from_block  (PLAYER_ONE, 13);
			set_sprite_from_block  (PLAYER_TWO, 14);
		}
	}
	set_sprite_coordinates (PLAYER_ONE, player_one.pos_x, player_one.pos_y);
	set_sprite_coordinates (PLAYER_TWO, player_two.pos_x, player_two.pos_y);
}

/* Movement of the projectile */

bool ball_x_dir = false;
bool ball_y_dir = true;
unsigned char ball_y = 140;
int  ball_x = 128;

void move_ball()
{
	if (ball_x_dir){
		ball_x++;
	}
	else {
		ball_x--;
	}

	if (ball_y_dir){
		ball_y++;
	}
	else {
		ball_y--;
	}

	if (ball_y>=230){
		ball_y_dir = false;
	}
	if (ball_y<=50){
		ball_y_dir = true;
	}

	// TODO: setup collision interrupt

	if (sprite_collision(BALL)) { // ball is hit by either of players
		ball_x_dir=!ball_x_dir;
	}
	else { // ball is traversing without collision
		if (ball_x>320) { // ball is out on right side
			ball_x_dir=!ball_x_dir; // reverse for now
		}
		if (ball_x<25) { // ball is out on left side
			ball_x_dir=!ball_x_dir; // reverse for now
		}
	}

	set_sprite_coordinates (BALL, ball_x, ball_y);
}

/* Main game cycle */

int main (void)
{

	joy_install (joy_static_stddrv);

	screensize (&screen_size_x, &screen_size_y);
	clear_screen ();

	// multicolor sprites
	set_sprite_mode_mask(0b00000111);

	// common colors for all multicolor sprites
	set_sprite_color_0 (COLOR_BLACK);
	set_sprite_color_1 (COLOR_ORANGE);

	// individual colors of sprites
	set_sprite_color (BALL, COLOR_WHITE);

	set_border_color (COLOR_BLUE);
	set_background_color (COLOR_BLUE);

  load_sprite_to_block 	 (spacefox_sprite_0, 13);
	load_sprite_to_block 	 (spacefox_sprite_1, 14);
	load_sprite_to_block 	 (ball_sprite, 15);
  set_sprite_from_block  (PLAYER_ONE, 13);
	set_sprite_from_block  (PLAYER_TWO, 14);
	set_sprite_from_block  (BALL, 15);

	stretch_sprites (0b00000000, 0b00000011); // 1 - stretched on (h_mask, v_mask)
	set_sprite_enable_mask (0b00000111); // 1 - enabled
	set_sprite_priority_mask (0b00000000); // 1 - behind background

	player_one.pos_x = 40; // edge at 30
	player_one.pos_y = 140;
	player_two.pos_x = 300; // edge at 310
	player_two.pos_y = 140;

	//enable_multicolor_chars(true);

	draw_title();

	draw_field();

  while (1)
	{
		poll_joysticks();
		animate_players();
		move_ball();
		raster_wait(100); // TODO: change game speed according to level
	}

	joy_uninstall ();
	clear_screen ();
  printf ("Exited\n");
  return EXIT_SUCCESS;
}
