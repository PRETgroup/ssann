#include "main.h"

#define USE_TRAINED_ALL
#define PARTIAL
#define RUN_FROM            5
#define RUN_TO              5
#define USE_TRAINED         2
#define DEBUG
// #define DEBUG_GA
#define TOP_NUM             10

#define RUN_ENFORCER_BASIC  10000
// #define RUN_NO_ENFORCER     1000
#define NUM_TICKS           10
// #define RUN_GA              1000
// #define TEST_GA             100000

// GA scoring

// Speed penalties
#define NOT_MOVING          100
#define STOPPING_PENALTY    2
#define ACC_PENALTY         1
#define CONST_PENALTY       1
#define TOO_SLOW            (speed2 == 0 ? 3 : 1)
#define TOO_FAST            (speed2 == 2 ? 1 : 1)

// Scoring type modifiers
#define BRAKE_PENALTY       1
#define ACCIDENT_PENALTY    0
#define TICKS_PENALTY       0

// Other modifiers
#define SIDES_MOD           3
#define PED_MOD             2
#define CAR_MOD             1
#define DEV_MOD             100

int epochs_trained[6] = {0, 1, 10, 100, 1000, 10000};

int actions_taken[3];

#ifdef RUN_GA
int num_accidents[POP];
int accident_mod[POP];
int ticks_to_accident[POP];
int run_flags[POP];
int speed_mod[POP];
int avg_speed[POP];
int run_ticks[POP]; 
int actions_taken_total[POP][3];
#ifdef TOP_NUM
float topX[RUN_GA + 1][TOP_NUM][4];
float bottomX[RUN_GA + 1][TOP_NUM][4];
#endif
#else
int run_ticks = 0;
int avg_speed = 0;
int bad_brakes = 0;
int enf_state[2] = {0, 0};
int ticks_to_accident = 0;
#endif

void fitness_function(GA * ga)
{
    #ifdef RUN_GA
    for(int i = 0; i < POP; i++)
    {
        ga->scores[i] = accident_mod[ga->positions[i]];
        ga->scores[i] -= ticks_to_accident[ga->positions[i]] * TICKS_PENALTY;

        if(num_accidents[ga->positions[i]] < (int)(0.1 * RUN_NO_ENFORCER) && ((float)avg_speed[ga->positions[i]] / (float)(run_ticks[ga->positions[i]])) > 0.8)  // huge bonus for doing well
            speed_mod[ga->positions[i]] -= 999999;
        else if(num_accidents[ga->positions[i]] < (int)(0.3 * RUN_NO_ENFORCER) && ((float)avg_speed[ga->positions[i]] / (float)(run_ticks[ga->positions[i]])) > 0.8)  // huge bonus for doing well
            speed_mod[ga->positions[i]] -= 99999;
        else if(num_accidents[ga->positions[i]] < (int)(0.5 * RUN_NO_ENFORCER) && ((float)avg_speed[ga->positions[i]] / (float)(run_ticks[ga->positions[i]])) > 0.8)  // huge bonus for doing well
            speed_mod[ga->positions[i]] -= 9999;
        else if(num_accidents[ga->positions[i]] < (int)(0.7 * RUN_NO_ENFORCER) && ((float)avg_speed[ga->positions[i]] / (float)(run_ticks[ga->positions[i]])) > 0.8)  // huge bonus for doing well
            speed_mod[ga->positions[i]] -= 999;

        // printf("Actions: ACC: %d, SB: %d, FB: %d\n", actions_taken_total[ga->positions[i]][0], actions_taken_total[ga->positions[i]][1], actions_taken_total[ga->positions[i]][2]);
        // printf("Run ticks: %d\n", (int)(0.9 * (float)(run_ticks[ga->positions[i]])));

        if(actions_taken_total[ga->positions[i]][0] >= (int)(0.7 * (float)(run_ticks[ga->positions[i]])) || 
            actions_taken_total[ga->positions[i]][1] >= (int)(0.7 * (float)(run_ticks[ga->positions[i]])) || 
            actions_taken_total[ga->positions[i]][2] >= (int)(0.7 * (float)(run_ticks[ga->positions[i]]))
        ) // large penatly for no deviation in action
        {
            speed_mod[ga->positions[i]] += 5000;
        }
        else
        {
            int highest = 0;
            for(int j = 0; j < FIN_NEURONS3; j++)
                if(actions_taken_total[ga->positions[i]][j] > highest)
                    highest = actions_taken_total[ga->positions[i]][j];
            speed_mod[ga->positions[i]] += (int)((float)highest * DEV_MOD / (float)run_ticks[ga->positions[i]]);
        }

        if(actions_taken_total[ga->positions[i]][0] < (int)(0.1 * (float)(run_ticks[ga->positions[i]])) &&
            actions_taken_total[ga->positions[i]][1] < (int)(0.1 * (float)(run_ticks[ga->positions[i]])) && 
            actions_taken_total[ga->positions[i]][2] < (int)(0.1 * (float)(run_ticks[ga->positions[i]]))
        ) // large penatly for no deviation in action
        {
            speed_mod[ga->positions[i]] += 5000;
        }
        else
        {
            // int lowest = 9999999;
            // for(int j = 0; j < FIN_NEURONS3; j++)
            //     if(actions_taken_total[ga->positions[i]][j] < lowest)
            //         lowest = actions_taken_total[ga->positions[i]][j];
            // speed_mod[ga->positions[i]] += (int)((1.0 - (float)lowest / (float)run_ticks[ga->positions[i]]) * DEV_MOD);
        }

        // float deviation = ((float)actions_taken_total[ga->positions[i]][0] / (float)(run_ticks[ga->positions[i]])) * 
        //                     ((float)actions_taken_total[ga->positions[i]][1] / (float)(run_ticks[ga->positions[i]])) * 
        //                     ((float)actions_taken_total[ga->positions[i]][2] / (float)(run_ticks[ga->positions[i]]));

        // speed_mod[ga->positions[i]] -= (int)(deviation * DEV_MOD);

        ga->scores[i] += speed_mod[ga->positions[i]];

        if(ga->scores[i] <= 0) // negative scores are the best, but -100 is better than -10
        {
            ga->scores[i] *= -1; // times by -1 to make better than the positive scores
        }
        else
        {
            ga->scores[i] = 1.0/ga->scores[i]; // invert scores to order by best to worst (higher score is actually worse)
        }

        // printf("Score %0.4f: \tAccident mod: %d,\tSpeed mod: %d\n", ga->scores[i], accident_mod[ga->positions[i]], speed_mod[ga->positions[i]]);
    }
    #endif
}

int main(int argc, char ** argv)
{
    #ifdef USE_TRAINED_ALL
    for(int trained_epochs = RUN_FROM; trained_epochs < RUN_TO + 1; trained_epochs++)
    {
        run_ticks = 0;
        avg_speed = 0;
        bad_brakes = 0;
        actions_taken[0] = 0;
        actions_taken[1] = 0;
        actions_taken[2] = 0;
        enf_state[0] = 0;
        enf_state[1] = 0;
    #endif
    srand(time(NULL));

    char * fails_list[6] = {
        "Collision with a pedestrian ahead and to the left of the vehicle\n",
        "Collision with a pedestrian directly ahead of the vehicle\n",
        "Collision with a pedestrian ahead and to the right of the vehicle\n",
        "A pedestrian walked into the back of the vehicle\n",
        "Collision with a vehicle directly ahead of the vehicle\n",
        "Collision with a vehicle directly behind the vehicle\n", 
    };

    #ifndef RUN_GA
    enforcervars_av_t enf;
    inputs_av_t inputs;
    outputs_av_t outputs;
    av_init_all_vars(&enf, &inputs, &outputs);
    #endif

    actions_taken[0] = 0;
    actions_taken[1] = 0;
    actions_taken[2] = 0;

    #ifdef RUN_NO_ENFORCER
    int fails_no_enforcer[6] = {0, 0, 0, 0, 0, 0};
    float time_no_enforcer = 0;
    int num_ticks = 0;
    #endif
    #ifdef RUN_ENFORCER_BASIC
    int fails_enforcer_basic[6] = {0, 0, 0, 0, 0, 0};
    float time_enforcer_basic = 0;
    int num_ticks = 0;
    #endif
    #ifdef RUN_GA
    GA * ga = (GA *)malloc(sizeof(GA));
    if (ga == 0)
    {
        printf("ERROR: Out of memory: p\n"); 
        return -1;
    }
    for(int i = 0; i < POP; i++)
    {
        ga->population[i] = (ANN_F *)malloc(sizeof(ANN_F));
        if (ga->population[i] == 0)
        {
            printf("ERROR: Out of memory: p\n"); 
            return -1;
        }
    }
    #ifdef DEBUG_GA
    printf("<========================= INITIALISING: GA         ==========================>\n");
    #endif

    #ifdef TEST_GA
    char folder[50];
    sprintf(folder, "backup/ANN10000", TEST_GA);
    ga_init_file(ga, folder);
    #else
    ga_init(ga);
    #endif
    #endif
 
    #ifdef RUN_GA
    plant *avs[POP];
    // =========== MALLOC STRUCTS =============
    for(int i = 0; i < POP; i++)
    {
        avs[i] = (plant *)malloc(sizeof(plant));
        if (avs[i] == 0)
        {
            printf("ERROR: Out of memory: p\n"); 
            return -1;
        }
        avs[i]->l = (lidar *)malloc(sizeof(lidar));
        if (avs[i]->l == 0)
        {
            printf("ERROR: Out of memory: avs[i]->l\n");
            return -1;
        }
    }
    // ============== END MALLOC ====================
    #else
    plant * av;
    // =========== MALLOC STRUCTS =============
    av = (plant *)malloc(sizeof(plant));
    if (av == 0)
    {
        printf("ERROR: Out of memory: p\n"); 
        return -1;
    }
    av->l = (lidar *)malloc(sizeof(lidar));
    if (av->l == 0)
    {
        printf("ERROR: Out of memory: av->l\n");
        return -1;
    }
    // for(int i = 0; i < CAMERAS; i++)
    // {
    //     av->cameras[i] = (tmr *)malloc(sizeof(tmr));
    //     if (av->cameras[i] == 0)
    //     {
    //         printf("ERROR: Out of memory: av->cameras[i]\n");
    //         return -1;
    //     }
    // }
    // ============== END MALLOC ====================
    #endif

    #ifdef RUN_GA
    #ifdef TEST_GA
    ga->last_epoch = 1;
    for(int epochs = 0; epochs < 1; epochs++)  // open GA epoch loop
    {
    #else
    int epochs = 0;
    for(epochs = 0; epochs < RUN_GA + 1; epochs++)  // open GA epoch loop
    {
    #endif
    #ifdef DEBUG_GA
    printf("\n<========================= GA: EPOCH %d             ==========================>\n\n", epochs);
    #endif
    for(int i = 0; i < POP; i++)  // initialise time to accident array and num accidents array for GA
    {
        ticks_to_accident[i] = 0;
        num_accidents[i] = 0;
        avg_speed[i] = 0;
        accident_mod[i] = 0;
        run_ticks[i] = 0;
        speed_mod[i] = 0;
        actions_taken_total[i][0] = 0;
        actions_taken_total[i][1] = 0;
        actions_taken_total[i][2] = 0;
    }
    #endif
    #ifdef RUN_NO_ENFORCER
    for(int runs = 0; runs < RUN_NO_ENFORCER; runs++)
    #endif
    #ifdef RUN_ENFORCER_BASIC 
    // while(flag)
    for(int runs = 0; runs < RUN_ENFORCER_BASIC; runs++)
    #endif
    {      
        ticks_to_accident = 0;
        // Initialisation
        #ifdef USE_TRAINED_ALL

        #ifdef RUN_GA
        for(int i = 0; i < POP; i++)
        {
            plant_init(avs[i], 0, 0, epochs_trained[trained_epochs]);
        }
        #else
        #ifdef DEBUG
        printf("<========================= INITIALISING: PLANT      ==========================>\n");
        #endif
        plant_init(av, runs, 1, epochs_trained[trained_epochs]);
        #ifdef DEBUG
        printf("<========================= INITIALISING: CONTROLLER ==========================>\n");
        #endif
        controller_init(epochs_trained[trained_epochs]);
        #endif
        #else
        #ifdef RUN_GA
        for(int i = 0; i < POP; i++)
        {
            plant_init(avs[i], 0, 0, USE_TRAINED);
        }
        #else
        #ifdef DEBUG
        printf("<========================= INITIALISING: PLANT      ==========================>\n");
        #endif
        plant_init(av, runs, 1, USE_TRAINED);
        #ifdef DEBUG
        printf("<========================= INITIALISING: CONTROLLER ==========================>\n");
        #endif
        controller_init(USE_TRAINED);
        #endif
        #endif // end use trained

        #ifdef RUN_GA
        for(int i = 0; i < POP; i++)  // run flags to dictate which systems have had an accident
        {
            run_flags[i] = 1;
        }
        #endif

        // Superloop 
        for(int i = 0; i < NUM_TICKS; i++)
        {
            // re-init enforcer
            #ifndef RUN_GA
            #ifdef RUN_ENFORCER_BASIC
            av_init_all_vars(&enf, &inputs, &outputs);
            #endif
            #endif

            #ifdef DEBUG
            #ifdef DEBUG_GA
            printf("\n<========================= 0. TICK: %d (EPOCH: %d, RUN: %d) =====================>\n", i, epochs, runs);
            #endif
            #ifdef USE_TRAINED_ALL
            printf("\n<========================= 0. TICK: %d (RUN: %d); TRAINED: %d ===============>\n", i, runs, trained_epochs); 
            #else
            printf("\n<========================= 0. TICK: %d (RUN: %d) =====================>\n", i, runs); 
            #endif
            #endif
            clock_t start, end;
            start = clock();

            // Fetch plant inputs
            #ifdef DEBUG
            printf("<========================= 1. FETCHING PLANT INPUTS ==========================>\n");
            #endif
            #ifdef RUN_GA
            for(int j = 0; j < POP; j++)
                if(run_flags[ga->positions[j]]) plant_get_inputs_no_cnns(avs[ga->positions[j]]);
            #else
            plant_get_inputs(av);
            #endif

            // Parse plant inputs
            #ifdef DEBUG
            printf("<========================= 2. PARSING PLANT INPUTS  ==========================>\n");
            #endif
            #ifdef RUN_GA
            for(int j = 0; j < POP; j++)
                if(run_flags[ga->positions[j]]) plant_run_inputs(avs[ga->positions[j]], 0);
            #else

            #ifdef PARTIAL
            plant_run_inputs(av, 0);
            #else
            plant_run_inputs(av, 1);
            #endif
            #endif

            // Send plant inputs to controller
            #ifdef DEBUG
            printf("<========================= 3. PLANT -> CONTROLLER   ==========================>\n");
            #endif

            #ifndef RUN_GA
            // copying inputs into enforcer
            inputs.speed = av->speed;
            inputs.prev_speed = av->prev_speed;

            inputs.lobject0 = av->l->objects[0];
            inputs.lobject1 = av->l->objects[1];
            inputs.lobject2 = av->l->objects[2];
            inputs.lobject3 = av->l->objects[3];
            inputs.lobject4 = av->l->objects[4];

            inputs.cobject0 = cameras->out[0];
            inputs.cobject1 = cameras->out[1];
            inputs.cobject2 = cameras->out[2];
            inputs.cobject3 = cameras->out[3];
            inputs.cobject4 = cameras->out[4];

            inputs.conf0 = cameras->conf[0];
            inputs.conf1 = cameras->conf[1];
            inputs.conf2 = cameras->conf[2];
            inputs.conf3 = cameras->conf[3];
            inputs.conf4 = cameras->conf[4]; 

            inputs.speed0 = av->l->speed[0];
            inputs.speed1 = av->l->speed[1];
            inputs.speed2 = av->l->speed[2];
            inputs.speed3 = av->l->speed[3];
            inputs.speed4 = av->l->speed[4];

            inputs.dir0 = av->l->direction[0];
            inputs.dir1 = av->l->direction[1];
            inputs.dir2 = av->l->direction[2];
            inputs.dir3 = av->l->direction[3];
            inputs.dir4 = av->l->direction[4];            

            #endif

            #ifdef RUN_ENFORCER_BASIC
            // running enforcer
            #ifdef DEBUG
            printf("<========================= 3.1 PLANT -> ENFORCER  ==========================>\n");
            #endif
            av_run_input_enforcer_brake(&enf, &inputs);
            #endif

            #ifdef RUN_GA
            // Train GA instead of using controller
            for(int j = 0; j < POP; j++)
            {
                if(run_flags[ga->positions[j]])
                {
                    // Copy inputs directly into ANNs
                    ga->population[ga->positions[j]]->neurons[0][0] = fix16_from_float((float)avs[ga->positions[j]]->speed / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][1] = fix16_from_float((float)avs[ga->positions[j]]->prev_speed / 2.0);

                    // GA trained on "perfect" knowledge from lidar sensor
                    ga->population[ga->positions[j]]->neurons[0][2] = fix16_from_float((float)avs[ga->positions[j]]->l->objects[0] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][3] = fix16_from_float((float)avs[ga->positions[j]]->l->objects[1] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][4] = fix16_from_float((float)avs[ga->positions[j]]->l->objects[2] / 2.0); 
                    ga->population[ga->positions[j]]->neurons[0][5] = fix16_from_float((float)avs[ga->positions[j]]->l->objects[4] / 2.0); 
                    ga->population[ga->positions[j]]->neurons[0][6] = fix16_from_float((float)avs[ga->positions[j]]->l->direction[0] / 3.0);
                    ga->population[ga->positions[j]]->neurons[0][7] = fix16_from_float((float)avs[ga->positions[j]]->l->direction[1] / 3.0);
                    ga->population[ga->positions[j]]->neurons[0][8] = fix16_from_float((float)avs[ga->positions[j]]->l->direction[2] / 3.0);
                    ga->population[ga->positions[j]]->neurons[0][9] = fix16_from_float((float)avs[ga->positions[j]]->l->direction[4] / 3.0);
                    ga->population[ga->positions[j]]->neurons[0][10] = fix16_from_float((float)avs[ga->positions[j]]->l->speed[0] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][11] = fix16_from_float((float)avs[ga->positions[j]]->l->speed[1] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][12] = fix16_from_float((float)avs[ga->positions[j]]->l->speed[2] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][13] = fix16_from_float((float)avs[ga->positions[j]]->l->speed[4] / 2.0);

                    ga->population[ga->positions[j]]->neurons[0][14] = fix16_from_float((float)avs[ga->positions[j]]->l->objects[3] / 2.0);
                    ga->population[ga->positions[j]]->neurons[0][15] = fix16_from_float((float)avs[ga->positions[j]]->l->direction[3] / 3.0);
                    ga->population[ga->positions[j]]->neurons[0][16] = fix16_from_float((float)avs[ga->positions[j]]->l->speed[3] / 2.0);

                    for(int k = 0; k < FIN_NEURONS1; k++)
                        if(ga->population[ga->positions[j]]->neurons[0][k] < 0)
                            ga->population[ga->positions[j]]->neurons[0][k] = 0;
                }
            }
            #else
            // vehicle stats
            #ifdef PARTIAL
            ctrl->fin_in[0] = (float)inputs.speed / 2.0;
            ctrl->fin_in[1] = (float)inputs.prev_speed / 2.0;

            ctrl->front_in[0] = (float)inputs.lobject0 / 2.0;
            ctrl->front_in[1] = (float)inputs.lobject1 / 2.0;
            ctrl->front_in[2] = (float)inputs.lobject2 / 2.0;
            ctrl->front_in[3] = (float)inputs.lobject4 / 2.0;
            ctrl->front_in[4] = (float)inputs.dir0 / 3.0;
            ctrl->front_in[5] = (float)inputs.dir1 / 3.0;
            ctrl->front_in[6] = (float)inputs.dir2 / 3.0;
            ctrl->front_in[7] = (float)inputs.dir4 / 3.0;
            ctrl->front_in[8] = (float)inputs.speed0 / 2.0;
            ctrl->front_in[9] = (float)inputs.speed1 / 2.0;
            ctrl->front_in[10] = (float)inputs.speed2 / 2.0;
            ctrl->front_in[11] = (float)inputs.speed4 / 2.0;

            ctrl->back_in[0] = (float)inputs.lobject3 / 2.0;
            ctrl->back_in[1] = (float)inputs.dir3 / 3.0;
            ctrl->back_in[2] = (float)inputs.speed3 / 2.0;
            #else
            ctrl->fin_in[0] = (float)inputs.speed / 2.0;
            ctrl->fin_in[1] = (float)inputs.prev_speed / 2.0;

            ctrl->front_in[0] = (float)inputs.cobject0 / 2.0;
            ctrl->front_in[1] = (float)inputs.cobject1 / 2.0;
            ctrl->front_in[2] = (float)inputs.cobject2 / 2.0;
            ctrl->front_in[3] = (float)inputs.cobject4 / 2.0;
            ctrl->front_in[4] = (float)inputs.dir0 / 3.0;
            ctrl->front_in[5] = (float)inputs.dir1 / 3.0;
            ctrl->front_in[6] = (float)inputs.dir2 / 3.0;
            ctrl->front_in[7] = (float)inputs.dir4 / 3.0;
            ctrl->front_in[8] = (float)inputs.speed0 / 2.0;
            ctrl->front_in[9] = (float)inputs.speed1 / 2.0;
            ctrl->front_in[10] = (float)inputs.speed2 / 2.0;
            ctrl->front_in[11] = (float)inputs.speed4 / 2.0;

            ctrl->back_in[0] = (float)inputs.cobject3 / 2.0;
            ctrl->back_in[1] = (float)inputs.dir3 / 3.0;
            ctrl->back_in[2] = (float)inputs.speed3 / 2.0;
            #endif
            #endif

            // Run controller
            #ifdef DEBUG
            printf("<========================= 4. RUN CONTROLLER        ==========================>\n");
            #endif
            #ifdef RUN_GA
            // Train GA instead of using controller
            for(int j = 0; j < POP; j++)
            {
                if(run_flags[ga->positions[j]]) ann_run_fixed(NULL, NULL, ga->population[ga->positions[j]]); // Run and store outputs in ANN struct
            }
            #else
            controller_I_start();
            controller_run_tick();
            run_ticks++;
            #endif

            // Send controller outputs to plant
            #ifdef DEBUG
            printf("<========================= 5. PLANT <- CONTROLLER   ==========================>\n");
            #endif

            // copy outputs
            #ifdef RUN_GA
            // Train GA instead of using controller
            #ifdef DEBUG
            printf("<========================= 6. UPDATE PLANTS         ==========================>\n");
            #endif
            for(int j = 0; j < POP; j++)
            {
                if(run_flags[ga->positions[j]])
                {
                    run_ticks[ga->positions[j]]++;

                    // ANN outputs: x > 0.5 = 1, x < 0.5 = 0
                    int num_layers = ga->population[ga->positions[j]]->num_layers - 1;

                    // highest output becomes chosen action
                    int max = 0;
                    int pos = 0;
                    int min_count = 0;
                    for(int i = 0; i < FIN_NEURONS3; i++)
                    {
                        if(fix16_to_float(ga->population[ga->positions[j]]->neurons[num_layers][i]) < MIN_OUTPUT)
                            min_count++;
                        if(ga->population[ga->positions[j]]->neurons[num_layers][i] > max)
                        {
                            max = ga->population[ga->positions[j]]->neurons[num_layers][i];
                            pos = i;
                        }
                    }
                    for(int i = 0; i < FIN_NEURONS3; i++)
                    {
                        avs[ga->positions[j]]->new_action[i] = 0;
                    }
                    if(min_count < FIN_NEURONS3)
                    {
                        avs[ga->positions[j]]->new_action[pos] = 1;
                        actions_taken_total[ga->positions[j]][pos] += avs[ga->positions[j]]->new_action[pos];
                    }

                    // Run plant on controller outputs (update plant)
                    // plant_print(av);
                    plant_update_speed(avs[ga->positions[j]]);

                    // Check if they car braked for no good reason -> big penalty
                    int speed1 = avs[ga->positions[j]]->prev_speed;  // initial speed of vehicle
                    int speed2 = avs[ga->positions[j]]->speed;  // final speed of vehicle
                    avg_speed[ga->positions[j]] += speed2;

                    if(speed2 == 0)
                    {
                        if((avs[ga->positions[j]]->l->objects[1] == 0 || avs[ga->positions[j]]->l->objects[1] == 2 && speed2 < avs[ga->positions[j]]->l->speed[1]) && 
                            avs[ga->positions[j]]->l->objects[4] != 1 &&
                            !(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->direction[0] == 1) && 
                            !(avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->direction[2] == 3)
                        )
                        {
                            speed_mod[ga->positions[j]] += NOT_MOVING;
                        }
                    }

                    if(speed2 < speed1) // vehicle has slowed down -> bigger penalty
                    {
                        if(avs[ga->positions[j]]->l->objects[1] == 2) // car in front
                        {
                            if(avs[ga->positions[j]]->l->speed[1] > speed2) // car should be going faster
                                speed_mod[ga->positions[j]] += (avs[ga->positions[j]]->l->speed[1] - speed2) * STOPPING_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                            else if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slower, reward!
                                speed_mod[ga->positions[j]] -= (speed2 - avs[ga->positions[j]]->l->speed[1]) * STOPPING_PENALTY * BRAKE_PENALTY;
                            else  // perfect speed, reward!
                                speed_mod[ga->positions[j]] -= STOPPING_PENALTY * BRAKE_PENALTY;
                        }
                        else if(avs[ga->positions[j]]->l->objects[1] == 0) // nothing in front -> car should be going faster
                        {
                            // check ahead ahead position
                            if(avs[ga->positions[j]]->l->objects[4] == 0)
                            {
                                speed_mod[ga->positions[j]] += (2 - speed2) * STOPPING_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                            }
                            else if(avs[ga->positions[j]]->l->objects[4] == 2)
                            {
                                if(avs[ga->positions[j]]->l->speed[1] > speed2) // car should be going faster
                                    speed_mod[ga->positions[j]] += (avs[ga->positions[j]]->l->speed[1] - speed2) * STOPPING_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                                else if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slower, reward!
                                    speed_mod[ga->positions[j]] -= (speed2 - avs[ga->positions[j]]->l->speed[1]) * STOPPING_PENALTY * BRAKE_PENALTY;
                                else  // perfect speed, reward!
                                    speed_mod[ga->positions[j]] -= STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                            else // pedestrian
                            {
                                if(speed2 > 0)
                                {
                                    speed_mod[ga->positions[j]] += speed2 * PED_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                                }
                                else
                                {
                                    speed_mod[ga->positions[j]] -= PED_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                                }
                            }
                        }
                        else // braked before pedestrian, reward!
                        {
                            if(speed2 > 0) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * PED_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else if (speed2 == 0)
                            {
                                speed_mod[ga->positions[j]] -= PED_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }

                        // pedestrian moving into the road from the sides
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 2 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 2 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            if(speed2 > 0) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else
                            {
                                speed_mod[ga->positions[j]] -= SIDES_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 1 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 1 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            if(speed2 > 1) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else
                            {
                                speed_mod[ga->positions[j]] -= SIDES_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }
                    }
                    else if(speed2 > speed1) // vehicle has accelerated
                    {
                        // lessen penalty
                        if(avs[ga->positions[j]]->l->objects[1] == 2) // car in front
                        {
                            if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slowe
                                speed_mod[ga->positions[j]] += (speed2 -avs[ga->positions[j]]->l->speed[1]) * ACC_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            else  // perfect speed (or speedup), reward!
                                speed_mod[ga->positions[j]] -= ACC_PENALTY * BRAKE_PENALTY;
                        }
                        else if(avs[ga->positions[j]]->l->objects[1] == 0) // car accelerated, good
                        {
                            // check ahead ahead position
                            if(avs[ga->positions[j]]->l->objects[4] == 0)
                            {
                                speed_mod[ga->positions[j]] -= speed2 * ACC_PENALTY * BRAKE_PENALTY;
                            }
                            else if(avs[ga->positions[j]]->l->objects[4] == 2)
                            {
                                if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slower
                                    speed_mod[ga->positions[j]] += (speed2 -avs[ga->positions[j]]->l->speed[1]) * ACC_PENALTY *  TOO_FAST * BRAKE_PENALTY;
                                else  // perfect speed (or speedup), reward!
                                    speed_mod[ga->positions[j]] -= ACC_PENALTY * BRAKE_PENALTY;
                            }
                            else // pedestrian
                            {
                                speed_mod[ga->positions[j]] += speed2 * PED_MOD * ACC_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                        }
                        else
                        {
                            speed_mod[ga->positions[j]] += PED_MOD * ACC_PENALTY * TOO_FAST * BRAKE_PENALTY;  // accelerated into a pedestrian, extra penalty
                        }

                        // pedestrian moving into the road from the sides
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 2 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 2 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                        }
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 1 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 1 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            if(speed2 > 1) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else
                            {
                                speed_mod[ga->positions[j]] -= SIDES_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }
                    }
                    else // no change in speed
                    {
                        if(avs[ga->positions[j]]->l->objects[1] == 2) // car in front
                        {
                            if(avs[ga->positions[j]]->l->speed[1] > speed2) // car should be going faster
                                speed_mod[ga->positions[j]] += (avs[ga->positions[j]]->l->speed[1] - speed2) * CONST_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                            else if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slower
                                speed_mod[ga->positions[j]] += (speed2 - avs[ga->positions[j]]->l->speed[1]) * CONST_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            else  // perfect speed, reward!
                                speed_mod[ga->positions[j]] -= CONST_PENALTY * BRAKE_PENALTY;
                        }
                        else if(avs[ga->positions[j]]->l->objects[1] == 0) // nothing in front -> car should be going faster
                        {
                            // check ahead ahead position
                            if(avs[ga->positions[j]]->l->objects[4] == 0)
                            {
                                speed_mod[ga->positions[j]] += (2 - speed2) * CONST_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                            }
                            else if(avs[ga->positions[j]]->l->objects[4] == 2)
                            {
                                if(avs[ga->positions[j]]->l->speed[1] > speed2) // car should be going faster
                                    speed_mod[ga->positions[j]] += (avs[ga->positions[j]]->l->speed[1] - speed2) * CONST_PENALTY * TOO_SLOW * BRAKE_PENALTY;
                                else if(avs[ga->positions[j]]->l->speed[1] < speed2) // car should be going slower
                                    speed_mod[ga->positions[j]] += (speed2 - avs[ga->positions[j]]->l->speed[1]) * CONST_PENALTY * TOO_FAST * BRAKE_PENALTY;
                                else  // perfect speed, reward!
                                    speed_mod[ga->positions[j]] -= CONST_PENALTY * BRAKE_PENALTY;
                            }
                            else // pedestrian
                            {
                                if(speed2 > 0)
                                {
                                    speed_mod[ga->positions[j]] += speed2 * PED_MOD * CONST_PENALTY * TOO_FAST * BRAKE_PENALTY;
                                }
                                else
                                {
                                    speed_mod[ga->positions[j]] -= PED_MOD * CONST_PENALTY * BRAKE_PENALTY;
                                }
                            }
                        }
                        else // braked before pedestrian, reward!
                        {
                            if(speed2 > 0) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * PED_MOD * CONST_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else if (speed2 == 0)
                            {
                                speed_mod[ga->positions[j]] -= PED_MOD * CONST_PENALTY * BRAKE_PENALTY;
                            }
                        }

                        // pedestrian moving into the road from the sides
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 2 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 2 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            if(speed2 > 0) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else
                            {
                                speed_mod[ga->positions[j]] -= SIDES_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }
                        if(avs[ga->positions[j]]->l->objects[0] == 1 && avs[ga->positions[j]]->l->speed[0] == 1 && avs[ga->positions[j]]->l->direction[0] == 1 || 
                        avs[ga->positions[j]]->l->objects[2] == 1 && avs[ga->positions[j]]->l->speed[2] == 1 && avs[ga->positions[j]]->l->direction[2] == 3)
                        {
                            if(speed2 > 1) // did not brake enough... (already penalised?)
                            {
                                speed_mod[ga->positions[j]] += speed2 * SIDES_MOD * STOPPING_PENALTY * TOO_FAST * BRAKE_PENALTY;
                            }
                            else
                            {
                                speed_mod[ga->positions[j]] -= SIDES_MOD * STOPPING_PENALTY * BRAKE_PENALTY;
                            }
                        }
                    }

                    // Update the plant after calculating the cost
                    int accident = plant_run_outputs(avs[ga->positions[j]]);

                    if(accident)
                    {
                        if(accident != 4) 
                        {
                            run_flags[ga->positions[j]] = 0;
                            num_accidents[ga->positions[j]]++;
                            accident_mod[ga->positions[j]] += (accident < 4 ? PED_MOD : CAR_MOD) * ACCIDENT_PENALTY;
                        }
                        else
                        {
                            ticks_to_accident[ga->positions[j]]++;  // increase ticks until the accident occurred
                        }
                    }
                    else
                    {
                        ticks_to_accident[ga->positions[j]]++;  // increase ticks until the accident occurred
                    }
                }
            }
            #else
            outputs.accelerate = ctrl->decision[0];
            outputs.slow_brake = ctrl->decision[1];
            outputs.fast_brake = ctrl->decision[2];

            #ifdef RUN_ENFORCER_BASIC
            // run enforcer on outputs
            #ifdef DEBUG
            printf("------------ ACTIONS BEFORE ENFORCER -----------\n");
            printf("Car is driving at speed %d\n", av->speed);
            printf("Car is undertaking the following action(s):\n");
            printf("ACC: %d, SB: %d, FB: %d\n\n", outputs.accelerate, outputs.slow_brake, outputs.fast_brake);
            printf("Enforcer state (before): %d\n", enf._policy_brake_state);
            printf("<========================= 5.1 PLANT <- ENFORCER    ==========================>\n");
            #endif
            av_run_output_enforcer_brake(&enf, &inputs, &outputs);
            #ifdef DEBUG
            printf("Enforcer state (after): %d\n", enf._policy_brake_state);
            printf("Enforcer I/O:\n");
            printf("speed: %d\n", inputs.speed);
            printf("prev_speed: %d\n", inputs.prev_speed);
            // printf("lobjects: %d; %d; %d; %d; %d\n", inputs.lobject0, inputs.lobject1, inputs.lobject2, inputs.lobject3, inputs.lobject4);
            printf("cobjects: %d; %d; %d; %d; %d\n", inputs.cobject0, inputs.cobject1, inputs.cobject2, inputs.cobject3, inputs.cobject4);
            // printf("confs: %d; %d; %d; %d; %d\n", inputs.conf0, inputs.conf1, inputs.conf2, inputs.conf3, inputs.conf4);
            printf("speeds: %d; %d; %d; %d; %d\n", inputs.speed0, inputs.speed1, inputs.speed2, inputs.speed3, inputs.speed4);
            printf("dirs: %d; %d; %d; %d; %d\n", inputs.dir0, inputs.dir1, inputs.dir2, inputs.dir3, inputs.dir4);
            #endif
            #endif

            av->new_action[0] = outputs.accelerate;
            av->new_action[1] = outputs.slow_brake;
            av->new_action[2] = outputs.fast_brake;

            actions_taken[0] += av->new_action[0];
            actions_taken[1] += av->new_action[1];
            actions_taken[2] += av->new_action[2];

            if(av->new_action[1])
            {
                if(av->l->objects[1] == 0 && av->l->objects[4] == 0 || av->l->objects[1] == 0 && av->l->objects[4] == 2 && av->l->speed[4] >= av->speed)
                {
                    if(!(av->l->objects[0] == 1 && av->l->direction[0] == 1 && av->l->speed[0] == 1))
                    {
                        if(!(av->l->objects[2] == 1 && av->l->direction[2] == 3 && av->l->speed[2] == 1))
                        {
                            bad_brakes++;
                        }
                    }
                }
            }
            else if(av->new_action[2])
            {
                if(av->l->objects[1] == 0 || av->l->objects[1] == 2 && av->l->speed[4] >= av->speed)
                {
                    if(!(av->l->objects[0] == 1 && av->l->direction[0] == 1 && av->l->speed[0] == 2))
                    {
                        if(!(av->l->objects[2] == 1 && av->l->direction[2] == 3 && av->l->speed[2] == 2))
                        {
                            bad_brakes++;
                        }
                    }
                }
            }

            // Run plant on controller outputs (update plant)
            #ifdef DEBUG
            printf("<========================= 6. UPDATE PLANT          ==========================>\n");
            #endif
            // plant_print(av);
            plant_update_speed(av);
            avg_speed += av->speed;
            ticks_to_accident++;
            int accident = plant_run_outputs(av);
            #ifdef RUN_NO_ENFORCER
            end = clock();
            time_no_enforcer += ((double) (end - start)) / CLOCKS_PER_SEC;
            if(accident)
            {
                if(accident != 4) printf("Accident occurred, shutting down AV system\n"); 
                fails_no_enforcer[accident - 1] ++;

                if(accident != 4)
                {
                    break;
                }
            }
            #endif
            #ifdef RUN_ENFORCER_BASIC
            end = clock();
            time_enforcer_basic += (((double) (end - start)) / CLOCKS_PER_SEC);
            if(accident)
            {
                if(accident != 4) printf("Accident occurred, shutting down AV system\n"); 
                fails_enforcer_basic[accident - 1] += 1;
                if(accident != 4)
                {
                    enf_state[enf._policy_brake_state]++;
                    break;
                }
            }
            #endif
            #endif
        }
        if(ticks_to_accident >= NUM_TICKS)
            ticks_to_accident = 1440;
        num_ticks += ticks_to_accident;
    }
    // Destroy malloc'd memory
    #ifndef RUN_GA
    #ifdef DEBUG
    printf("<========================= DESTRUCTOR: PLANT        ==========================>\n");
    #endif
    plant_destroy(av, 0);
    #ifdef DEBUG
    printf("<========================= DESTUCTOR: CONTROLLER    ==========================>\n");
    #endif
    controller_destroy();
    #endif

    #ifndef RUN_GA // Don't print out system stats during GA training

    #ifdef RUN_NO_ENFORCER 
    int num_fails1 = 0;
    for(int i = 0; i < 6; i++)
    {
        if(i != 3)
            num_fails1 += fails_no_enforcer[i];
    }
    printf("Without the enforcer, the system took approximately %f seconds to run per tick\n", time_no_enforcer/(float)run_ticks);
    printf("The system took these actions: ACC: %d, SB: %d, FB: %d\n", actions_taken[0], actions_taken[1], actions_taken[2]);
    printf("The system averaged a speed of %f\n", (float)avg_speed / (float)run_ticks);
    printf("The system averaged %f ticks (/%d) before crashing\n", (float)num_ticks / (float)(RUN_NO_ENFORCER), NUM_TICKS);
    printf("The system braked %d times that were unecessary/dangerous\n", bad_brakes);
    printf("With %d systems tested, each with %d ticks and no enforcer, the list of %d accidents is as follows:\n", RUN_NO_ENFORCER, NUM_TICKS, num_fails1);
    for(int i = 0; i < 6; i++)
    {
        printf("%d failures caused by: %s", fails_no_enforcer[i], fails_list[i]);
    }

    #ifdef USE_TRAINED_ALL
    FILE * fp;
    char buf[50];
    #ifdef PARTIAL
    sprintf(buf, "results/results%d_nenf_partial.txt", epochs_trained[trained_epochs]);
    #else
    sprintf(buf, "results/results%d_nenf.txt", epochs_trained[trained_epochs]);
    #endif
    fp = fopen(buf, "w");

    fprintf(fp, "Without the enforcer, the system took approximately %f seconds to run per tick\n", time_no_enforcer/(float)run_ticks);
    fprintf(fp, "The system took these actions: ACC: %d, SB: %d, FB: %d\n", actions_taken[0], actions_taken[1], actions_taken[2]);
    fprintf(fp, "The system averaged a speed of %f\n", (float)avg_speed / (float)run_ticks);
    fprintf(fp, "The system averaged %f ticks (/%d) before crashing\n", (float)num_ticks / (float)(RUN_NO_ENFORCER), NUM_TICKS);
    fprintf(fp, "The system braked %d times that were unecessary/dangerous\n", bad_brakes);
    fprintf(fp, "With %d systems tested, each with %d ticks and no enforcer, the list of %d accidents is as follows:\n", RUN_NO_ENFORCER, NUM_TICKS, num_fails1);
    for(int i = 0; i < 6; i++)
    {
        fprintf(fp, "%d failures caused by: %s", fails_no_enforcer[i], fails_list[i]);
    }

    fclose(fp);
    #endif

    #endif
    #ifdef RUN_ENFORCER_BASIC
    int num_fails2 = 0;
    for(int i = 0; i < 6; i++)
    {
        if(i != 3)
            num_fails2 += fails_enforcer_basic[i];
    }
    printf("With the basic enforcer, the system took approximately %f seconds to run per tick\n", time_enforcer_basic/(float)run_ticks);
    printf("The system took these actions: ACC: %d, SB: %d, FB: %d\n", actions_taken[0], actions_taken[1], actions_taken[2]);
    printf("The system averaged a speed of %f\n", (float)avg_speed / (float)run_ticks);
    printf("The system averaged %f ticks (/%d) before crashing\n", (float)num_ticks / (float)(RUN_ENFORCER_BASIC), NUM_TICKS);
    printf("The system braked %d times that were unecessary/dangerous\n", bad_brakes);
    printf("The system had %d accidents in state 0 and %d accidents in state 1\n", enf_state[0], enf_state[1]);
    printf("With %d systems tested, each with %d ticks and a basic enforcer, the list of %d accidents is as follows:\n", RUN_ENFORCER_BASIC, NUM_TICKS, num_fails2);
    for(int i = 0; i < 6; i++)
    {
        printf("%d failure(s) caused by: %s", fails_enforcer_basic[i], fails_list[i]);
    }

    #ifdef USE_TRAINED_ALL
    FILE * fp;
    char buf[50];
    #ifdef PARTIAL
    sprintf(buf, "results/results%d_enf_partial.txt", epochs_trained[trained_epochs]);
    #else
    sprintf(buf, "results/results%d_enf.txt", epochs_trained[trained_epochs]);
    #endif
    fp = fopen(buf, "w");

    fprintf(fp, "With the basic enforcer, the system took approximately %f seconds to run per tick\n", time_enforcer_basic/(float)run_ticks);
    fprintf(fp, "The system took these actions: ACC: %d, SB: %d, FB: %d\n", actions_taken[0], actions_taken[1], actions_taken[2]);
    fprintf(fp, "The system averaged a speed of %f\n", (float)avg_speed / (float)run_ticks);
    fprintf(fp, "The system averaged %f ticks (/%d) before crashing\n", (float)num_ticks / (float)(RUN_ENFORCER_BASIC), NUM_TICKS);
    fprintf(fp, "The system braked %d times that were unecessary/dangerous\n", bad_brakes);
    fprintf(fp, "The system had %d accidents in state 0 and %d accidents in state 1\n", enf_state[0], enf_state[1]);
    fprintf(fp, "With %d systems tested, each with %d ticks and a basic enforcer, the list of %d accidents is as follows:\n", RUN_ENFORCER_BASIC, NUM_TICKS, num_fails2);
    for(int i = 0; i < 6; i++)
    {
        fprintf(fp, "%d failure(s) caused by: %s", fails_enforcer_basic[i], fails_list[i]);
    }

    fclose(fp);
    #endif
    #endif

    #endif // Don't print out system stats during GA training

    #ifdef RUN_GA
    #ifdef DEBUG
    printf("<========================= GA: UPDATE POPULATION    ==========================>\n");
    #endif
    // Update the population once (i.e. one epoch) (don't breed on last epoch)
    fitness_function(ga);
    fitness_arrange(ga);
    int in_epochs = 0;
    for(int i = 0; i < 6; i++)
        if(epochs_trained[i] == epochs)
            in_epochs = 1;

    if(in_epochs)
    {
        for(int i = 0; i < TOP_NUM; i++)
        {
            char filename[50];
            sprintf(filename, "backup/ANN10000/epoch%d_top%d.txt", epochs, i);
            // printf("Saving ANN epoch%d_top%d to file\n", epochs, i);
            ann_save_to_file_fixed(ga->population[ga->positions[i]], filename);
        }
    }

    if(epochs == RUN_GA) ga->last_epoch = 1;
    if(!ga->last_epoch) breed(ga);

    int num_show = (POP > 40 ? 40 : POP);
    for(int i = 0; i < num_show; i++)
    {
        if(i < 10)
            printf("Top: %d,\t\tScore %0.4f: \tAccidents: %d,\tSpeed mod: %d", i, ga->scores[i], num_accidents[ga->positions[i]], speed_mod[ga->positions[i]]);
        else
            printf("Top: %d,\tScore %0.4f: \tAccidents: %d,\tSpeed mod: %d", i, ga->scores[i], num_accidents[ga->positions[i]], speed_mod[ga->positions[i]]);
        if(speed_mod[ga->positions[i]] > 9999)
            printf("\tACC: %d, SB: %d, FB: %d (/%d)\n", actions_taken_total[ga->positions[i]][0], 
            actions_taken_total[ga->positions[i]][1], actions_taken_total[ga->positions[i]][2], run_ticks[ga->positions[i]]);
        else
            printf("\t\tACC: %d, SB: %d, FB: %d (/%d)\n", actions_taken_total[ga->positions[i]][0], 
            actions_taken_total[ga->positions[i]][1], actions_taken_total[ga->positions[i]][2], run_ticks[ga->positions[i]]);
    }
    #endif

    #ifdef RUN_GA
    // Print GA "results" and save top X of every epoch
    #ifdef  TOP_NUM
    for(int i = 0; i < TOP_NUM; i++)
    {
        topX[epochs][i][0] = (float)num_accidents[ga->positions[i]];
        topX[epochs][i][1] = ((float)actions_taken_total[ga->positions[i]][0] / (float)(run_ticks[ga->positions[i]])) * 
                            ((float)actions_taken_total[ga->positions[i]][1] / (float)(run_ticks[ga->positions[i]])) * 
                            ((float)actions_taken_total[ga->positions[i]][2] / (float)(run_ticks[ga->positions[i]]));
        topX[epochs][i][2] = (float)avg_speed[ga->positions[i]] / (float)(run_ticks[ga->positions[i]]); 
        topX[epochs][i][3] = ga->scores[i];
    }
    for(int i = TOP_NUM - 1; i >= 0; i--)
    {
        int pos = POP - TOP_NUM + i;
        bottomX[epochs][i][0] = (float)num_accidents[ga->positions[pos]];
        bottomX[epochs][i][1] = ((float)actions_taken_total[ga->positions[pos]][0] / (float)(run_ticks[ga->positions[i]])) * 
                            ((float)actions_taken_total[ga->positions[pos]][1] / (float)(run_ticks[ga->positions[i]])) * 
                            ((float)actions_taken_total[ga->positions[pos]][2] / (float)(run_ticks[ga->positions[i]]));
        bottomX[epochs][i][2] = (float)avg_speed[ga->positions[pos]] / (float)(run_ticks[ga->positions[pos]]);
        bottomX[epochs][i][3] = ga->scores[pos]; 
    }
    #endif
    }  // close GA epoch loop

    #ifndef TEST_GA
    // save top 10 of every epoch's stats to file (for python)
    // Scores:
    #ifdef TOP_NUM
    for(int i = 0; i < TOP_NUM; i++)
    {
        char filename[50];
        sprintf(filename, "backup/ANN10000/epoch%d_top%d.txt", epochs, i);
        // printf("Saving ANN epoch%d_top%d to file\n", epochs, i);
        ann_save_to_file_fixed(ga->population[ga->positions[i]], filename);
    }

    int in_epochs = 0;
    for(int i = 0; i < 6; i++)
        if(epochs_trained[i] == epochs)
            in_epochs = 1;

    if(in_epochs)
    {
        char filename[50];
        sprintf(filename, "backup/ANN10000/bestscores%d.txt", epochs);
        FILE * fp;
        fp = fopen(filename, "w");

        int num_print = 1;
        fprintf(fp, "self.scores = [\n");
        for(int i = 0; i < RUN_GA + 1; i++)
        {
            fprintf(fp, "\t[");
            for(int j = 0; j < num_print; j++)
            {
                fprintf(fp, "%f", topX[i][j][3]);
                if(j < num_print - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, "],\n");
        }
        fprintf(fp, "]\n\n");
        // Accidents:
        fprintf(fp, "\nself.accidents = [\n");
        for(int i = 0; i < RUN_GA + 1; i++)
        {
            fprintf(fp, "\t[");
            for(int j = 0; j < num_print; j++)
            {
                fprintf(fp, "%f", topX[i][j][0]);
                if(j < num_print - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, "],\n");
        }
        fprintf(fp, "]\n\n");
        // Ticks:
        fprintf(fp, "\nself.ticks = [\n");
        for(int i = 0; i < RUN_GA + 1; i++)
        {
            fprintf(fp, "\t[");
            for(int j = 0; j < num_print; j++)
            {
                fprintf(fp, "%f", topX[i][j][1]);
                if(j < num_print - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, "],\n");
        }
        fprintf(fp, "]\n\n");
        // Speeds:
        fprintf(fp, "\nself.speeds = [\n");
        for(int i = 0; i < RUN_GA + 1; i++)
        {
            fprintf(fp, "\t[");
            for(int j = 0; j < num_print; j++)
            {
                fprintf(fp, "%f", topX[i][j][2]);
                if(j < num_print - 1)
                    fprintf(fp, ", ");
            }
            fprintf(fp, "],\n");
        }
        fprintf(fp, "]\n\n");

        fclose(fp);
    }
    #endif
    #endif

    #ifdef DEBUG
    printf("\n<========================= DESTUCTOR: GA           ==========================>\n");
    #endif
    ga_destroy(ga);
    #ifdef DEBUG
    printf("<========================= DESTUCTOR: PLANTS        ==========================>\n");
    #endif
    for(int i = 0; i < POP; i++)
    {
        plant_destroy(avs[i], 0);
    }

    #ifdef DEBUG_GA
    #ifdef  TOP_NUM
    printf("\n\n<========================= GA: RESULTS             ==========================>\n");
    printf("\nTOP %d (%d EPOCHS): \tNumber of accidents (/%d)\tAction deviation (/%d)\tAverage speed (/2)\n", TOP_NUM, RUN_GA, RUN_NO_ENFORCER, 1);
    printf("_______________________________________________________________________________________________\n");
    for(int i = 0; i < TOP_NUM; i++)
    {
        if(topX[RUN_GA][i][3] > 1)
            printf("%d:\t%.0f\t\t%.2f\t\t\t\t%.2f\t\t\t%.2f\n", i, topX[RUN_GA][i][3], topX[RUN_GA][i][0], topX[RUN_GA][i][1], topX[RUN_GA][i][2]);
        else
            printf("%d:\t%.4f\t\t%.2f\t\t\t\t%.2f\t\t\t%.2f\n", i, topX[RUN_GA][i][3], topX[RUN_GA][i][0], topX[RUN_GA][i][1], topX[RUN_GA][i][2]);
    }
    printf("\nBOT %d (%d EPOCHS): \tNumber of accidents (/%d)\tAction deviation (/%d)\tAverage speed (/2)\n", TOP_NUM, RUN_GA, RUN_NO_ENFORCER, 1);
    printf("_______________________________________________________________________________________________\n");
    for(int i = 0; i < TOP_NUM; i++)
    {
        int pos = POP - TOP_NUM + i;
        if(bottomX[RUN_GA][i][3] > 1)
            printf("%d:\t%.0f\t\t%.2f\t\t\t\t%.2f\t\t\t%.2f\n", pos, bottomX[RUN_GA][i][3], bottomX[RUN_GA][i][0], bottomX[RUN_GA][i][1], bottomX[RUN_GA][i][2]);
        else
            printf("%d:\t%.4f\t\t%.2f\t\t\t\t%.2f\t\t\t%.2f\n", pos, bottomX[RUN_GA][i][3], bottomX[RUN_GA][i][0], bottomX[RUN_GA][i][1], bottomX[RUN_GA][i][2]);
    }
    #endif
    #endif

    #endif

    #ifdef USE_TRAINED_ALL
    }
    #endif // end use all trained for loop

    return 0;
}

void av_run(inputs_av_t* inputs, outputs_av_t* outputs)
{
    
}

void ga_test()
{
    // TESTING GRAY ENCODING
    // float val = -16.60701;
    // printf("Gray encoding: %f (%d)\n", val, fix16_from_float(val));
    // int gray[GRAY_BITS];
    // gray_encode(0, fix16_from_float(val), gray);

    // for(int i = 0; i < GRAY_BITS; i++)
    // {
    //     printf("%d ", gray[i]);
    // }
    // printf("\n");

    // fix16_t decoded = gray_decode(NULL, gray);
    // printf("Decoded: %f (%d)\n", fix16_to_float(decoded), decoded);

    // return 0;
    // END TESTING

    // TESTING SORTING
    GA * ga = (GA *)malloc(sizeof(GA));
    for(int i = 0; i < POP; i++)
    {
        ga->population[i] = (ANN_F *)malloc(sizeof(ANN_F));
    }

    int arr[10][2] = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}};
    int scores[10] = {4, 2, 7, 5, 0, 1, 5, 3, 7, 8};
    int positions[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    ga_init(ga);

    for(int i = 0; i < 10; i++)
    {
        ga->scores[i] = scores[i];
        ga->positions[i] = i;
    }

    printf("Arrays before:\nScores:"); 
    for(int i = 0; i < 10; i++)
        printf(" [%d, %d](%d): %f\t", arr[ga->positions[i]][0], arr[ga->positions[i]][1], ga->positions[i], ga->scores[i]);
    printf("\nPositions:");
    for(int i = 0; i < 10; i++)
        printf(" %d\t", ga->positions[i]);
    printf("\n");

    ga_run_once(ga);

    printf("\nArrays after:\nScores:"); 
    for(int i = 0; i < 10; i++)
        printf(" [%d, %d](%d): %f\t", arr[ga->positions[i]][0], arr[ga->positions[i]][1], ga->positions[i], ga->scores[i]);
    printf("\nPositions:");
    for(int i = 0; i < 10; i++)
        printf(" %d\t", ga->positions[i]);
    printf("\n");

    int scores2[10] = {6, 1, 9, 3, 6, 0, 1, 1, 5, 4};
    for(int i = 0; i < 10; i++)
    {
        ga->scores[i] = scores2[i];
    }

    printf("\nArrays before:\nScores:"); 
    for(int i = 0; i < 10; i++)
        printf(" [%d, %d](%d): %f\t", arr[ga->positions[i]][0], arr[ga->positions[i]][1], ga->positions[i], ga->scores[i]);
    printf("\nPositions:");
    for(int i = 0; i < 10; i++)
        printf(" %d\t", ga->positions[i]);
    printf("\n");

    ga_run_once(ga);

    printf("\nArrays after:\nScores:"); 
    for(int i = 0; i < 10; i++)
        printf(" [%d, %d](%d): %f\t", arr[ga->positions[i]][0], arr[ga->positions[i]][1], ga->positions[i], ga->scores[i]);
    printf("\nPositions:");
    for(int i = 0; i < 10; i++)
        printf(" %d\t", ga->positions[i]);
    printf("\n");

    ga_destroy(ga);
    // END TESTING
}