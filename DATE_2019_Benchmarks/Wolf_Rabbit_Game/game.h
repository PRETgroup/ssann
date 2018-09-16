/*
 * game.h
 *
 *  Created on: Mar 12, 2018
 *      Author: keyan
 */

#ifndef GAME_H_
#define GAME_H_

// includes
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "ann_fixed.h"

// definitions
#define MAX_BLOCKS 	10
#define MAX_EXITS 	3
#define MAX_INPUTS	8
#define RABBIT_OUT	4
#define WOLF_OUT	8
#define PI fix16_from_float(3.14159265358979323846)

// animal struct, identical for wolf and rabbit
typedef struct{ 
	int old_pos[2];
	int pos[2];
	int score;
	ANN_F *ann;
	int symbol;
}Animal;

// game state struct
typedef struct{
	int state[MAX_BLOCKS][MAX_BLOCKS];
	int exits[MAX_EXITS][2];
	int size;
	// 0 - empty
	// 1 - obstacle
	// 2 - rabbit exit
	// 3 - wolf1
	// 4 - wolf2
	// 5 - rabbit
	int score[2]; // score between wolves and rabbit
	int round;
	int turns; // wolf1, wolf2 or rabbit
	int max_rounds;
	int num_exits;
	int max_score;
	int max_turns;
	fix16_t move_thresh;
	Animal *rabbit;
	Animal *wolf1;
	Animal *wolf2;
}Game;

// test ann output
void testWolfANN(int index, ANN_F *ann);

// animal functions
void init_animal(Animal *animal, int score, int pos[], int symbol, int num_layers, int layers[num_layers], int bias, int activation[], int num);

// game initialization
void init_game(Animal *wolf1, Animal *wolf2, Animal *rabbit, Game *game, int rounds, int score, int size, int turns, int num_exits, fix16_t move_thresh);
void update_game(Game *game);
void print_game(Game *game);
void start_game(Game *game);
void get_inputs(Game *game, int pos[], fix16_t inputs[20]);

// game running
void rabbit_turn(Game *game, int new_pos[2]);
void wolf1_turn(Game *game, int new_pos[2]);
void wolf2_turn(Game *game, int new_pos[2]);
void run_rabbit(Game *game, int new_pos[2], int score[2]);
void run_wolf1(Game *game, int new_pos[2], int score[2]);
void run_wolf2(Game *game, int new_pos[2], int score[2]);

// helper functions
int check_pos(Game *game, int pos[2], int val);
fix16_t dist(int p1[], int p2[]);
fix16_t angle(int p1[], int p2[]);

#endif /* GAME_H_ */
























