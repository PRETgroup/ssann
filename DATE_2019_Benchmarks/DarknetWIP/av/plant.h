#ifndef PLANT_H
#define PLANT_H

#include "ensemble.h"

typedef struct{
    int objects[CAMERAS];  // nothing, person, car
    int direction[CAMERAS];  // left, front, right, back
    int speed[CAMERAS];  // none, slow, fast
}lidar;

typedef struct{
    // inputs
    // char cameras_in[CAMERAS][255];
    int lidar_in[CAMERAS];

    // structs
    // tmr * cameras[CAMERAS];  // ensemble CNNs
    lidar * l;  // LiDAR

    // outputs
    // int cameras_out[CAMERAS];
    // int cameras_conf[CAMERAS];

    // Environment
    int speed; // none, slow, fast
    int prev_speed;
    int action[3]; // turn left, turn right, accelerate, slow brake, fast brake, nothing (cruise) (0, 1, 2, 3, 4, 5)
    int new_action[3]; // turn left, turn right, accelerate, slow brake, fast brake, nothing (cruise) (0, 1, 2, 3, 4, 5)
}plant;

// initialisation
void plant_init(plant * p, int reset, int cnns, int trained);
void plant_destroy(plant * p, int destroy_cnns);

// run functions
void plant_get_inputs(plant * p);
void plant_run_inputs(plant * p, int run_cnns);
void plant_get_inputs_no_cnns(plant * p);
void plant_update_speed(plant * p);
int plant_run_outputs(plant * p);

// helper functions
void plant_print(plant * p);
void make_nothing(lidar * l, int i);

#endif