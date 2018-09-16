#include "main.h"

#define ROUNDS  10

#define OVERHEAD    100000
// #define ENFORCER 
#define RUNS    1

int r_newpos[2] = {0, 0};
int w1_newpos[2] = {0, 0};
int w2_newpos[2] = {0, 0};

int main()
{
    srand(time(NULL));

    enforcervars_move_t enf;
    inputs_move_t inputs;
    outputs_move_t outputs;

    enforcervars_position_t penf;
    inputs_position_t pinputs;
    outputs_position_t poutputs;

    enforcervars_score_t senf;
    inputs_score_t sinputs;
    outputs_score_t soutputs;

    #ifdef OVERHEAD
    float time_taken[4] = {0, 0, 0, 0};
    int ticks_run[4] = {0, 0, 0, 0};
    for(int timing = 0; timing < OVERHEAD; timing++)
    {
        printf("Timing tick: %d\n", timing);
        for(int sys = 0; sys < 4; sys++)
        {
            clock_t start, end;
		    start = clock();
    #else 
    int sys = 0;
    #endif

    int score[2] = {0, 0};
    for(int i = 0; i < RUNS; i++)
    {
        // initialisation
        init();
        playgame_reset();
        playgame();

        // Init enforcer inputs
        if(sys == 0)
        {

        }
        if(sys == 1)
        {
            move_init_all_vars(&enf, &inputs, &outputs);
            inputs.rx = this_game->rabbit->pos[0];
            inputs.ry = this_game->rabbit->pos[1];
            inputs.w1x = this_game->wolf1->pos[0];
            inputs.w1y = this_game->wolf1->pos[1];
            inputs.w2x = this_game->wolf2->pos[0];
            inputs.w2y = this_game->wolf2->pos[1];
            inputs.exit1x = this_game->exits[0][0];
            inputs.exit1y = this_game->exits[0][1];
            inputs.exit2x = this_game->exits[1][0];
            inputs.exit2y = this_game->exits[1][1];
            inputs.exit3x = this_game->exits[2][0];
            inputs.exit3y = this_game->exits[2][1];
        }
        if(sys == 2)
        {
            position_init_all_vars(&penf, &pinputs, &poutputs);
            pinputs.rx = this_game->rabbit->pos[0];
            pinputs.ry = this_game->rabbit->pos[1];
            pinputs.w1x = this_game->wolf1->pos[0];
            pinputs.w1y = this_game->wolf1->pos[1];
            pinputs.w2x = this_game->wolf2->pos[0];
            pinputs.w2y = this_game->wolf2->pos[1];
        }
        if(sys == 3)
        {
            score_init_all_vars(&senf, &sinputs, &soutputs);
            sinputs.rx = this_game->rabbit->pos[0];
            sinputs.ry = this_game->rabbit->pos[1];
            sinputs.w1x = this_game->wolf1->pos[0];
            sinputs.w1y = this_game->wolf1->pos[1];
            sinputs.w2x = this_game->wolf2->pos[0];
            sinputs.w2y = this_game->wolf2->pos[1];
            sinputs.exit1x = this_game->exits[0][0];
            sinputs.exit1y = this_game->exits[0][1];
            sinputs.exit2x = this_game->exits[1][0];
            sinputs.exit2y = this_game->exits[1][1];
            sinputs.exit3x = this_game->exits[2][0];
            sinputs.exit3y = this_game->exits[2][1];
        }

        // run the game until score or out of rounds
        for(int i = 0; i < ROUNDS; i++)
        {
            ticks_run[sys]++;

            // Run Esterel controller
            playgame_I_decide();
            playgame();

            // Update enforcer outputs
            if(sys == 0)
            {
                
            }
            if(sys == 1)
            {
                outputs.new_rx = r_newpos[0];
                outputs.new_ry = r_newpos[1];
                outputs.new_w1x = w1_newpos[0];
                outputs.new_w1y = w1_newpos[1];
                outputs.new_w2x = w2_newpos[0];
                outputs.new_w2y = w2_newpos[1];
            }
            if(sys == 2)
            {
                poutputs.new_rx = r_newpos[0];
                poutputs.new_ry = r_newpos[1];
                poutputs.new_w1x = w1_newpos[0];
                poutputs.new_w1y = w1_newpos[1];
                poutputs.new_w2x = w2_newpos[0];
                poutputs.new_w2y = w2_newpos[1];
            }
            if(sys == 3)
            {
                soutputs.new_rx = r_newpos[0];
                soutputs.new_ry = r_newpos[1];
                soutputs.new_w1x = w1_newpos[0];
                soutputs.new_w1y = w1_newpos[1];
                soutputs.new_w2x = w2_newpos[0];
                soutputs.new_w2y = w2_newpos[1];
            }

            // printf("Game exits: 1 -> [%d, %d], 2 -> [%d, %d], 3 -> [%d, %d]\n", inputs.exit1x, inputs.exit1y, inputs.exit2x, 
                                                                                // inputs.exit2y, inputs.exit3x, inputs.exit3y);


            // RABBIT'S UPDATE
            if(sys == 0)
            {

            }
            if(sys == 1)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                inputs.w2x = this_game->wolf2->pos[0];
                inputs.w2y = this_game->wolf2->pos[1];
                // printf("Before: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
                // Run enforcer
                move_run_output_enforcer_safe(&enf, &inputs, &outputs);
                // Update system outputs
                r_newpos[0] = outputs.new_rx;
                r_newpos[1] = outputs.new_ry;
                // Update system
                // printf("After: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
            }
            if(sys == 2)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                pinputs.w2x = this_game->wolf2->pos[0];
                pinputs.w2y = this_game->wolf2->pos[1];
                // printf("Before: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
                // Run enforcer
                position_run_output_enforcer_safe(&penf, &pinputs, &poutputs);
                // Update system outputs
                r_newpos[0] = poutputs.new_rx;
                r_newpos[1] = poutputs.new_ry;
                // Update system
                // printf("After: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
            }
            if(sys == 3)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                sinputs.w2x = this_game->wolf2->pos[0];
                sinputs.w2y = this_game->wolf2->pos[1];
                // printf("Before: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
                // Run enforcer
                score_run_output_enforcer_safe(&senf, &sinputs, &soutputs);
                // Update system outputs
                r_newpos[0] = soutputs.new_rx;
                r_newpos[1] = soutputs.new_ry;
                // Update system
                // printf("After: Rabbit moving from [%d, %d] to: [%d, %d]\n", inputs.rx, inputs.ry, outputs.new_rx, outputs.new_ry);
            }
            rabbit_move(r_newpos);

            // WOLF 1'S UPDATE
            if(sys == 0)
            {
                
            }
            if(sys == 1)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                inputs.rx = this_game->rabbit->pos[0];
                inputs.ry = this_game->rabbit->pos[1];
                // Run enforcer
                move_run_output_enforcer_safe(&enf, &inputs, &outputs);
                // Update system outputs
                w1_newpos[0] = outputs.new_w1x;
                w1_newpos[1] = outputs.new_w1y;
                // Update system
                // printf("Wolf 1 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf1->pos[0], this_game->wolf1->pos[1], w1_newpos[0], w1_newpos[1]);
            }
            if(sys == 2)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                pinputs.rx = this_game->rabbit->pos[0];
                pinputs.ry = this_game->rabbit->pos[1];
                // Run enforcer
                position_run_output_enforcer_safe(&penf, &pinputs, &poutputs);
                // Update system outputs
                w1_newpos[0] = poutputs.new_w1x;
                w1_newpos[1] = poutputs.new_w1y;
                // Update system
                // printf("Wolf 1 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf1->pos[0], this_game->wolf1->pos[1], w1_newpos[0], w1_newpos[1]);
            }
            if(sys == 3)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                sinputs.rx = this_game->rabbit->pos[0];
                sinputs.ry = this_game->rabbit->pos[1];
                // Run enforcer
                score_run_output_enforcer_safe(&senf, &sinputs, &soutputs);
                // Update system outputs
                w1_newpos[0] = soutputs.new_w1x;
                w1_newpos[1] = soutputs.new_w1y;
                // Update system
                // printf("Wolf 1 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf1->pos[0], this_game->wolf1->pos[1], w1_newpos[0], w1_newpos[1]);
            }
            wolf1_move(w1_newpos);

            // WOLF 2'S UPDATE
            if(sys == 0)
            {

            }
            if(sys == 1)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                inputs.w1x = this_game->wolf1->pos[0];
                inputs.w1y = this_game->wolf1->pos[1];
                // Run enforcer
                move_run_output_enforcer_safe(&enf, &inputs, &outputs);
                // Update system outputs
                w2_newpos[0] = outputs.new_w2x;
                w2_newpos[1] = outputs.new_w2y;
                // Update system
                // printf("Wolf 2 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf2->pos[0], this_game->wolf2->pos[1], w2_newpos[0], w2_newpos[1]);
            }
            if(sys == 2)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                pinputs.w1x = this_game->wolf1->pos[0];
                pinputs.w1y = this_game->wolf1->pos[1];
                // Run enforcer
                position_run_output_enforcer_safe(&penf, &pinputs, &poutputs);
                // Update system outputs
                w2_newpos[0] = poutputs.new_w2x;
                w2_newpos[1] = poutputs.new_w2y;
                // Update system
                // printf("Wolf 2 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf2->pos[0], this_game->wolf2->pos[1], w2_newpos[0], w2_newpos[1]);
            }
            if(sys == 3)
            {
                // printf("Enforcer is in state: %d\n", enf._policy_safe_state);
                // Update enforcer inputs
                sinputs.w1x = this_game->wolf1->pos[0];
                sinputs.w1y = this_game->wolf1->pos[1];
                // Run enforcer
                score_run_output_enforcer_safe(&senf, &sinputs, &soutputs);
                // Update system outputs
                w2_newpos[0] = soutputs.new_w2x;
                w2_newpos[1] = soutputs.new_w2y;
                // Update system
                // printf("Wolf 2 moving from [%d, %d] to: [%d, %d]\n", this_game->wolf2->pos[0], this_game->wolf2->pos[1], w2_newpos[0], w2_newpos[1]);
            }
            wolf2_move(w2_newpos);

            // Check score for game over
            if(check_score(score)) break;
        }
        // printf("\n");
    }
    
    #ifndef OVERHEAD
    printf("Score after %d runs is: R -> %d (%f%%) to W -> %d (%f%%). Total: %d (%f%%). None: %d (%f%%)\n", RUNS, 
            score[0], (float)score[0] * 100.0 / (float)RUNS, score[1], (float)score[1] * 100.0 / (float)RUNS, score[0] + score[1], 
            (float)(score[0] + score[1]) * 100.0 / (float)RUNS, RUNS - (score[0] + score[1]), (float)(RUNS - (score[0] + score[1])) * 100.0 / (float)RUNS);

    FILE * fp;
    #ifdef ENFORCER
    fp = fopen("score_enf.txt", "a");
    #else
    fp = fopen("score_nenf.txt", "a");
    #endif

    fprintf(fp, "Score after %d runs is: R -> %d (%f%%) to W -> %d (%f%%). Total: %d (%f%%). None: %d (%f%%)\n", RUNS, 
            score[0], (float)score[0] * 100.0 / (float)RUNS, score[1], (float)score[1] * 100.0 / (float)RUNS, score[0] + score[1], 
            (float)(score[0] + score[1]) * 100.0 / (float)RUNS, RUNS - (score[0] + score[1]), (float)(RUNS - (score[0] + score[1])) * 100.0 / (float)RUNS);

    fclose(fp);
    #endif

    #ifdef OVERHEAD
            end = clock();
		    time_taken[sys] += ((double) (end - start)) / CLOCKS_PER_SEC;
        }
    }

    printf("Time taken over %d ticks for the unenforced system: %f. Average time taken: %f\n", ticks_run[0], time_taken[0], time_taken[0] / (float)ticks_run[0]);
    printf("Time taken over %d ticks for the fully enforced system: %f. Average time taken: %f\n", ticks_run[1], time_taken[1], time_taken[1] / (float)ticks_run[1]);
    printf("Time taken over %d ticks for the position enforced system: %f. Average time taken: %f\n", ticks_run[2], time_taken[2], time_taken[2] / (float)ticks_run[2]);
    printf("Time taken over %d ticks for the score enforced system: %f. Average time taken: %f\n", ticks_run[3], time_taken[3], time_taken[3] / (float)ticks_run[3]);

    FILE * fp;
    fp = fopen("rabbit_timing.txt", "a");

    fprintf(fp, "Time taken over %d ticks for the unenforced system: %f. Average time taken: %f\n", ticks_run[0], time_taken[0], time_taken[0] / (float)ticks_run[0]);
    fprintf(fp, "Time taken over %d ticks for the fully enforced system: %f. Average time taken: %f\n", ticks_run[1], time_taken[1], time_taken[1] / (float)ticks_run[1]);
    fprintf(fp, "Time taken over %d ticks for the position enforced system: %f. Average time taken: %f\n", ticks_run[2], time_taken[2], time_taken[2] / (float)ticks_run[2]);
    fprintf(fp, "Time taken over %d ticks for the score enforced system: %f. Average time taken: %f\n\n", ticks_run[3], time_taken[3], time_taken[3] / (float)ticks_run[3]);

    fclose(fp);
    #endif

    return 0;
}

void playgame_O_rx(int x)
{
    r_newpos[0] = x;
}
void playgame_O_ry(int y)
{
    r_newpos[1] = y;
}
void playgame_O_w1x(int x)
{
    w1_newpos[0] = x;
}
void playgame_O_w1y(int y)
{
    w1_newpos[1] = y;
}
void playgame_O_w2x(int x)
{
    w2_newpos[0] = x;
}
void playgame_O_w2y(int y)
{
    w2_newpos[1] = y;
}

void move_run(inputs_move_t* inputs, outputs_move_t* outputs){}
void position_run(inputs_position_t* inputs, outputs_position_t* outputs){}
void score_run(inputs_score_t* inputs, outputs_score_t* outputs){}