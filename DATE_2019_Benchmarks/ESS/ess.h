#pragma once
 
#include "system.h"

EVSystem * init(int day, float soc, int pinc, int dinc);

float getFuturePrice();
float getPreviousDemand();

void runQLayer1(float future_price, float future_demand, float price, float demand, float SoC);
void runQLayer2();

float getBatteryWatts(float demandKWHThisTick);
float systemUpdate();

// Esterel output functions
void ess_O_watts(float);
int ess_reset();




















