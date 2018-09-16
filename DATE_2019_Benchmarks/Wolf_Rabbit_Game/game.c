/*
 * game.c
 *
 *  Created on: Mar 12, 2018
 *      Author: keyan
 */

#include "game.h"
#include "ann_weights.h"
// #include "libfixmath/fix16.c"
// #include "libfixmath/fix16_exp.c"
// #include "libfixmath/fix16_sqrt.c"
// #include "libfixmath/fix16_trig.c"

// initialize the animal structure with ANN and map data
void init_animal(Animal *animal, int score, int pos[], int symbol, int num_layers, int layers[num_layers], int bias, int activation[], int num)
{
	if(num == 0)
		ann_init_fixed_custom(animal->ann, num_layers, layers, R_LAYER1, r_weights, bias, activation);
	else if(num == 1)
		ann_init_fixed_custom(animal->ann, num_layers, layers, W_LAYER1, w1_weights, bias, activation);
	else
		ann_init_fixed_custom(animal->ann, num_layers, layers, W_LAYER1, w2_weights, bias, activation);
	animal->score = score;
	animal->pos[0] = pos[0];
	animal->pos[1] = pos[1];
	animal->old_pos[0] = pos[0];
	animal->old_pos[1] = pos[1];
	animal->symbol = symbol;
}

// initialize the game structure
void init_game(Animal *wolf1, Animal *wolf2, Animal *rabbit, Game *game, int rounds, int score, int size, int turns, int num_exits, fix16_t move_thresh)
{
	if(score == 0)
		game->max_score = rounds;
	else
		game->max_score = score;
	if(size < MAX_BLOCKS)
		game->size = size;
	else
		game->size = MAX_BLOCKS;
	game->max_rounds = rounds;
	game->round = 0;
	game->turns = 0;
	game->score[0] = -1;
	game->score[1] = -1;
	game->num_exits = num_exits;
	game->move_thresh = move_thresh;
	game->max_turns = turns;

	game->rabbit = rabbit;
	game->wolf1 = wolf1;
	game->wolf2 = wolf2;

	// first initialize game board rabbit exits
	int hole[2];
	int flag = 0;

	for(int i = 0; i < game->num_exits; i++)
	{
		do
		{
			hole[0] = rand()%game->size;
			hole[1] = rand()%game->size;
			flag = 1;
			for(int j = 0; j < 4; j++)
			{
				if(hole[0] == 0 || hole[0] == 9 || hole[1] == 0 || hole[1] == 9)
				{
					flag = 0;
					for(int k = 0; k < game->num_exits; k++)
					{
						if(game->exits[k][0] == hole[0] && game->exits[k][1] == hole[1])
						{
							flag = 1;
							break;
						}
					}
					break;
				}
			}
		}
		while(flag);

		game->exits[i][0] = hole[0];
		game->exits[i][1] = hole[1];
	}

	int x, y;
	// then initialize the animals on the board
	// wolf 1
	do
	{
		x = rand()%(game->size - 3) + 1; // don't generate on walls
		y = rand()%(game->size - 3) + 1; // don't generate on walls

		flag = 0;
		for(int i = 0; i < game->num_exits; i++)
		{
			if(game->exits[i][0] == x && game->exits[i][1] == y)
				flag = 1;
		}
	}
	while(flag);
	game->wolf1->score = 0;
	game->wolf1->pos[0] = x;
	game->wolf1->pos[1] = y;
	game->wolf1->old_pos[0] = x;
	game->wolf1->old_pos[1] = y;

	// wolf 2
	do
	{
		x = rand()%(game->size - 3) + 1; // don't generate on walls
		y = rand()%(game->size - 3) + 1; // don't generate on walls

		flag = 0;
		for(int i = 0; i < game->num_exits; i++)
		{
			if((game->exits[i][0] == x && game->exits[i][1] == y) || (x == wolf1->pos[0] && y == wolf1->pos[1]))
				flag = 1;
		}
	}
	while(flag);
	game->wolf2->score = 0;
	game->wolf2->pos[0] = x;
	game->wolf2->pos[1] = y;
	game->wolf2->old_pos[0] = x;
	game->wolf2->old_pos[1] = y;

	// rabbit
	do
	{
		x = rand()%(game->size - 3) + 1; // don't generate on walls
		y = rand()%(game->size - 3) + 1; // don't generate on walls

		flag = 0;
		for(int i = 0; i < game->num_exits; i++)
		{
			if((game->exits[i][0] == x && game->exits[i][1] == y) || (x == game->wolf1->pos[0] && y == game->wolf1->pos[1]) || (x == game->wolf2->pos[0] && y == game->wolf2->pos[1]))
				flag = 1;
		}
	}
	while(flag);
	game->rabbit->score = 0;
	game->rabbit->pos[0] = x;
	game->rabbit->pos[1] = y;
	game->rabbit->old_pos[0] = x;
	game->rabbit->old_pos[1] = y;

	// update game state with exits and animals
	update_game(game);
	// printf("Starting state: \n");
	// print_game(game);
}

// update changed positions on game state
void update_game(Game *game)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int flag = 0;

#pragma loopbound min 0 max MAX_BLOCKS
	for(i = 0; i < game->size; i++)
	{
#pragma loopbound min 0 max MAX_BLOCKS
		for(j = 0; j < game->size; j++)
		{
			flag = 0;
#pragma loopbound min 0 max MAX_EXITS
			for(k = 0; k < game->num_exits; k++)
			{
				if(game->exits[k][0] == i && game->exits[k][1] == j)
				{
					flag = 1;
					break;
				}
			}
			if(flag)
				game->state[i][j] = 2;
			else if(i == 0 || j == 0 || i == game->size - 1 || j == game->size - 1)
				game->state[i][j] = 1;
			else if(i == game->wolf1->pos[0] && j == game->wolf1->pos[1])
				game->state[i][j] = 3;
			else if(i == game->wolf2->pos[0] && j == game->wolf2->pos[1])
				game->state[i][j] = 4;
			else if(i == game->rabbit->pos[0] && j == game->rabbit->pos[1])
				game->state[i][j] = 5;
			else if(i == game->wolf1->old_pos[0] && j == game->wolf1->old_pos[1])
				game->state[i][j] = 6;
			else if(i == game->wolf2->old_pos[0] && j == game->wolf2->old_pos[1])
				game->state[i][j] = 7;
			else if(i == game->rabbit->old_pos[0] && j == game->rabbit->old_pos[1])
				game->state[i][j] = 8;
			else
				game->state[i][j] = 0;
		}
	}
}

// print game state to console
void print_game(Game *game)
{
	setlocale(LC_ALL, "");

	int i = 0;
	int j = 0;

//	for(j = 0; j < game->size; j++)
//	{
//		for(i = 0; i < game->size; i++)
//		{
//			printf("%d", game->state[i][game->size - 1 -j]);
//		}
//		printf("\n");
//	}

#pragma loopbound min 0 max MAX_BLOCKS
	for(j = 0; j < game->size; j++)
	{
#pragma loopbound min 0 max MAX_BLOCKS
		for(i = 0; i < game->size; i++)
		{
			switch(game->state[i][game->size - 1 -j])
			{
			case 0:
				printf("%lc  ", (wint_t)(9633));
				break;
			case 1:
				printf("%lc  ", (wint_t)(9632));
				break;
			case 2:
				printf("%lc  ", (wint_t)(9678));
				break;
			case 3:
				printf("%lc  ", (wint_t)(game->wolf1->symbol));
				break;
			case 4:
				printf("%lc  ", (wint_t)(game->wolf2->symbol));
				break;
			case 5:
				printf("%lc  ", (wint_t)(game->rabbit->symbol));
				break;
			case 6:
				printf("%lc  ", (wint_t)(9651));
				break;
			case 7:
				printf("%lc  ", (wint_t)(9661));
				break;
			case 8:
				printf("%lc  ", (wint_t)(9675));
				break;
			default:
				printf("%lc  ", (wint_t)(8265));
				break;
			}
		}
		printf("\n");
	}
}

// find the inputs of an animal depending on its location
void get_inputs(Game *game, int pos[], fix16_t inputs[20])
{
	int i = 0;
	int j = 0;

	// printf("Getting inputs for position: [%f, %f]\n", pos[0], pos[1]);

	fix16_t animals[3][2] = {{0, 0}, {0, 0}, {0, 0}};
	int animal_type = 0;
	int temp_pos[2] = {0, 0};

#pragma loopbound min 0 max MAX_BLOCKS
	for(i = 0; i < game->size; i++)
	{
#pragma loopbound min 0 max MAX_BLOCKS
		for(j = 0; j < game->size; j++)
		{
			// test if any animal is found on this point
			if(pos[0] == i && pos[1] == j)
			{
				animal_type = game->state[i][j];
			}
			else if(game->state[i][j] == 3)
			{
				temp_pos[0] = i;
				temp_pos[1] = j;
				animals[0][0] = dist(pos, temp_pos);
				animals[0][1] = angle(pos, temp_pos);
			}
			else if(game->state[i][j] == 4)
			{
				temp_pos[0] = i;
				temp_pos[1] = j;
				animals[1][0] = dist(pos, temp_pos);
				animals[1][1] = angle(pos, temp_pos);
			}
			else if(game->state[i][j] == 5)
			{
				temp_pos[0] = i;
				temp_pos[1] = j;
				animals[2][0] = dist(pos, temp_pos);
				animals[2][1] = angle(pos, temp_pos);
			}
		}
	}
	// add correct animals to inputs
	if(animal_type == 3)  // wolf 1
	{
		inputs[0] = animals[1][0];
		inputs[1] = animals[1][1];
		inputs[2] = animals[2][0];
		inputs[3] = animals[2][1];
	}
	if(animal_type == 4) // wolf 2
	{
		inputs[0] = animals[0][0];
		inputs[1] = animals[0][1];
		inputs[2] = animals[2][0];
		inputs[3] = animals[2][1];
	}
	if(animal_type == 5)  // rabbit
	{
		inputs[0] = animals[0][0];
		inputs[1] = animals[0][1];
		inputs[2] = animals[1][0];
		inputs[3] = animals[1][1];
	}

	// scan 8 directions for inputs
#pragma loopbound min 0 max MAX_INPUTS
	for(i = 0; i < 8; i++)
	{
		int found_obs = 0;
		int found_exit = 0;
		int end = 0;
		int x = pos[0];
		int y = pos[1];
		fix16_t length = 0;

		int x_mod = 0;
		if(2 < i && i < 6)
			x_mod = -1;
		if(i < 2 || i == 7)
			x_mod = 1;
		int y_mod = 0;
		if(0 < i && i < 4)
			y_mod = 1;
		if(i > 4)
			y_mod = -1;

#pragma loopbound min 0 max 9
		while(!(found_obs && found_exit) && !end)
		{
			x += x_mod;
			y += y_mod;
			// length += sqrt(pow(x_mod, 2) + pow(y_mod, 2));
			length = fix16_add(length, fix16_sqrt(fix16_add(
				fix16_mul(fix16_from_int(x_mod), fix16_from_int(x_mod)), 
				fix16_mul(fix16_from_int(y_mod), fix16_from_int(y_mod)))));

            if(x < 0 || y < 0 || x > game->size - 1 || y > game->size - 1)  // outside game boundary
                break;
            if(x == 0 || y == 0 || x == game->size - 1 || y == game->size - 1)  // found game boundary
                end = 1;

            if(game->state[x][y] == 1)  // obstacle found
			{
                found_obs = 1;
				inputs[4 + i * 2] = fix16_div(65536, length);
			}
            if(game->state[x][y] == 2)  // exit found
			{
                found_exit = 1;
                inputs[4 + i * 2 + 1] = fix16_div(65536, length);
			}
		}
        if(!found_obs)
            inputs[4 + i * 2] = 0;
        if(!found_exit)
            inputs[4 + i * 2 + 1] = 0;
	}

	// printf("Inputs are: [");
	// for(int i = 0; i < 20; i++)
	// {
	// 	printf("%f ", inputs[i]);
	// }
	// printf("]\n");
}

// returns the position the rabbit decided to move to
void rabbit_turn(Game *game, int new_pos[2])
{
	fix16_t inputs[20] = {0};
	fix16_t outputs[4] = {0};

	get_inputs(game, game->rabbit->pos, inputs);  // get rabbit inputs

	int i, j, k;
	fix16_t weighted_sum;
	fix16_t hidden[R_NEURONS2];
	RUN_ANN_LAYER_FIXED(inputs, hidden, game->rabbit->ann, 1, i, j, k, weighted_sum, R_NEURONS1, R_NEURONS2)
	RUN_ANN_LAYER_FIXED(hidden, outputs, game->rabbit->ann, 2, i, j, k, weighted_sum, R_NEURONS2, R_NEURONS3)

	// find rabbits new position
	int index = -1;
	fix16_t high = game->move_thresh;

#pragma loopbound min 0 max RABBIT_OUT
	for(i = 0; i < 4; i++)
	{
		if(outputs[i] >= high)
		{
			high = outputs[i];
			index = i;
		}
	}

	if(index == -1)
	{
		new_pos[0] = game->rabbit->pos[0];
		new_pos[1] = game->rabbit->pos[1];
	}
	if(index == 0)
	{
		new_pos[0] = game->rabbit->pos[0] + 1;
		new_pos[1] = game->rabbit->pos[1];
	}
	if(index == 1)
	{
		new_pos[0] = game->rabbit->pos[0];
		new_pos[1] = game->rabbit->pos[1] + 1;
	}
	if(index == 2)
	{
		new_pos[0] = game->rabbit->pos[0] - 1;
		new_pos[1] = game->rabbit->pos[1];
	}
	if(index == 3)
	{
		new_pos[0] = game->rabbit->pos[0];
		new_pos[1] = game->rabbit->pos[1] - 1;
	}
}

// return the wolf1 position to move to
void wolf1_turn(Game *game, int new_pos[2])
{
	fix16_t inputs[20] = {0};
	fix16_t outputs[8] = {0};

	get_inputs(game, game->wolf1->pos, inputs);  // get rabbit inputs

	int i, j, k;
	fix16_t weighted_sum;
	fix16_t hidden[W_NEURONS2];
	RUN_ANN_LAYER_FIXED(inputs, hidden, game->wolf1->ann, 1, i, j, k, weighted_sum, W_NEURONS1, W_NEURONS2)
	RUN_ANN_LAYER_FIXED(hidden, outputs, game->wolf1->ann, 2, i, j, k, weighted_sum, W_NEURONS2, W_NEURONS3)

	// find wolf1 new position
	int index = -1;
	fix16_t high = game->move_thresh;

#pragma loopbound min 0 max WOLF_OUT
	for(i = 0; i < 8; i++)
	{
		if(outputs[i] >= high)
		{
			high = outputs[i];
			index = i;
		}
	}

	if(index == -1)
	{
		new_pos[0] = game->wolf1->pos[0];
		new_pos[1] = game->wolf1->pos[1];
	}
	if(index == 0)
	{
		new_pos[0] = game->wolf1->pos[0] + 1;
		new_pos[1] = game->wolf1->pos[1];
	}
	if(index == 1)
	{
		new_pos[0] = game->wolf1->pos[0] + 1;
		new_pos[1] = game->wolf1->pos[1] + 1;
	}
	if(index == 2)
	{
		new_pos[0] = game->wolf1->pos[0];
		new_pos[1] = game->wolf1->pos[1] + 1;
	}
	if(index == 3)
	{
		new_pos[0] = game->wolf1->pos[0] - 1;
		new_pos[1] = game->wolf1->pos[1] + 1;
	}
	if(index == 4)
	{
		new_pos[0] = game->wolf1->pos[0] - 1;
		new_pos[1] = game->wolf1->pos[1];
	}
	if(index == 5)
	{
		new_pos[0] = game->wolf1->pos[0] - 1;
		new_pos[1] = game->wolf1->pos[1] - 1;
	}
	if(index == 6)
	{
		new_pos[0] = game->wolf1->pos[0];
		new_pos[1] = game->wolf1->pos[1] - 1;
	}
	if(index == 7)
	{
		new_pos[0] = game->wolf1->pos[0] + 1;
		new_pos[1] = game->wolf1->pos[1] - 1;
	}
}

// return the wolf2 position to move to
void wolf2_turn(Game *game, int new_pos[2])
{
	fix16_t inputs[20] = {0};
	fix16_t outputs[8] = {0};

	get_inputs(game, game->wolf2->pos, inputs);  // get rabbit inputs

	int i, j, k;
	fix16_t weighted_sum;
	fix16_t hidden[W_NEURONS2];
	RUN_ANN_LAYER_FIXED(inputs, hidden, game->wolf2->ann, 1, i, j, k, weighted_sum, W_NEURONS1, W_NEURONS2)
	RUN_ANN_LAYER_FIXED(hidden, outputs, game->wolf2->ann, 2, i, j, k, weighted_sum, W_NEURONS2, W_NEURONS3)

	// find wolf2 new position
	int index = -1;
	fix16_t high = game->move_thresh;

#pragma loopbound min 0 max WOLF_OUT
	for(i = 0; i < 8; i++)
	{
		if(outputs[i] >= high)
		{
			high = outputs[i];
			index = i;
		}
	}

	if(index == -1)
	{
		new_pos[0] = game->wolf2->pos[0];
		new_pos[1] = game->wolf2->pos[1];
	}
	if(index == 0)
	{
		new_pos[0] = game->wolf2->pos[0] + 1;
		new_pos[1] = game->wolf2->pos[1];
	}
	if(index == 1)
	{
		new_pos[0] = game->wolf2->pos[0] + 1;
		new_pos[1] = game->wolf2->pos[1] + 1;
	}
	if(index == 2)
	{
		new_pos[0] = game->wolf2->pos[0];
		new_pos[1] = game->wolf2->pos[1] + 1;
	}
	if(index == 3)
	{
		new_pos[0] = game->wolf2->pos[0] - 1;
		new_pos[1] = game->wolf2->pos[1] + 1;
	}
	if(index == 4)
	{
		new_pos[0] = game->wolf2->pos[0] - 1;
		new_pos[1] = game->wolf2->pos[1];
	}
	if(index == 5)
	{
		new_pos[0] = game->wolf2->pos[0] - 1;
		new_pos[1] = game->wolf2->pos[1] - 1;
	}
	if(index == 6)
	{
		new_pos[0] = game->wolf2->pos[0];
		new_pos[1] = game->wolf2->pos[1] - 1;
	}
	if(index == 7)
	{
		new_pos[0] = game->wolf2->pos[0] + 1;
		new_pos[1] = game->wolf2->pos[1] - 1;
	}
}

// update game state with rabbit move and check victory conditions
void run_rabbit(Game *game, int new_pos[2], int score[2])
{
	score[0] = 0;
	score[1] = 0;

	if(!(new_pos[0] == game->rabbit->pos[0] && new_pos[1] == game->rabbit->pos[1]))
	{
		if(check_pos(game, new_pos, 0))  // check for empty space
		{
			game->rabbit->old_pos[0] = game->rabbit->pos[0];
			game->rabbit->old_pos[1] = game->rabbit->pos[1];
			game->rabbit->pos[0] = new_pos[0];
			game->rabbit->pos[1] = new_pos[1];
		}
		else if(check_pos(game, new_pos, 1))  // check for obstacle
		{
			game->rabbit->old_pos[0] = game->rabbit->pos[0];
			game->rabbit->old_pos[1] = game->rabbit->pos[1];
		}
		else if(check_pos(game, new_pos, 2))  // check for exit
		{
			game->rabbit->score += 1;
			game->rabbit->old_pos[0] = game->rabbit->pos[0];
			game->rabbit->old_pos[1] = game->rabbit->pos[1];
			game->rabbit->pos[0] = new_pos[0];
			game->rabbit->pos[1] = new_pos[1];
			score[1] += 1;
		}
		else if(check_pos(game, new_pos, 3) || check_pos(game, new_pos, 4))  // check for wolf1
		{
			game->rabbit->score -= 1;
			game->rabbit->old_pos[0] = game->rabbit->pos[0];
			game->rabbit->old_pos[1] = game->rabbit->pos[1];
			game->rabbit->pos[0] = new_pos[0];
			game->rabbit->pos[1] = new_pos[1];
			score[1] -= 1;
		} 
		else if(check_pos(game, new_pos, 6) || check_pos(game, new_pos, 7) || check_pos(game, new_pos, 8)) // check for old position of other animal
		{
			game->rabbit->old_pos[0] = game->rabbit->pos[0];
			game->rabbit->old_pos[1] = game->rabbit->pos[1];
			game->rabbit->pos[0] = new_pos[0];
			game->rabbit->pos[1] = new_pos[1];
		}
	}
	else
	{
		game->rabbit->old_pos[0] = game->rabbit->pos[0];
		game->rabbit->old_pos[1] = game->rabbit->pos[1];
	}
}

// update game state with wolf1 move and check victory conditions
void run_wolf1(Game *game, int new_pos[2], int score[2])
{
	score[0] = 0;
	score[1] = 0;

	if(!(new_pos[0] == game->wolf1->pos[0] && new_pos[1] == game->wolf1->pos[1]))
	{
		if(check_pos(game, new_pos, 4) || check_pos(game, new_pos, 1) || check_pos(game, new_pos, 2))  // check for obstacle, rabbit hole or other wolf
		{
			game->wolf1->old_pos[0] = game->wolf1->pos[0];
			game->wolf1->old_pos[1] = game->wolf1->pos[1];
		}
		else if(check_pos(game, new_pos, 0))  // check for empty space
		{
			game->wolf1->old_pos[0] = game->wolf1->pos[0];
			game->wolf1->old_pos[1] = game->wolf1->pos[1];
			game->wolf1->pos[0] = new_pos[0];
			game->wolf1->pos[1] = new_pos[1];
		}
		else if(check_pos(game, new_pos, 5))  // check for rabbit
		{
			game->wolf1->score += 1;
			game->wolf1->old_pos[0] = game->wolf1->pos[0];
			game->wolf1->old_pos[1] = game->wolf1->pos[1];
			game->wolf1->pos[0] = new_pos[0];
			game->wolf1->pos[1] = new_pos[1];
			score[0] +=1 ;
		}
		else if(check_pos(game, new_pos, 6) || check_pos(game, new_pos, 7) || check_pos(game, new_pos, 8)) // check for old position of other animal
		{
			game->wolf1->old_pos[0] = game->wolf1->pos[0];
			game->wolf1->old_pos[1] = game->wolf1->pos[1];
			game->wolf1->pos[0] = new_pos[0];
			game->wolf1->pos[1] = new_pos[1];
		}
	}
	else
	{
		game->wolf1->old_pos[0] = game->wolf1->pos[0];
		game->wolf1->old_pos[1] = game->wolf1->pos[1];
	}
}

// update game state with wolf2 move and check victory conditions
void run_wolf2(Game *game, int new_pos[2], int score[2])
{
	score[0] = 0;
	score[1] = 0;

	if(!(new_pos[0] == game->wolf2->pos[0] && new_pos[1] == game->wolf2->pos[1]))
	{
		if(check_pos(game, new_pos, 0))  // check for empty space
		{
			game->wolf2->old_pos[0] = game->wolf2->pos[0];
			game->wolf2->old_pos[1] = game->wolf2->pos[1];
			game->wolf2->pos[0] = new_pos[0];
			game->wolf2->pos[1] = new_pos[1];
		}
		else if(check_pos(game, new_pos, 3) || check_pos(game, new_pos, 1) || check_pos(game, new_pos, 2))  // check for obstacle, rabbit hole or other wolf
		{
			game->wolf2->old_pos[0] = game->wolf2->pos[0];
			game->wolf2->old_pos[1] = game->wolf2->pos[1];
		}
		else if(check_pos(game, new_pos, 5))  // check for rabbit
		{
			game->wolf2->score += 1;
			game->wolf2->old_pos[0] = game->wolf2->pos[0];
			game->wolf2->old_pos[1] = game->wolf2->pos[1];
			game->wolf2->pos[0] = new_pos[0];
			game->wolf2->pos[1] = new_pos[1];
			score[0] +=1 ;
		}
		else if(check_pos(game, new_pos, 6) || check_pos(game, new_pos, 7) || check_pos(game, new_pos, 8)) // check for old position of other animal
		{
			game->wolf2->old_pos[0] = game->wolf2->pos[0];
			game->wolf2->old_pos[1] = game->wolf2->pos[1];
			game->wolf2->pos[0] = new_pos[0];
			game->wolf2->pos[1] = new_pos[1];
		}
	}
	else
	{
		game->wolf2->old_pos[0] = game->wolf2->pos[0];
		game->wolf2->old_pos[1] = game->wolf2->pos[1];
	}
}

// checks the position in the game state for a certain numerical value
int check_pos(Game *game, int pos[2], int val)
{
	if(game->state[pos[0]][pos[1]] == val)
		return 1;
	return 0;
}

// return the distance between 2 points
fix16_t dist(int p1[], int p2[])
{
	fix16_t x_mod = fix16_from_int(p1[0]) - fix16_from_int(p2[0]);
	fix16_t y_mod = fix16_from_int(p1[1]) - fix16_from_int(p2[1]);
	fix16_t d = fix16_sqrt(fix16_add(
				fix16_mul(fix16_from_int(x_mod), fix16_from_int(x_mod)), 
				fix16_mul(fix16_from_int(y_mod), fix16_from_int(y_mod))));
	return fix16_div(65536, d);
}

// return the angle between two points
fix16_t angle(int p1[], int p2[])
{
	fix16_t angle = 0;
	fix16_t m = 0;

    if(p1[0] == p2[0])
    {
        if(p1[1] > p2[1])
            angle = fix16_from_int(270);
        else
            angle = fix16_from_int(90);
    }
    else
    {
		fix16_t x_mod = fix16_from_int(p1[0]) - fix16_from_int(p2[0]);
		fix16_t y_mod = fix16_from_int(p1[1]) - fix16_from_int(p2[1]);
		m = fix16_div(y_mod, x_mod);

        angle = fix16_div(fix16_mul(fix16_atan(m), fix16_from_int(180)), PI);
        if(p1[0] > p2[0])
            angle = fix16_add(angle, fix16_from_int(180));
        if(-fix16_from_int(90) < angle && angle < 0)
            angle = fix16_add(angle, fix16_from_int(360));
    }

    return fix16_div(angle, fix16_from_int(360));
}















