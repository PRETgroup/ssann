#include "run_ann.h"

int run_ESS_ANN(ANN_F * Q, float future_price, float previous_demand, float price, float demand, float battery_SoC, int nullQ, int day)
{
	ANN_F Q_actual;
	if(nullQ)
	{
		int num_layers = 3;
		int layers[3] = {2 + DEMAND_STATES + PRICE_STATES + BATTERY_STATES, Q_NEURONS2, TOD_STEPS};
		int bias = 1;
		int activation[2] = {2, 3};
		ann_init_fixed_custom(&Q_actual, num_layers, layers, Q_LAYER1, Q_weightsf[day], bias, activation);
	}

	// Compose the ANN inputs
	int i;
	fix16_t inputs[PRICE_STATES + DEMAND_STATES + BATTERY_STATES + 2];

	// put future values in range [0, MAX_VAL]
	if(future_price > MAX_PRICE)
		future_price = MAX_PRICE;
	if(future_price < 0)
		future_price = 0;

	if(future_price > price)
	{
		price_increasing = 1;
	}
	else if(future_price < price)
	{
		price_increasing = 0;
	}
	inputs[0] = fix16_from_int(price_increasing);

	if(demand > previous_demand)
	{
		demand_increasing = 1;
	}
	else if(demand < previous_demand) 
	{
		demand_increasing = 0;
	}
	inputs[1] = fix16_from_int(demand_increasing);

	int demand_state = 0;
	int found = 0;
#pragma loopbound min 0 max DEMAND_STEPS
	for(i = 0; i < DEMAND_STEPS; i++)
	{
		if((i + 1) * DEMAND_STEP + MIN_KWH_PER_TICK > demand)
		{
			demand_state = i;
			found = 1;
			break;
		}
	}
	if(!found)
	{
		demand_state = i;
	}

	found = 0;
	int price_state = 0;
#pragma loopbound min 0 max PRICE_STEPS
	for(i = 0; i < PRICE_STEPS; i++)
	{
		if((i + 1) * PRICE_STEP + MIN_PRICE > price)
		{
			price_state = i;
			found = 1;
			break;
		}
	}
	if(!found)
	{
		price_state = i;
	}

	found = 0;
	int battery_state = 0;
 	//printf("Battery level is: %f\n", battery_level / SECONDS_PER_TICK);
#pragma loopbound min 0 max BATTERY_CAPACITY_STEPS
	for(i = 0; i < BATTERY_CAPACITY_STEPS; i++)
	{
		//printf("Battery step is: %f\n", (float)(i + 1) * BATTERY_STEP);
		if((i + 1) * BATTERY_STEP > battery_SoC)
		{
			//printf("%f > %f\n", (float)(i + 1) * BATTERY_STEP, battery_level);
			battery_state = i;
			found = 1;
			break;
		}
	}
	if(!found)
	{
		battery_state = i;
	}

	int binary_demand[DEMAND_STATES];
	int_to_binary(demand_state, DEMAND_STATES, binary_demand);
#pragma loopbound min 0 max DEMAND_STATES
	for(i = 0; i < DEMAND_STATES; i++)
	{
		inputs[i + 2] = fix16_from_int(binary_demand[i]);
	}

	int binary_price[PRICE_STATES];
	int_to_binary(price_state, PRICE_STATES, binary_price);
#pragma loopbound min 0 max PRICE_STATES
	for(i = 0; i < PRICE_STATES; i++)
	{
		inputs[i + 2 + DEMAND_STATES] = fix16_from_int(binary_price[i]);
	}

	int binary_battery[BATTERY_STATES];
	int_to_binary(battery_state, BATTERY_STATES, binary_battery);
#pragma loopbound min 0 max BATTERY_STATES
	for(i = 0; i < BATTERY_STATES; i++)
	{
		inputs[i + 2 + DEMAND_STATES + PRICE_STATES] = fix16_from_int(binary_battery[i]);
	}

	// Run the ANN

	// printf("Inputs: \n");
	// for(int k = 0; k < PRICE_STATES + DEMAND_STATES + BATTERY_STATES + 2; k++)
	// {
	// 	printf("%.0f; ", fix16_to_float(inputs[k]));
	// }
	// printf("\n");

	fix16_t Q_values[TOD_STEPS];
	if(nullQ)
		ann_run_fixed(inputs, Q_values, &Q_actual);
	else
		ann_run_fixed(inputs, Q_values, Q);

	// Find the TOD value

	// printf("Outputs: \n");
	// for(int k = 0; k < 3; k++)
	// {
	// 	printf("%d; ", Q_values[k]);
	// }
	// printf("\n");

	int q_index = 0;
	fix16_t q_max = 0;

	for(i = 0; i < TOD_STEPS; i++) 
	{
		if(Q_values[i] > q_max)
		{
			q_max = Q_values[i];
			q_index = i;
		}
	}

	// printf("Q index (TOD): %d\n", q_index);

	return q_index;
}

void int_to_binary(int num, int length, int binary[])
{
	int pos = 0;
	// printf("Input is: %d, length is: %d\n", num, length);
	int k; 
#pragma loopbound min 0 max MAX_BINARY
	for(k = 0; k < length; k++) 
	{ 
		binary[k] = 0; 
	} 

	int i;
#pragma loopbound min 0 max MAX_BINARY 
	for(i = 0; i < MAX_BINARY; i++)
	{
		if(num <= 0)
		{
			break;
		}
		binary[length - 1 - pos] = num%2;
		num /= 2;
		pos += 1;
	}
}