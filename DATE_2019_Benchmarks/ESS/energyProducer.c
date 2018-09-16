#include "energyProducer.h"
#include <math.h>
#include <stdio.h>

void EnergyProducerInit(struct EnergyProducer* me, int day, int pinc) {
    me->currentTick = 0;
    me->totalProducedKWH = 0;
    me->day = day % 7;
    me->futureKWHPrice = price[0][me->day];

    if(day > 0)
    {
        if(pinc == 1)
        {
            me->currentKWHPrice = me->futureKWHPrice - 1;
            me->previousKWHPrice = 0;
        }
        else
        {
            me->currentKWHPrice = me->futureKWHPrice + 1;
            me->previousKWHPrice = 1000;
        }
    }
}

void EnergyProducerUpdatePrice(struct EnergyProducer* me, int deltaTick) {
    me->currentTick += deltaTick;

    me->previousKWHPrice = me->currentKWHPrice;
    me->currentKWHPrice = me->futureKWHPrice; 
    // me->futureKWHPrice = 30 + 15*sin((double)(me->currentTick + 1) * (2*PI) / TICKS_PER_PRICE_PERIOD);
    me->futureKWHPrice = price[me->currentTick][me->day];
    //printf("new price at tick %d is %.2f\r\n", me->currentTick, me->currentKWHPrice);
}

void EnergyProducerProduceKWH(struct EnergyProducer* me, double numKWH) {
    me->totalProducedKWH += numKWH;
}

double EnergyProducerGetCurrentMWHPrice(struct EnergyProducer* me) {
    return me->currentKWHPrice;
}

double EnergyProducerGetCurrentKWMINPrice(struct EnergyProducer* me) {
    return me->currentKWHPrice / KWMIN;
}
