#pragma once

#include <stdio.h>
#include "system_config.h"

struct Battery {
    double SoC;
};

void BatteryInit(struct Battery* me, int day, float soc);

//BatteryAttemptInputCoulombs attempts to add "inputColumbs" coulombs to the battery's charge
//it will return the actual number of coulombs it added
double BatteryAttemptInputKWH(struct Battery* me, double inputKWH, int overwrite);

//BatteryAttemptRemoveCoulombs attempts to remove "inputColumbs" coulombs to the battery's charge
//it will return the actual number of coulombs it removed
double BatteryAttemptRemoveKWH(struct Battery* me, double removeKWH, int overwrite);

double BatteryGetKWH(struct Battery* me);
