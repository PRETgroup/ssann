/*
 * playgame.h
 *
 *  Created on: Mar 12, 2018
 *      Author: keyan
 */

#ifndef PLAYGAME_H_
#define PLAYGAME_H_

#include "game.h"

#define P 2
#define SIZE 10

// struct declarations
Game p_game;
static Game *this_game = &p_game;

// initialisation of system
void init();

// ANN decision functions
void rabbit_decide(int * x, int * y);
void wolf1_decide(int * x, int * y);
void wolf2_decide(int * x, int * y);

// animal move functions
void rabbit_move();
void wolf1_move();
void wolf2_move();

int check_score(int score[2]);

// Esterel generated functions
void playgame_I_decide();

void playgame_O_rx(int x);
void playgame_O_ry(int y);
void playgame_O_w1x(int x);
void playgame_O_w1y(int y);
void playgame_O_w2x(int x);
void playgame_O_w2y(int y);

int playgame();
int playgame_reset();

#endif /* PLAYGAME_H_ */

