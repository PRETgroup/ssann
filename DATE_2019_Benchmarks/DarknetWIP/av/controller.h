#include "ann_fixed.h"

#define MIN_OUTPUT      0.1

#define FRONT_NEURONS1  12
#define BACK_NEURONS1  3

#define ACTIVATION1     0
#define ACTIVATION2     0

#define FIN_EXTERNAL  2
#define FIN_NEURONS1  (FRONT_NEURONS1 + BACK_NEURONS1 + FIN_EXTERNAL)
#define FIN_NEURONS2  10
#define FIN_NEURONS3  3

typedef struct{
    // inputs
    float front_in[FRONT_NEURONS1];  // object x4, direction x4, speed x4
    float back_in[BACK_NEURONS1];  // object x1, direction x1, speed x1

    float fin_in[FIN_EXTERNAL];

    // outputs
    int decision[FIN_NEURONS3];
}control;

// Global variables
control c;
static control * ctrl = &c;
// structs
ANN_F f;
static ANN_F * fin = &f;

// Initialisation
void controller_init(int trained);
void controller_destroy();

// Run functions
void controller_run_tick();
void run_fin();

// Esterel generated functions
int controller();
void controller_I_start();
void controller_O_done();