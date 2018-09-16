#include "ess.h"
// #include "ann_weights.h"  

// demand predictor ANN
ANN_F new_demand_ann;
ANN_F *demand_ann = &new_demand_ann;

//price predictor ANN
ANN_F new_price_ann;
ANN_F *price_ann = &new_price_ann;

//Q-value ANN
ANN_F new_ann;
ANN_F *Q = &new_ann;

// EV system
EVSystem new_sys;
EVSystem *sys = &new_sys;

// system variables
fix16_t q_values[Q_NEURONS3];

// Hidden layer arrays
fix16_t hiddenQ[Q_NEURONS2];

EVSystem * init(int day, float soc, int pinc, int dinc)
{
	int num_weights;
	int num_layers = 3;
	int layers[3] = {Q_NEURONS1, Q_NEURONS2, Q_NEURONS3};
	int bias = 1;
	int activation[2] = {2, 3};
	ann_init_fixed_custom(Q, num_layers, layers, Q_LAYER1, Q_weightsf[day], bias, activation);

	EVSystemInit(sys, day, soc, pinc, dinc);
	return sys;
}

float getFuturePrice()
{
	return sys->producer.futureKWHPrice;
}
float getPreviousDemand()
{
	return sys->consumer.previousDemandKWH;
}

// run the Q ANN with the given inputs
void runQLayer1(float future_price, float future_demand, float price, float demand, float SoC)
{
	// printf("Sys tick1: %d\n", sys->currentTick);
	// printf("Inputs -> FP: %f, FD: %f, P: %f, D: %f, B: %f\n", 
	// future_price, future_demand, price, demand, SoC);

	fix16_t inputs[Q_NEURONS1];
	getQInputs(sys, future_price, future_demand, price, demand, SoC, inputs);

	int i, j, k;
	fix16_t weighted_sum;
	RUN_ANN_LAYER_FIXED(inputs, hiddenQ, Q, 1, i, j, k, weighted_sum, Q_NEURONS1, Q_NEURONS2)
}

void runQLayer2()
{
	// printf("Sys tick2: %d\n", sys->currentTick);
	int i, j, k;
	fix16_t weighted_sum;
	RUN_ANN_LAYER_FIXED(hiddenQ, q_values, Q, 2, i, j, k, weighted_sum, Q_NEURONS2, Q_NEURONS3)

	// printf("Outputs: [%d, %d, %d]\n", q_values[0], q_values[1], q_values[2]);
}

float getBatteryWatts(float demandKWHThisTick)
{
	int TOD = 0;
	fix16_t q_max = 0;

	for(int i = 0; i < TOD_STEPS; i++) 
	{
		if(q_values[i] > q_max)
		{
			q_max = q_values[i];
			TOD = i;
		}
	}

	if(TOD == 0)  // off peak charging
	{
		return BATTERY_MAX_CHARGE_KWH;
	}
	else if(TOD == 1)  // off peak
	{ 
		if(demandKWHThisTick > PEAK_DISCHARGE_KWH)
		{
			return -(demandKWHThisTick - PEAK_DISCHARGE_KWH);
		}
		else
		{
			return 0;
		}
	}
	else  // on peak
	{
		return -BATTERY_MAX_DISCHARGE_KWH;
	}
}

// updates the system and returns the watts produced by the battery in the previous tick
float systemUpdate()
{
	// Update the system with this tick's values
	return EVSystemRunTick(sys, q_values); // update system
}
