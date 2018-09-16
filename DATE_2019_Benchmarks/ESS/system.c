#include "system.h"

// #define PREDICTING
// #define DEBUG
// #define DEBUGRUN
// #define DEBUGBATTERY 
// #define DEBUGINIT

void EVSystemInit(EVSystem* sys, int day, float soc, int pinc, int dinc) {
    BatteryInit(&sys->battery, day, soc);
    EnergyConsumerInit(&sys->consumer, day, dinc);
    EnergyProducerInit(&sys->producer, day, pinc);
    sys->currentExpense = 0;
    sys->currentTick = 1;

	if(day > 0)
	{
    	sys->priceIncreasing = pinc;
		sys->demandIncreasing = dinc;
	}
	else
	{
		sys->priceIncreasing = 0;
		sys->demandIncreasing = 0;
	}
    	
	sys->TOD = 0;
	sys->day = day % 7;
}

void UpdateChildren(EVSystem* sys) {
	int prod = 0;
	int cons = 0;
	sys->currentTick++;

	if(sys->currentTick % TICKS_PER_PRICE_PERIOD == 0)
		prod = 1;
	if(sys->currentTick % TICKS_PER_DEMAND_PERIOD == 0)
		cons = 1;

    EnergyConsumerUpdateDemand(&sys->consumer, cons);
    EnergyProducerUpdatePrice(&sys->producer, prod);
}

float getKWHValue(EVSystem * sys, float demandKWHThisTick, int overwrite)
{
	float outKWH = demandKWHThisTick;
	// printf("In: %f\t", outKWH);
	if(sys->TOD == 0)  // off peak charging
	{
		float input = BatteryAttemptInputKWH(&sys->battery, BATTERY_MAX_CHARGE_KWH, overwrite);

		#ifdef DEBUGBATTERY
		printf("->> Battery is charging (off peak) %f KW. SoC: %f\n", input, sys->battery.SoC);
		#endif

		outKWH += input;
	}
	else if(sys->TOD == 1)  // off peak
	{
		if(outKWH > PEAK_DISCHARGE_KWH)
		{
			float output = BatteryAttemptRemoveKWH(&sys->battery, outKWH - PEAK_DISCHARGE_KWH, overwrite);

			#ifdef DEBUGBATTERY
			printf("->> Battery is discharging (off peak) %f KW. SoC: %f\n", output, sys->battery.SoC);
			#endif

			outKWH -= output;
		}
		else
		{
			#ifdef DEBUGBATTERY
			printf("->> Battery is unchanged (off peak). SoC: %f\n", sys->battery.SoC);
			#endif
		}
	}
	else  // on peak
	{
		float output = BatteryAttemptRemoveKWH(&sys->battery, BATTERY_MAX_DISCHARGE_KWH > outKWH ? outKWH : BATTERY_MAX_DISCHARGE_KWH, overwrite);

		#ifdef DEBUGBATTERY
		printf("->> Battery is discharging (on peak) %f KW. SoC: %f\n", output, sys->battery.SoC);
		#endif

		outKWH -= output;
	}

	// printf("Out: %f\n", outKWH);

	return outKWH;
}

float EVSystemRunTick(EVSystem* sys, fix16_t Q_values[]) {
	// find action with biggest reward and return Q value
	int i = 0;
	//printf("Finding correct action to take in tick: %d\n", tick);

	if(Q_values != NULL)
	{
		int q_index = 0;
		fix16_t q_max = 0;

	#pragma loopbound min 0 max TOD_STEPS
		for(i = 0; i < TOD_STEPS; i++) 
		{
			if(Q_values[i] > q_max)
			{
				q_max = Q_values[i];
				q_index = i;
			}
		}

		sys->TOD = q_index;
	}

	#ifdef DEBUGRUN
	printf("TICK: %d; Time of Day is: %d\n", sys->currentTick, sys->TOD);
	#endif

	double demandKWHThisTick = EnergyConsumerGetDemandKWH(&sys->consumer);
	EnergyConsumerConsumeKWH(&sys->consumer, demandKWHThisTick);

	#ifdef DEBUGRUN
	printf("KWH being demanded by consumers: %f\n", demandKWHThisTick);
	#endif

	double expenseThisTick = getKWHValue(sys, demandKWHThisTick, 0) * EnergyProducerGetCurrentKWMINPrice(&sys->producer);
	// printf("Expense is: %f * %f = %f\n", getKWHValue(sys, demandKWHThisTick, 0), EnergyProducerGetCurrentKWMINPrice(&sys->producer), expenseThisTick);

	#ifdef DEBUGRUN
	printf("Net KW being paid for: %f\n", getKWHValue(sys, demandKWHThisTick, 0));
	printf("Expense this tick is: %f * %f = %f\n", getKWHValue(sys, demandKWHThisTick, 0), EnergyProducerGetCurrentKWMINPrice(&sys->producer), expenseThisTick);
	#endif
 
	//printf("====== KWH being produced by supplier: %f\n", demandKWHThisTick);
	EnergyProducerProduceKWH(&sys->producer, getKWHValue(sys, demandKWHThisTick, 1));

	// update current expense
	sys->currentExpense += expenseThisTick;
	// printf("Expense this tick (%d) is: $ %f\tTotal expenses: $ %f\n", sys->currentTick, expenseThisTick, sys->currentExpense);

	UpdateChildren(sys);
}
 
// this is the Q(s, a) ANN function for this system
void getQInputs(EVSystem *sys, float future_price, float previous_demand, float price, float demand, float battery_SoC, fix16_t inputs[]) {
    // Compose the ANN inputs
	int i;

	// put future values in range [0, MAX_VAL]
	if(future_price > MAX_PRICE)
		future_price = MAX_PRICE;
	if(future_price < 0)
		future_price = 0;

	if(future_price > price)
	{
		sys->priceIncreasing = 1;
	}
	else if(future_price < price)
	{
		sys->priceIncreasing = 0;
	}
	inputs[0] = fix16_from_int(sys->priceIncreasing);

	if(demand > previous_demand)
	{
		sys->demandIncreasing = 1;
	}
	else if(demand < previous_demand) 
	{
		sys->demandIncreasing = 0;
	}
	inputs[1] = fix16_from_int(sys->demandIncreasing);

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
}

// ======================= CODE FOR TRAINING ===================================

void init_train(ANN_F * Q, ANN_F * price_ann, ANN_F * demand_ann, int W1, int W2, int W3,
                fix16_t q_weights[][W1], fix16_t p_weights[][W2], fix16_t d_weights[][W3])
{
	// printf("<===========  Initialisation  ============>\n");
	int num_layers = 3;
	int layers[3] = {2 + DEMAND_STATES + PRICE_STATES + BATTERY_STATES, Q_NEURONS2, TOD_STEPS};
	int bias = 1;
	int activation[2] = {2, 3};

	if(W1 != 0)
	{
		#ifdef DEBUGINIT
		printf("==> Q ANN initialised with predefined weights\n");
		#endif

		ann_init_fixed_custom(Q, num_layers, layers, Q_LAYER1, q_weights, bias, activation);
	}
	else
	{
		#ifdef DEBUGINIT
		printf("==> Q ANN initialised with random weights\n");
		#endif
		
    	ann_init_fixed(Q, num_layers, layers, bias, activation);
	}

	int i, j, num_weights;
	int train_num_layers1 = 3;
	int train_layers1[3] = {TICK_STATES, TICK_STATES, 1};
	int train_bias1 = 1;
	int train_activation1[2] = {1, 1};

	if(W2 != 0)
	{
		#ifdef DEBUGINIT
		printf("==> Price ANN initialised with predefined weights\n");
		#endif
		
		ann_init_fixed_custom(price_ann, train_num_layers1, train_layers1, PRICE_LAYER1, p_weights, train_bias1, train_activation1);
	}
	else
	{
		#ifdef DEBUGINIT
		printf("==> Price ANN initialised with random weights\n");
		#endif
		
		ann_init_fixed(price_ann, train_num_layers1, train_layers1, train_bias1, train_activation1);
	}

	int train_num_layers2 = 3;
	int train_layers2[3] = {TICK_STATES, (TICK_STATES + 1) / 2, 1};
	int train_bias2 = 1;
	int train_activation2[2] = {1, 1};

	if(W3 != 0)
	{
		#ifdef DEBUGINIT
		printf("==> Demand ANN initialised with predefined weights\n");
		#endif
		
		ann_init_fixed_custom(demand_ann, train_num_layers2, train_layers2, DEMAND_LAYER1, d_weights, train_bias2, train_activation2);
	}
	else
	{
		#ifdef DEBUGINIT
		printf("==> Demand ANN initialised with random weights\n");
		#endif
		
		ann_init_fixed(demand_ann, train_num_layers2, train_layers2, train_bias2, train_activation2);
	}

	#ifdef DEBUG
	printf("<===========  End Initialisation  ============>\n");
	#endif
}

// retrieves the Q-values for the complete inputs of the Q ANN 
void getQValue(EVSystem * sys, ANN_F * Q, float future_price, float previous_demand, float price, float demand, float battery_level, fix16_t q_values[])
{
	int i, j, k;

	fix16_t weighted_sum;
	fix16_t inputs[Q_NEURONS1];
	fix16_t hiddenQ[Q_NEURONS2];

	#ifdef DEBUGRUN
	printf("\nInputs to ANN: %f, %f, %f, %f, %f\n", future_price, previous_demand, price, demand, battery_level);
	#endif

	getQInputs(sys, future_price, previous_demand, price, demand, battery_level, inputs);

	// printf("Inputs to Q ANN: ");
	// for(int i = 0; i < Q_NEURONS1; i++)
	// 	printf("%d ", fix16_to_int(inputs[i]));
	// printf("\n");

	// RUN_ANN_LAYER_FIXED(inputs, hiddenQ, Q, 1, i, j, k, weighted_sum, Q_NEURONS1, Q_NEURONS2)
	// RUN_ANN_LAYER_FIXED(hiddenQ, q_values, Q, 2, i, j, k, weighted_sum, Q_NEURONS2, Q_NEURONS3)

	// ann_print_fixed(Q, inputs, 0);

	ann_run_fixed(inputs, q_values, Q);

	// printf("Outputs: %f, %f, %f\n", fix16_to_float(q_values[0]), fix16_to_float(q_values[1]), fix16_to_float(q_values[2]));
}

// ESS training tick 
void EVSystemTrainTick(EVSystem* sys, ANN_F *Q, ANN_F * price_ann, ANN_F * demand_ann, int tick, int epoch, int max_weights, fix16_t delta_accumulate[][max_weights])
{
	float price = EnergyProducerGetCurrentMWHPrice(&sys->producer);  
	float demand = sys->consumer.currentDemandKWH;  
	float battery_level = sys->battery.SoC * BATTERY_MAX_KWH;  // battery level in KWH

	#ifdef DEBUG
	printf("\nTICK: %d, Price: $ %f; Demand: %f KWH; Battery level: %f KWH\n", tick, price, demand, battery_level);
	#endif

	fix16_t Q_values[TOD_STEPS];

	// find action with biggest reward and return Q value
	int i = 0;

	#ifdef DEBUG
	printf("Finding correct action to take in tick: %d\n", tick);
	#endif

	int index = 0;
	float max = 0;
	int exploit = 1;

	// exploit or explore?
	double explore = (double)rand() / (double)RAND_MAX;
	float explore_factor = ((float)epoch/(EXPLORE_INCREASE - EXPLORE_EPOCH_MOD) > EXPLORE_MAX) ? EXPLORE_MAX : (float)epoch/(EXPLORE_INCREASE - EXPLORE_EPOCH_MOD);
	// printf("Explore factor: %f\n", explore_factor);
	if(explore > EXPLORE_START + explore_factor)
	{
		exploit = 0;
		index = rand()%(TOD_STEPS);
	}

	// predict future price and demand values
	float future_price = 0; //predictPrice(sys, price_ann, 0);
	float previous_demand = 0; //predictDemand(sys, price_ann, 0);

	#ifndef PREDICTING
	future_price = sys->producer.futureKWHPrice;
	previous_demand = sys->consumer.previousDemandKWH;
	#endif

	#ifdef DEBUG
	printf("--> Exploring/Exploiting: %s\n", exploit == 1 ? "Exploiting" : "Exploring");
	#endif

	// exploit or explore?
	if(!exploit)  // explore at the random index
	{
		getQValue(sys, Q, future_price, previous_demand, price, demand, battery_level, Q_values);  // get Q Value for this action

		sys->TOD = index;

		#ifdef DEBUG
		printf("Time of Day is: %d\n", sys->TOD);
		#endif

		double demandKWHThisTick = EnergyConsumerGetDemandKWH(&sys->consumer);

		#ifdef DEBUG
		printf("KWH being demanded by consumers: %f\n", demandKWHThisTick);
		#endif

		double expenseThisTick = getKWHValue(sys, demandKWHThisTick, 0) * EnergyProducerGetCurrentKWMINPrice(&sys->producer);

		#ifdef DEBUG
		printf("Net KW being paid for: %f\n", getKWHValue(sys, demandKWHThisTick, 0));
		printf("Expense this tick is: %f * %f = %f\n", getKWHValue(sys, demandKWHThisTick, 0), EnergyProducerGetCurrentKWMINPrice(&sys->producer), expenseThisTick);
		#endif
		
		float reward = rewardFunction(expenseThisTick);

		max = reward;
	}
	else if(exploit)  // exploit
	{
		getQValue(sys, Q, future_price, previous_demand, price, demand, battery_level, Q_values);  // get Q Value for this action

		for(i = 0; i < TOD_STEPS; i++)
		{
			sys->TOD = i;
			#ifdef DEBUG
			printf("Time of Day is: %d\n", sys->TOD);
			#endif

			double demandKWHThisTick = EnergyConsumerGetDemandKWH(&sys->consumer);

			#ifdef DEBUG
			printf("KWH being demanded by consumers: %f\n", demandKWHThisTick);
			#endif

			double expenseThisTick = getKWHValue(sys, demandKWHThisTick, 0) * EnergyProducerGetCurrentKWMINPrice(&sys->producer);

			#ifdef DEBUG
			printf("Net KW being paid for: %f\n", getKWHValue(sys, demandKWHThisTick, 0));
			printf("Expense this tick is: %f * %f = %f\n", getKWHValue(sys, demandKWHThisTick, 0), EnergyProducerGetCurrentKWMINPrice(&sys->producer), expenseThisTick);
			#endif
			
			float reward = rewardFunction(expenseThisTick);

			// find action with highest reward
			if(reward > max)
			{
				max = reward;
				index = i;
			}
			else if(reward == max)
			{
				double randomize = 0.75;
				double replace = (double)rand() / (double)RAND_MAX;
				if(replace > randomize)  // chance to ignore new highest reward
				{
					max = reward;
					index = i;
				}
			}
		}
	}

	#ifdef DEBUG
	printf("--> Updating system with current Q-value\n");
	#endif

	sys->TOD = index;
	// printf("TOD: %d\n", sys->TOD);

	#ifdef DEBUG
	printf("Time of Day is: %d\n", sys->TOD);
	#endif

	double demandKWHThisTick = EnergyConsumerGetDemandKWH(&sys->consumer);
	EnergyConsumerConsumeKWH(&sys->consumer, demandKWHThisTick);

	#ifdef DEBUG
	printf("KWH being demanded by consumers: %f\n", demandKWHThisTick);
	#endif

	double expenseThisTick = getKWHValue(sys, demandKWHThisTick, 0) * EnergyProducerGetCurrentKWMINPrice(&sys->producer);

	#ifdef DEBUG
	printf("Net KW being paid for: %f\n", getKWHValue(sys, demandKWHThisTick, 0));
	printf("Expense this tick is: %f * %f = %f\n", getKWHValue(sys, demandKWHThisTick, 0), EnergyProducerGetCurrentKWMINPrice(&sys->producer), expenseThisTick);
	#endif

	EnergyProducerProduceKWH(&sys->producer, getKWHValue(sys, demandKWHThisTick, 1));

	// update current expense
	sys->currentExpense += expenseThisTick;
	// printf("Expense this tick (%d) is: $ %f\tTotal expenses: $ %f\n", sys->currentTick, expenseThisTick, sys->currentExpense);
	// ========= END UPDATE ============

	// reward for this action -> rewarded for lower expenses
	float reward = max;  // best reward already found

	// predict future costs for target Q calculation (already done at beginning of training)
	float future_battery_level = sys->battery.SoC * BATTERY_MAX_KWH;  // use updated battery value to run next Q
	// predict future price and demand for 2 ticks ahead
	float future_price2 = 0; //predictPrice(sys, price_ann, 1);
	float previous_demand2 = 0; //predictDemand(sys, price_ann, 1);

	UpdateChildren(sys);

	#ifndef PREDICTING
	future_price2 = sys->producer.futureKWHPrice;
	previous_demand2 = sys->consumer.previousDemandKWH;
	#endif

	// printf("Future demand: %f\n", future_demand);
	// printf("Future price: %f\n", future_price);
	// printf("Future battery level: %f\n", future_battery_level);

	//FIND ACTION WITH BIGGEST Q VALUE
	// future Q value for future inputs
	fix16_t next_max_q = 0;
	int index_q = 0;
	fix16_t future_Q_values[TOD_STEPS];
	getQValue(sys, Q, future_price2, previous_demand2, future_price, previous_demand, future_battery_level, future_Q_values);  // get Q Value for this action
	for(i = 0; i < TOD_STEPS; i++)
	{
		if(future_Q_values[i] > next_max_q)
		{
			next_max_q = future_Q_values[i];
			index_q = i;
		}
	}

	fix16_t targetQ = targetFunction(reward, next_max_q);
	for(i = 0; i < TOD_STEPS; i++) // back-propagate 0 error except on the chosen action
	{
		future_Q_values[i] = Q_values[i];
	}

	future_Q_values[index] = targetQ;

	// printf("Original Q value: %f\n", fix16_to_float(Q_values[index]));
	// printf("Future Q value: %f\n", fix16_to_float(future_Q_values[index]));

	// printf("Reward for this action is: %f\n", reward);
	// printf("Error to be backpropagated is: %f - %f = %f\n", fix16_to_float(targetQ), fix16_to_float(q_value), fix16_to_float(fix16_sub(targetQ, q_value)));

	ann_train_q(Q, Q_values, future_Q_values, max_weights, delta_accumulate, tick);  // back-propagate the error

	// float inputs[9] = {0};
	// ann_print(Q, inputs, 1);

	//printf("Finished backpropagating error\n\n");
}

// returns the reward function for the given system expenses
float rewardFunction(double expenseThisTick)
{
	float reward = (1 + REWARD_FACTOR) * (float)(1 - expenseThisTick/(float)MAX_EXPENSE_KWH) - REWARD_FACTOR;
	// printf("Max expense: %f\n", MAX_EXPENSE_KWH);

	#ifdef DEBUG
	printf("Rewards for expense of %f is: %f\n", expenseThisTick, reward); 
	#endif

	return reward;
}

// returns the target Q value
fix16_t targetFunction(double reward, fix16_t future_value)
{
	fix16_t target = fix16_add(fix16_from_float(reward), fix16_mul(fix16_from_float(GAMMA), future_value));
	//printf("===========================> Target value is: %f + %f * %f = %f\n", reward, GAMMA, future_value, target);
	return target;
}

// backpropagation process for the Q ANN
void ann_train_q(ANN_F *ann, fix16_t q_actual[], fix16_t q_target[], int max_weights, fix16_t delta_accumulate[][max_weights], int tick)
{
	int i, j, k;
	////printf("Starting Q-ANN training\n");
	int num_weights = ann->num_layers - 1;
	// ann->max_weights = 0;
	// for(i = 0; i < num_weights; i++)
	// {
	// 	if(ann->layers[i] * ann->layers[i + 1] + ann->layers[i + 1] > ann->max_weights)
	// 		ann->max_weights = ann->layers[i] * ann->layers[i + 1] + ann->layers[i + 1];
	// }

	// printf("Max weights: %d\n", ann->max_weights);
	// fix16_t delta_accumulate[num_weights][ann->max_weights]; // same size as number of layers of weights
	// for(j = 0; j < num_weights; j++)
	// 	for(k = 0; k < ann->max_weights; k++)
	// 		delta_accumulate[j][k] = 0;

	//printf("Starting Q-ANN weight delta calculations\n");

	// get weight corrections
	ann_get_deltas_fixed(ann, q_actual, q_target, ann->max_weights, delta_accumulate, fix16_from_float(LEARNING_RATE_Q), !EPOCH_TRAINING);

	// for(j = 0; j < num_weights; j++)
	// 	for(k = 0; k < ann->max_weights; k++)
	// 		printf("Delta accumulate after [%d][%d]: %f\n", j, k, fix16_to_float(delta_accumulate[j][k]));

	//printf("ANN max weights: %d\n", ann->max_weights);
	//printf("Starting Q-ANN weight corrections\n");

	if(tick == NUM_TRAIN_TICKS_MAIN - 1 || !EPOCH_TRAINING) // only update weights at the end of the epoch or if we are not using epoch training
	{
		// printf("Updating weights\n");
	// update weights (fixed point arithmetic)
		for(i = 0; i < num_weights; i++) // run through each layer
		{
			for(j = 0; j < ann->layers[i] * ann->layers[i + 1]; j++) // run through each delta weight sum
			{
				fix16_t delta_weight = fix16_div(delta_accumulate[i][j], fix16_from_int(NUM_TRAIN_TICKS_MAIN));  // average delta accumulate
				ann->weights[i][j] = fix16_add(ann->weights[i][j], delta_weight); // add to the corresponding weight
				ann->delta_weights[i][j] = delta_weight;
			}

			// add to bias neuron if necessary
			if(ann->bias)
			{
				for(j = 0; j < ann->layers[i + 1]; j++) // run through each bias weight at end of the weights
				{
					// update bias' in the positions of the final weights
					fix16_t delta_weight = fix16_div(delta_accumulate[i][ann->layers[i] * ann->layers[i + 1] + j], fix16_from_int(NUM_TRAIN_TICKS_MAIN));
					ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j] = fix16_add(ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j], delta_weight);
					ann->delta_weights[i][ann->layers[i] * ann->layers[i + 1] + j] = delta_weight;
				}
			}
		}
	}
}

// predict future price (training only)
float predictPrice(EVSystem * sys, ANN_F * price_ann, int mod)
{
	#ifdef PREDICTING

	int i, j, k;
	fix16_t weighted_sum;

	fix16_t hiddenPrice[PRICE_NEURONS2];
	fix16_t future_price[1];
	int tick_binary[TICK_STATES];
	fix16_t tick_float[TICK_STATES];

	int tick = sys->currentTick + mod;

	if(tick < 200)
		int_to_binary(tick + 1 + 600, TICK_STATES, tick_binary);
	else if(tick < 600)
		int_to_binary(tick + 1 + 400, TICK_STATES, tick_binary);
	else
		int_to_binary(tick + 1, TICK_STATES, tick_binary);
#pragma loopbound min 0 max TICK_STATES
	for(i = 0; i < TICK_STATES; i++)
	{
		tick_float[i] = fix16_from_int(tick_binary[i] == 1 ? 1 : -1);
	}

	RUN_ANN_LAYER_FIXED(tick_float, hiddenPrice, price_ann, 1, i, j, k, weighted_sum, PRICE_NEURONS1, PRICE_NEURONS2)
	RUN_ANN_LAYER_FIXED(hiddenPrice, future_price, price_ann, 2, i, j, k, weighted_sum, PRICE_NEURONS2, PRICE_NEURONS3)

	future_price[0] = fix16_mul(fix16_div(fix16_add(future_price[0], 65536), fix16_from_int(2)), fix16_from_int(MAX_PRICE));

	return fix16_from_float(future_price[0]);

	#else

	return 0;

	#endif
}

// predict future demand (training only)
float predictDemand(EVSystem *sys, ANN_F * demand_ann, int mod)
{
	#ifdef PREDICTING

	int i, j, k;
	fix16_t weighted_sum;

	fix16_t hiddenDemand[DEMAND_NEURONS2];
	fix16_t future_demand[1];
	int tick_binary[TICK_STATES];
	fix16_t tick_float[TICK_STATES];

	int tick = sys->currentTick + mod;

	if(tick < 200)
		int_to_binary(tick + 1 + 600, TICK_STATES, tick_binary);
	else if(tick < 600)
		int_to_binary(tick + 1 + 400, TICK_STATES, tick_binary);
	else
		int_to_binary(tick + 1, TICK_STATES, tick_binary);
#pragma loopbound min 0 max TICK_STATES
	for(i = 0; i < TICK_STATES; i++)
	{
		tick_float[i] = fix16_from_int(tick_binary[i] == 1 ? 1 : -1);
	}

	RUN_ANN_LAYER_FIXED(tick_float, hiddenDemand, demand_ann, 1, i, j, k, weighted_sum, DEMAND_NEURONS1, DEMAND_NEURONS2)	
	RUN_ANN_LAYER_FIXED(hiddenDemand, future_demand, demand_ann, 2, i, j, k, weighted_sum, DEMAND_NEURONS2, DEMAND_NEURONS3)

	future_demand[0] = fix16_mul(fix16_div(fix16_add(future_demand[0], 65536), fix16_from_int(2)), fix16_from_int(MAX_KWH_PER_TICK));

	return fix16_from_float(future_demand[0]);

	#else

	return 0;

	#endif
}

// // trains the demand predictor
// void ann_train_demand(ANN_F *ann, EVSystem *sys, int num_prev, int debug)
// {
// 	FILE *fp;
// 	fp = fopen("demand_data.txt", "w");

// 	fprintf(fp, "%d\n", NUM_TRAIN_TICKS);

// 	int i, j;
// 	for(i = 0; i < NUM_TRAIN_TICKS; i++)
// 	{
// 		int tick_binary[TICK_STATES];
// 		int_to_binary(i + 1, TICK_STATES, tick_binary);
// 		for(j = 0; j < TICK_STATES; j++)
// 			fprintf(fp, "%f ", tick_binary[j] == 1 ? 1.0 : -1.0);
// 		fprintf(fp, "%f\n", (sys->consumer.currentDemandKWH / MAX_KWH_PER_TICK) * 2 - 1);
// 		EnergyConsumerUpdateDemand(&sys->consumer, 1);
// 	}

// 	fclose(fp);

// 	ann_train_batch_fixed(ann, "demand_data.txt", NUM_TRAIN_EPOCHS, TRAIN_ERROR, debug);

// 	// test the ann
// 	FILE *fp2;
// 	fp2 = fopen("demand_data_test.txt", "w");


// 	EnergyConsumerInit(&sys->consumer);
// 	float output[1] = {0};

// 	fprintf(fp2, "[");
// 	for(i = 0; i < NUM_TRAIN_TICKS; i++)
// 	{
// 		int tick_binary[TICK_STATES];
// 		float tick_float[TICK_STATES];
// 		if(i < 200)
// 			int_to_binary(i + 1 + 600, TICK_STATES, tick_binary);
// 		else if(i < 600)
// 			int_to_binary(i + 1 + 400, TICK_STATES, tick_binary);
// 		else
// 			int_to_binary(i + 1, TICK_STATES, tick_binary);
// 		for(j = 0; j < TICK_STATES; j++)
// 			tick_float[j] = tick_binary[j] == 1 ? 1.0 : -1.0;
// 		ann_run_fixed(tick_float, output, ann);
// 		if(i < NUM_TRAIN_TICKS - 1)
// 			fprintf(fp2, "%f, ", (output[0] + 1) / 2 * MAX_KWH_PER_TICK);
// 		else
// 			fprintf(fp2, "%f", (output[0] + 1) / 2 * MAX_KWH_PER_TICK);
// 		EnergyConsumerUpdateDemand(&sys->consumer, 1);
// 	}
// 	fprintf(fp2, "]\n");

// 	fclose(fp2);
// }

// // trains the price predictor
// void ann_train_price(ANN_F *ann, EVSystem *sys, int num_prev, int debug)
// {
// 	FILE *fp;
// 	fp = fopen("price_data.txt", "w");

// 	fprintf(fp, "%d\n", NUM_TRAIN_TICKS);

// 	int i, j;
// 	for(i = 0; i < NUM_TRAIN_TICKS; i++)
// 	{
// 		int tick_binary[TICK_STATES];
// 		int_to_binary(i + 1, TICK_STATES, tick_binary);
// 		for(j = 0; j < TICK_STATES; j++)
// 			fprintf(fp, "%f ", tick_binary[j] == 1 ? 1.0 : -1.0);
// 		fprintf(fp, "%f\n", (EnergyProducerGetCurrentMWHPrice(&sys->producer) / MAX_PRICE) * 2 - 1);
// 		EnergyProducerUpdatePrice(&sys->producer, 1);
// 	}

// 	fclose(fp);

// 	ann_train_batch_fixed(ann, "price_data.txt", NUM_TRAIN_EPOCHS, TRAIN_ERROR, debug);

// 	/// test the ann
// 	FILE *fp2;
// 	fp2 = fopen("price_data_test.txt", "w");

// 	EnergyProducerInit(&sys->producer);
// 	float output[1] = {0};

// 	fprintf(fp2, "[");
// 	for(i = 0; i < NUM_TRAIN_TICKS; i++)
// 	{
// 		int tick_binary[TICK_STATES];
// 		float tick_float[TICK_STATES];
// 		if(i < 200)
// 			int_to_binary(i + 1 + 600, TICK_STATES, tick_binary);
// 		else if(i < 600)
// 			int_to_binary(i + 1 + 400, TICK_STATES, tick_binary);
// 		else
// 			int_to_binary(i + 1, TICK_STATES, tick_binary);
// 		for(j = 0; j < TICK_STATES; j++)
// 			tick_float[j] = tick_binary[j] == 1 ? 1.0 : -1.0;
// 		ann_run(tick_float, output, ann);
// 		if(i < NUM_TRAIN_TICKS - 1)
// 			fprintf(fp2, "%f, ", (output[0] + 1) / 2 * MAX_PRICE);
// 		else
// 			fprintf(fp2, "%f", (output[0] + 1) / 2 * MAX_PRICE);
// 		EnergyProducerUpdatePrice(&sys->producer, 1);
// 	}
// 	fprintf(fp2, "]\n");

// 	fclose(fp2);
// }