#include "battery.h"

void BatteryInit(struct Battery* me, int day, float soc) { 
    if(day > 0)
        me->SoC = soc;
    else
        me->SoC = BATTERY_START_KWH;
}

double BatteryAttemptInputKWH(struct Battery* me, double inputKWH, int overwrite) {
	// if(overwrite) printf("++++++ KWH being input into battery with charge %f C: %f\n", me->SoC, inputKWH);
    
    float mod = inputKWH;

    float addKWH = mod / ((float)BATTERY_MAX_KWH * 60.0); 

    // if(overwrite) printf("Attempting to add %f KW to the battery\n", addKWH);

    if(me->SoC + addKWH > BATTERY_MAX_SAFE)
    {
        mod -= (me->SoC + addKWH - BATTERY_MAX_SAFE) * (BATTERY_MAX_KWH * 60.0);
    	if(overwrite) me->SoC = BATTERY_MAX_SAFE;
    }
    else if(overwrite) me->SoC += addKWH;

    // printf("****** Battery charge remaining: %f C\n", me->SoC);
    return mod;
}

double BatteryAttemptRemoveKWH(struct Battery* me, double removeKWH, int overwrite) {
    // if(overwrite) printf("++++++ KWH being removed from  battery with charge %f C: %f\n", me->SoC, removeKWH);
    // if(overwrite) printf("Attempting to remove %f KW from the battery\n", removeKWH / ((float)BATTERY_MAX_KWH * 60.0));

    float SoC_dif = me->SoC - removeKWH / ((float)BATTERY_MAX_KWH * 60.0);
    // if(overwrite) printf("Remaining battery after attempted removal: %f\n", SoC_dif);
    if(SoC_dif < BATTERY_MIN_SAFE) {
        double total = me->SoC - BATTERY_MIN_SAFE;
        if(overwrite) me->SoC = BATTERY_MIN_SAFE;
        return total * (BATTERY_MAX_KWH * 60.0);
    }
    // printf("###### KWH being removed from battery with charge %f C: %f\n", me->SoC, removeKWH);

    if(overwrite) me->SoC -= removeKWH / ((float)BATTERY_MAX_KWH * 60.0);
    // if(overwrite) printf("////// Battery charge remaining: %f C\n", me->SoC);
    return removeKWH;
}

double BatteryGetKWH(struct Battery* me) {
    return me->SoC * 620;
}
