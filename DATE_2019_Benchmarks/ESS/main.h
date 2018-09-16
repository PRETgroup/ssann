#ifndef MAIN_H
#define MAIN_H    

#include "ess.h"
#include "F_ESS.h"
#include "F_soc.h"
#include "F_overcurrent.h"

void ESS_run(inputs_ESS_t* inputs, outputs_ESS_t* outputs);
void soc_run(inputs_soc_t* inputs, outputs_soc_t* outputs);
void overcurrent_run(inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs);

#endif