#include "energyConsumer.h"
#include <math.h>
#include <stdio.h>

void EnergyConsumerInit(struct EnergyConsumer* me, int day, int dinc) {
    me->day = day % 7;
    me->futureDemandKWH = demand[0][me->day];

    if(day > 0)
    {
        if(dinc == 1)
        {
            me->currentDemandKWH = me->futureDemandKWH - 1;
            me->previousDemandKWH = 0;
        }
        else
        {
            me->currentDemandKWH = me->futureDemandKWH + 1;
            me->previousDemandKWH = 1000;
        }
    }

    me->totalConsumedKWH = 0;
    me->currentTick = 0;
}

void EnergyConsumerUpdateDemand(struct EnergyConsumer* me, int deltaTick) {
    //me->currentDemandKWH = 100 - me->currentDemandKWH;
    me->currentTick += deltaTick;
    me->previousDemandKWH = me->currentDemandKWH;
    me->currentDemandKWH = me->futureDemandKWH;
    me->futureDemandKWH = demand[me->currentTick][me->day];
}

void EnergyConsumerConsumeKWH(struct EnergyConsumer* me, double KWH) {
    me->totalConsumedKWH += KWH;
    // printf("Consumed %f KWH on tick %d\nTotal consumption: %f KWH\n", KWH, me->currentTick, me->totalConsumedKWH);
}

double EnergyConsumerGetDemandKWH(struct EnergyConsumer* me) {
    return me->currentDemandKWH;
}

double EnergyConsumerGetTotalKWHConsumption(struct EnergyConsumer* me) {
    return me->totalConsumedKWH;
}
