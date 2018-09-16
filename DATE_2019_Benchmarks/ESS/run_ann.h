#ifndef RANN_H
#define RANN_H

#include <math.h>
#include "ann_weights.h"

#define MAX_KWH_PER_TICK    					280
#define MIN_KWH_PER_TICK                        80
#define MAX_PRICE								260
#define MIN_PRICE                               60
#define MAX_EXPENSE_KWH                         (MAX_KWH_PER_TICK * MAX_PRICE / KWMIN)
#define MAX_BINARY                              25

#define TOD_STEPS                               3

#define BATTERY_STEP                            10
#define BATTERY_CAPACITY_STEPS                  62
#define BATTERY_STATES                          6

#define DEMAND_STEP								4
#define DEMAND_STEPS                            50
#define DEMAND_STATES                           6

#define PRICE_STEP								4
#define PRICE_STEPS                             50
#define PRICE_STATES                            6

static int price_increasing = 0;
static int demand_increasing = 0;

void int_to_binary(int num, int length, int binary[]);

int run_ESS_ANN(ANN_F * Q, float future_price, float previous_demand, float price, float demand, float battery_SoC, int nullQ, int day);
 
#endif


























