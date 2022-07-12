#include <6502.h>
#include <cc65.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <joystick.h>
#include <peekpoke.h>

#include "sound.h"
#include "gfx.h"
#include "spritedata.h"
#include "players.h"
#include "utils.h"
#include "title.h"

struct player player_one;
struct player player_two;

const char help_line_1[] = "F1 | JOY1 to select player one";
const char help_line_2[] = "F2 | JOY2 to select player two";
const char help_line_3[] = "PRESS FIRE TO START!";
const char help_line_4[] = "Press FIRE for a re-match";

const char player_name[5][5] = {"Zero", "Nick", "Yodh", "Iris", "Pong"};
char score[] = "0:0";

#define PLAYER_ONE 0
#define PLAYER_TWO 1
#define BALL			 2

#define NUM_OF_LIVES 	 5
#define LOSS_TO_LIFE	 1
#define PLAYER_V_SPEED 2
#define PLAYER_H_SPEED 1
#define GAME_RESTART	 0
#define GAME_IS_ON	 	 1
#define GAME_OVER	 		 2
#define GAME_REMATCH	 4
#define GAME_EXIT			 8

unsigned char screen_size_x, screen_size_y;
unsigned char game_state = 0;
unsigned char game_speed = 255;

bool ball_x_dir = false;
bool ball_y_dir = true;
unsigned char ball_y = 140;
int  ball_x = 128;

/* PLAYERS */

void reset_ball()
{
	ball_x = 180;
	ball_y = 128;
	set_sprite_coordinates (BALL, ball_x, ball_y);
}

void init_players()
{
	player_one.life	 = NUM_OF_LIVES;
	player_one.last_life = NUM_OF_LIVES;
	player_one.score = 0;
	player_one.pos_x = 84;
	player_one.pos_y = 123;

	player_two.life  = NUM_OF_LIVES;
	player_two.last_life = NUM_OF_LIVES;
	player_two.score = 0;
	player_two.pos_x = 260;
	player_two.pos_y = 123;

	game_state = GAME_IS_ON;
	set_sprite_enable_mask (0b00000111);
}

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

	player_one.pos_x = constrain_int(player_one.pos_x, 30, 150);
	player_two.pos_x = constrain_int(player_two.pos_x, 200, 315);

	player_one.pos_y = constrain_char(player_one.pos_y, 50, 230);
	player_two.pos_y = constrain_char(player_two.pos_y, 50, 230);
}

/* Draw title screen */

void draw_title ()
{
	unsigned char n, i;

	poke_text_color(COLOR_WHITE);
	for (i=0; i<4; i++){
		for (n=0; n<40; n++){
			poke_char_to(title[i*40+n], n, i);
		}
	}

	set_text_color(COLOR_LIGHT_BLUE);
	character_box (5, 7, 6, 7);
	set_sprite_coordinates (PLAYER_ONE, 84, 123);
	character_box (27, 7, 6, 7);
	set_sprite_coordinates (PLAYER_TWO, 260, 123);

	gotoxy(screen_size_x/2-strlen(help_line_1)/2, 20);
	printf("%s", help_line_1);
	gotoxy(screen_size_x/2-strlen(help_line_2)/2, 21);
	printf("%s", help_line_2);

	set_text_color(COLOR_YELLOW);
	gotoxy(screen_size_x/2-strlen(help_line_3)/2, 24);
	printf("%s", help_line_3);

	set_text_color(COLOR_WHITE);
	gotoxy(19, 11);
	printf("VS.");

	i = COLOR_WHITE;
	n = COLOR_GREEN;
	while (!(JOY_FIRE (player_two.joy)||(JOY_FIRE (player_one.joy))))
	{
		poll_joysticks();

		// cycle through the players with function keys
		switch (cbm_k_getin()) {
			case 133: // F1
			i++;
			if (i>5) i = 1;
			break;

			case 137: // F2
			n++;
			if (n>5) n = 1;
			break;

			default:
			break;
		}

		// select player with respective joysticks
		if (player_one.last_joy != player_one.joy){
			if (JOY_LEFT  (player_one.joy)) i--;
			if (JOY_RIGHT (player_one.joy)) i++;
			i = constrain_char(i, 1, 5);
		}

		if (player_two.last_joy != player_two.joy){
			if (JOY_LEFT  (player_two.joy)) n--;
			if (JOY_RIGHT (player_two.joy)) n++;
			n = constrain_char(n, 1, 5);
		}

		set_sprite_color (PLAYER_ONE, i);
		gotoxy(7,16);
		player_one.id = i-1;
		printf("%s",player_name[player_one.id]);

		set_sprite_color (PLAYER_TWO, n);
		gotoxy(29,16);
		player_two.id = n-1;
		printf("%s",player_name[player_two.id]);

		player_one.last_joy = player_one.joy;
		player_two.last_joy = player_two.joy;
	}

	clear_screen();
}

/* GAME OVER */

void game_over()
{
	game_state = GAME_OVER;
	reset_ball();
	set_sprite_enable_mask (0b00000000);
	set_text_color(COLOR_WHITE);
	fill_with_char(0x20, 17, 7, 24, 15);
	character_box (16, 7, 7, 7);
	gotoxy(17, 16);
	printf("WINNER!");
	gotoxy(screen_size_x/2-strlen(help_line_4)/2, 24);
	printf("%s", help_line_4);

	gotoxy(6, 20);
	printf("[M]");
	gotoxy(16, 20);
	printf("[R]");
	gotoxy(29, 20);
	printf("[Q]");
	set_text_color(COLOR_YELLOW);
	gotoxy(9, 20);
	printf("enu");
	gotoxy(19, 20);
	printf("estart");
	gotoxy(32, 20);
	printf("uit");

	if (player_one.score>player_two.score){
		set_sprite_coordinates (PLAYER_ONE, 176, 123);
		enable_sprite(PLAYER_ONE, true);
	}
	else {
		set_sprite_coordinates (PLAYER_TWO, 176, 123);
		enable_sprite(PLAYER_TWO, true);
	}

	while (game_state==GAME_OVER)
	{
		poll_joysticks();
		if (JOY_FIRE (player_two.joy) || JOY_FIRE (player_one.joy)) {
			game_state = GAME_REMATCH;
		}

		switch (cbm_k_getin()) {
			case 0x51: // q - quit
			game_state = GAME_EXIT;
			break;
			case 0x52: // r - re-match
			game_state = GAME_REMATCH;
			break;
			case 0x4D: // m - menu
			game_state = GAME_RESTART;
			break;
			default:
			break;
		}
	}
}

void shake_borders (unsigned char color)
{
	unsigned char n;
	for (n=0; n<128; n++)
	{
		set_border_color (color);
		raster_wait(25);
		set_border_color (COLOR_BLUE);
		raster_wait(25);
	}
	set_border_color (COLOR_BLUE);
}

void update_lives (void)
{
	unsigned char n;

	player_one.life = NUM_OF_LIVES-(player_two.score/LOSS_TO_LIFE);
	player_two.life = NUM_OF_LIVES-(player_one.score/LOSS_TO_LIFE);

	if (player_one.life!=player_one.last_life)
	{
		gotoxy(6,1);
		for (n=1; n<=NUM_OF_LIVES; n++){
			if (n<=player_one.life) set_text_color(COLOR_WHITE);
			else set_text_color(COLOR_LIGHT_BLUE);
			cputc(0xA1);
		}
		shake_borders (player_one.id+1);
	}

	if (player_two.life!=player_two.last_life)
	{
		gotoxy(29,1);
		for (n=1; n<=NUM_OF_LIVES; n++){
			if (n<=player_two.life) set_text_color(COLOR_WHITE);
			else set_text_color(COLOR_LIGHT_BLUE);
			cputc(0xA1);
		}
		shake_borders (player_two.id+1);
	}

	if (player_two.life==0 || player_one.life==0) game_over();
	player_one.last_life = player_one.life;
	player_two.last_life = player_two.life;
}

void update_scores()
{
	ball_sound();
	if (player_one.score<10){
		gotoxy(screen_size_x/2-1, 1);
	}
	else {
		gotoxy(screen_size_x/2-2, 1);
	}
	set_text_color(COLOR_WHITE);
	printf("%d:%d",player_one.score, player_two.score);
	update_lives();
}

/* Draw the character based play field */

void draw_field (void)
{
	unsigned char n;

	clear_screen();
	set_text_color(COLOR_LIGHT_BLUE);
	// field background
	fill_with_char(123, 1, 1, 39, 25);
	// field frame
	set_text_color(COLOR_WHITE);
	fill_with_char(0xAB, 1, 3, 2, 25);
	fill_with_char(0xB3, screen_size_x-1, 3, screen_size_x, 25);
	// fill_with_char(0xB1, 2, 24, screen_size_x-1, 25);
	// player brackets
	fill_with_char(0x20, 1, 1, 11, 2);
	character_box (0, 0, 10, 1);
	fill_with_char(0x20, 29, 1, 39, 2);
	character_box (28, 0, 10, 1);
	// score box
	fill_with_char(0x20, 17, 1, 24, 2);
	character_box (16, 0, 7, 1);
	// display player names
	gotoxy(1,1);
	printf("%s",player_name[player_one.id]);
	gotoxy(screen_size_x-strlen(player_name[player_two.id])-1,1);
	printf("%s",player_name[player_two.id]);
	// display initial scores
	update_scores();
	gotoxy(6,1);
	for (n=1; n<=NUM_OF_LIVES; n++){
		if (n<=player_one.life) set_text_color(COLOR_WHITE);
		else set_text_color(COLOR_LIGHT_BLUE);
		cputc(0xA1);
	}

	gotoxy(29,1);
	for (n=1; n<=NUM_OF_LIVES; n++){
		if (n<=player_two.life) set_text_color(COLOR_WHITE);
		else set_text_color(COLOR_LIGHT_BLUE);
		cputc(0xA1);
	}
}

/* Player animations and sprite streching */

bool anim_players = false;

void animate_players ()
{
	if (get_raster()==255)
	{
		anim_players=!anim_players;
		if (anim_players){
			set_sprite_from_block (PLAYER_ONE, 14);
			set_sprite_from_block (PLAYER_TWO, 13);
		}
		else {
			set_sprite_from_block (PLAYER_ONE, 13);
			set_sprite_from_block (PLAYER_TWO, 14);
		}
	}
	set_sprite_coordinates (PLAYER_ONE, player_one.pos_x, player_one.pos_y);
	set_sprite_coordinates (PLAYER_TWO, player_two.pos_x, player_two.pos_y);
}

/* Movement of the ball */

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

	if (ball_x>320) { // ball is out on right side
		ball_x_dir=!ball_x_dir; // reverse for now
		player_one.score+=1;
		update_scores();
	}
	if (ball_x<25) { // ball is out on left side
		ball_x_dir=!ball_x_dir; // reverse for now
		player_two.score+=1;
		update_scores();
	}

	if (sprite_collision(BALL)) ball_x_dir=!ball_x_dir;

	set_sprite_coordinates (BALL, ball_x, ball_y);
}

/* Collision interrupt */

void collision_irq(void) {
	VIC.irr = 0b00000100; // interrupt request register
	ball_x_dir=!ball_x_dir;
	get_sprite_collision_mask(); // read to clear
	__asm__("JMP $EA31"); // jump to irq vector
}

void __fastcall__ irq_setup(void (*irqh)(void)) {
	CIA1.icr = 0x7f;
	VIC.imr  = 0; // disable IRQ sources, SEI not needed
	POKEW(0x0314, (int)irqh); // set kernal IRQ vector
	VIC.imr  = 0b00000100; //  Interrupt Mask Register: Bit#2 Sprite-Sprite collision. Bit#0 Raster
}

/* Main game cycle */

int main (void)
{

	joy_install (joy_static_stddrv);
	screensize (&screen_size_x, &screen_size_y);
	set_text_color(COLOR_YELLOW);
	printf("Loading...");

	// irq_setup(&collision_irq); // breaks cbm_k_getin for some reason

	restart:
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
	set_sprite_priority_mask (0b00000000); // 1 - behind background
	set_sprite_enable_mask (0b00000011); // 1 - enabled

	clear_screen ();
	draw_title ();

	rematch:
	init_players ();
	draw_field ();

  while (game_state != GAME_EXIT)
	{
		poll_joysticks ();
		animate_players ();
		move_ball ();

		//raster_wait(game_speed);

		switch (game_state)
		{
			case GAME_IS_ON:
			// nothing to do
			break;
			case GAME_REMATCH:
			goto rematch;
			break;
			case GAME_RESTART:
			goto restart;
			break;
			default:
			break;
		}
	}

	set_sprite_enable_mask (0b00000000);
	set_border_color (COLOR_LIGHT_BLUE);
	set_background_color (COLOR_BLUE);
	joy_uninstall ();
	clear_screen ();
  printf ("Good bye!\n");
  return EXIT_SUCCESS;
}
