#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdio.h>
#include <stdlib.h> 
#include "system_config.h"
#include "battery.h"
#include "energyConsumer.h"
#include "energyProducer.h"
#include "run_ann.h"

typedef struct {
    struct Battery battery;         //the battery in the system
    struct EnergyConsumer consumer; //the electric demand (i.e. the car drawing current)
    struct EnergyProducer producer; //the power plant
    int TOD;
    int day;

    int currentTick;

    int priceIncreasing;
    int demandIncreasing;
 
    double currentExpense;
}EVSystem;

void UpdateChildren(EVSystem* sys);

void EVSystemInit(EVSystem* me, int day, float soc, int pinc, int dinc);
float EVSystemRunTick(EVSystem* me, fix16_t Q_values[]);

float getKWHValue(EVSystem * sys, float demandKWHThisTick, int overwrite);

// helper functions
void getQInputs(EVSystem *sys, float future_price, float previous_demand, float price, float demand, float battery_level, fix16_t inputs[]);

// training only functions
void init_train(ANN_F * Q, ANN_F * price_ann, ANN_F * demand_ann, int W1, int W2, int W3,
                fix16_t q_weights[][W1], fix16_t p_weights[][W2], fix16_t d_weights[][W3]);
void getQValue(EVSystem * sys, ANN_F * Q, float future_price, float previous_demand, float price, float demand, float battery_level, fix16_t q_values[]);
void EVSystemTrainTick(EVSystem* sys, ANN_F *Q, ANN_F * price_ann, ANN_F * demand_ann, int tick, int epoch, int max_weights, fix16_t delta_accumulate[][max_weights]);
float predictPrice(EVSystem * sys, ANN_F * price_ann, int mod);
float predictDemand(EVSystem *sys, ANN_F * demand_ann, int mod);
float rewardFunction(double expenseThisTick); 
fix16_t targetFunction(double reward, fix16_t future_value);
void ann_train_q(ANN_F *ann, fix16_t q_actual[], fix16_t q_target[], int max_weights, fix16_t delta_accumulate[][max_weights], int tick);
void ann_train_demand(ANN_F *ann, EVSystem *sys, int num_prev, int debug);
void ann_train_price(ANN_F *ann, EVSystem *sys, int num_prev, int debug);


#endif