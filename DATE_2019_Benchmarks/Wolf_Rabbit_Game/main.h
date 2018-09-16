#include "playgame.h"
#include "F_move.h"
#include "F_position.h"
#include "F_score.h"
#include <stdio.h>

void move_run(inputs_move_t* inputs, outputs_move_t* outputs);
void position_run(inputs_position_t* inputs, outputs_position_t* outputs);
void score_run(inputs_score_t* inputs, outputs_score_t* outputs);