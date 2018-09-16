#include "playgame.h"
// #include "game.c"
// #include "ann.c"

#define FIXED_ONE 65536
// #define DEBUG

// struct declarations
Game p_game;
Animal p_wolf1;
Animal p_wolf2;
Animal p_rabbit;
ANN_F w1_ann;
ANN_F w2_ann;
ANN_F r_ann;
Animal *this_wolf1 = &p_wolf1;
Animal *this_wolf2 = &p_wolf2;
Animal *this_rabbit = &p_rabbit;

int this_score[2] = {0, 0};

// init function
void init()
{
	// printf("<====== Initializing system ======>\n");

	this_wolf1->ann = &w1_ann;
	this_wolf2->ann = &w2_ann;
	this_rabbit->ann = &r_ann; 

	int pos[2] = {0, 0};
	int num_layers = 3;
	int bias = 1;
	int activation[2] = {0, 0};
	int w_layers[3] = {20, 30, 8};
	int r_layers[3] = {20, 24, 4};
	init_animal(this_rabbit, 0, pos, 9679, num_layers, r_layers, bias, activation, 0);
	init_animal(this_wolf1, 0, pos, 9650, num_layers, w_layers, bias, activation, 1);
	init_animal(this_wolf2, 0, pos, 9660, num_layers, w_layers, bias, activation, 2);

	int rounds = 1;
	int score = 0;
	int size = 10;
	int turns = 10;
	int num_exits = 3;
	fix16_t move_thresh = fix16_from_float(0.1);
	init_game(this_wolf1, this_wolf2, this_rabbit, this_game, rounds, score, size, turns, num_exits, move_thresh);
}

void rabbit_decide(int * x, int * y)
{
	#ifdef DEBUG
	printf("\nRabbit is deciding its move\n");
	#endif

	int newpos[2];
	rabbit_turn(this_game, newpos);

	*x = newpos[0];
	*y = newpos[1];
}
void wolf1_decide(int * x, int * y)
{
	#ifdef DEBUG
	printf("Wolf 1 is deciding its move\n");
	#endif
	
	int newpos[2];
	wolf1_turn(this_game, newpos);

	*x = newpos[0];
	*y = newpos[1];
}
void wolf2_decide(int * x, int * y)
{
	#ifdef DEBUG
	printf("Wolf 2 is deciding its move\n");
	#endif

	int newpos[2];
	wolf2_turn(this_game, newpos);

	*x = newpos[0];
	*y = newpos[1];
}

// animal move functions
void rabbit_move(int newpos[2])
{
	run_rabbit(this_game, newpos, this_score);
	update_game(this_game);
	#ifdef DEBUG
	printf("Moving rabbit\n");
	print_game(this_game);
	#endif
}
void wolf1_move(int newpos[2])
{
	run_wolf1(this_game, newpos, this_score);
	update_game(this_game);
	#ifdef DEBUG
	printf("Moving wolf 1\n");
	print_game(this_game);
	#endif
}
void wolf2_move(int newpos[2])
{
	run_wolf2(this_game, newpos, this_score);
	update_game(this_game);
	#ifdef DEBUG
	printf("Moving wolf 2\n");
	print_game(this_game);
	#endif
}

int check_score(int score[2])
{
	#ifdef DEBUG
	printf("Checking for end game conditions\n");
	#endif
	if(this_game->rabbit->score == 1)
	{
		#ifdef DEBUG
		printf("Rabbit scored!\n");
	#endif
		score[0]++;
	}
	else if(this_game->rabbit->score == -1)
	{
		#ifdef DEBUG
		printf("Rabbit was penalized\n");
	#endif
		score[1]++;
	}
	else if(this_game->wolf1->score == 1 || this_game->wolf2->score == 1)
	{
		#ifdef DEBUG
		printf("Wolves scored!\n");
	#endif
		score[1]++;
	}
	else
	{
		#ifdef DEBUG
		printf("No one scored...\n");
		#endif
		return 0;
	}
	return 1;
}















