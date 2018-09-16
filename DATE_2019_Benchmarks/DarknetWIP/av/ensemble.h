#include "tmr.h"

#define CAMERAS 5

typedef struct{
    char in[CAMERAS][255];
    // outputs
    int out[CAMERAS];
    int conf[CAMERAS];

    tmr * cnns[CAMERAS];
}ensembles;

ensembles * cameras;

void ensemble_init(int reset, int cnns, int trained);

void run_cnn(int ensemble, int net);

void run_ensemble(int ensemble);

void ensemble_destroy(int destroy_cnns);

// Generated esterel files
int ensemble();
void ensemble_I_start();