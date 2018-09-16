#include "main.h"

// #define TRAIN
// #define TRAINONLY
// #define RUNONLY

#ifndef TRAIN
int ess();
void ess_I_price(float);
void ess_I_demand(float);
void ess_I_soc(float);

// global variables
int out_flag = 0;
float batteryKW = 0;
#endif

int main(){
    #ifdef TRAIN
    srand(time(NULL));

    // Training section of main (asynchronous)
    EVSystem sys;

    ANN_F Q;
    ANN_F price_ann;
    ANN_F demand_ann;
    fix16_t q_weightsf[0][0];
    fix16_t price_weightsf[0][0];
    fix16_t demand_weightsf[0][0];
	float score_days[7];
	float soc[8] = {0};
	float pinc[8] = {0};
	float dinc[8] = {0};

	for(int day = 0; day < 7; day++)
	{
		int i, j, k;
		int runs = 1;
		float lowest2 = 9999999;
		float lowest_train = 9999999;
		float lowest_epoch = 9999999;
		int epoch_pos = 0;

		for(k = 0; k < runs; k++)
		{
			int tickCount;

			#ifndef RUNONLY
			printf("\n<=========== Starting day %d, system %d training with %d ticks (SoC = %f) =============>\n", day, k, NUM_TRAIN_TICKS_MAIN, soc[day]);

			init_train(&Q, &price_ann, &demand_ann, 0, 0, 0, q_weightsf, price_weightsf, demand_weightsf); // initialise system and ANNs
			EVSystemInit(&sys, day, soc[day], pinc[day], dinc[day]);

			for(i = 0; i < NUM_TRAIN_EPOCHS_MAIN; i++)
			{
				float explore_factor = EXPLORE_START + (((float)i/(EXPLORE_INCREASE - EXPLORE_EPOCH_MOD) > EXPLORE_MAX) ? EXPLORE_MAX : (float)i/(EXPLORE_INCREASE - EXPLORE_EPOCH_MOD));
				// printf("\n<===== EPOCH %d (%f) =====>\n", i, explore_factor);
				tickCount = 0;
				fix16_t delta_accumulate[Q.num_layers - 1][Q.max_weights]; // same size as number of layers of weights
				EVSystemInit(&sys, day, soc[day], pinc[day], dinc[day]);
				while(tickCount < NUM_TRAIN_TICKS_MAIN) {
					EVSystemTrainTick(&sys, &Q, &price_ann, &demand_ann, tickCount, i, Q.max_weights, delta_accumulate);
					tickCount++;
				}

				tickCount--;

				if(lowest_epoch > sys.currentExpense)
				{
					lowest_epoch = sys.currentExpense;
					epoch_pos = i;
					char str[30];
					sprintf(str, "Q_ann_best_epoch.net");
					ann_save_to_file_fixed(&Q, str);
				}
			}

			if(lowest_train > sys.currentExpense)
				lowest_train = sys.currentExpense;
			#endif

			#ifndef TRAINONLY
			// printf("<=========== Starting system %d running with %d ticks =============>\n", k, NUM_RUN_TICKS);

			EVSystem sys3;
			EVSystemInit(&sys3, day, soc[day], pinc[day], dinc[day]);

			tickCount = 0;

			price_increasing = pinc[day];
			demand_increasing = dinc[day];
			while(tickCount++ < NUM_RUN_TICKS) {
				float price = sys3.producer.currentKWHPrice;
				float demand = sys3.consumer.currentDemandKWH;
				float battery_level = sys3.battery.SoC * BATTERY_MAX_KWH;  // battery level in KWH
				float previous_demand = sys3.consumer.previousDemandKWH;
				float future_price = sys3.producer.futureKWHPrice;

				#ifdef RUNONLY
				int qnull = 1;
				#else
				int qnull = 0;
				#endif

				// printf("TICK: %d;\tFP: %f, PD: %f, P: %f, D: %f, B: %f\n", tickCount, future_price, previous_demand, price, demand, battery_level);

				sys3.TOD = run_ESS_ANN(&Q, future_price, previous_demand, price, demand, battery_level, qnull, day);  // get Q Value for this action
				EVSystemRunTick(&sys3, NULL);
			}

void ESS_run(inputs_ESS_t* inputs, outputs_ESS_t* outputs);

			tickCount--;

			if(lowest2 > sys3.currentExpense)
			{
				lowest2 = sys3.currentExpense;
				#ifndef RUNONLY
				char filename[50];
				sprintf(filename, "Q_ann_best_perfect_day%d.net", day);
				ann_save_to_file_fixed(&Q, filename);
				#endif
				soc[day + 1] = sys3.battery.SoC;
				pinc[day + 1] = sys3.priceIncreasing;
				dinc[day + 1] = sys3.demandIncreasing;
			}
			printf("Best on day %d score during running so far using perfect predictions was: %f.\n\n", day, lowest2);
		}

		score_days[day] = lowest2;

		printf("Best score on day %d during training was: %f\n", day, lowest_train);
		printf("Best score on day %d during any epoch (%d) was: %f\n", day, epoch_pos, lowest_epoch);
		printf("Best score on day %d during running using perfect predictions was: %f.\n", day, lowest2);
		#else
		}
		printf("Best score on day %d during training was: %f\n", day, lowest_train);
		#endif
	}

	printf("\n<=========== Best predictions by day ===========>\n");
	for(int i = 0; i < 7; i++)
	{
		printf("Best score on day %d during running using perfect predictions was: %f. End of day SoC is: %f\n", i, score_days[i], soc[i + 1]);
	}
	#else

	// Running section of main (synchronous)
	#define INTEGER
	// #define ENFORCER
	// #define SAFE
	// #define OVERCURRENT
	// #define SOC
	#define OVERHEAD		1
	#define RUN_FROM		1
	#define RUN_TO			1
	#define RESULTS

	float lowest[4] = {1.0, 1.0, 1.0, 1.0};
	float highest[4] = {0, 0, 0, 0};
	int time_over[4] = {0, 0, 0, 0};
	int time_under[4] = {0, 0, 0, 0};
	int over_current[4] = {0, 0, 0, 0};
	float time_taken[4] = {0, 0, 0, 0};

	float expense[4] = {0, 0, 0, 0};
	int minutes[4] = {0, 0, 0, 0};

	float amount_demanded = 0;
	float amount_supplied = 0;

	enforcervars_ESS_t F_me; 
	inputs_ESS_t F_inputs; 
	outputs_ESS_t F_outputs;

	enforcervars_soc_t soc_me; 
	inputs_soc_t soc_inputs; 
	outputs_soc_t soc_outputs;

	enforcervars_overcurrent_t oc_me; 
	inputs_overcurrent_t oc_inputs; 
	outputs_overcurrent_t oc_outputs;

	#ifdef OVERHEAD	
	float total_time[4] = {0, 0, 0, 0};
	for(int times = 0; times < OVERHEAD; times++)
	{

	for(int Osys = RUN_FROM; Osys < RUN_TO + 1; Osys++)
	{
		clock_t start, end;
		start = clock();
	#endif

		// Testing variables

	if(Osys == 1)
	{
		// Enforcer initialization
		ESS_init_all_vars(&F_me, &F_inputs, &F_outputs);
	}
	else if (Osys == 2)
	{
		soc_init_all_vars(&soc_me, &soc_inputs, &soc_outputs);
	}
	else if (Osys == 3)
	{
		overcurrent_init_all_vars(&oc_me, &oc_inputs, &oc_outputs);
	}

	// System initialization
	EVSystem * sys = init(0, 0.25, 0, 0);

	int i;

	ess_reset();
	ess();

	ess_I_price(EnergyProducerGetCurrentMWHPrice(&sys->producer));   
	ess_I_demand(EnergyConsumerGetDemandKWH(&sys->consumer));
	ess_I_soc(BatteryGetKWH(&sys->battery));
	for(i = 0; i < (1439 * 3); i++)
	{
		#ifdef OVERHEAD
		if(Osys == 0)
		{

		}
		if(Osys == 1)
		{
			F_inputs.SoC = BatteryGetKWH(&sys->battery) * 60.0;
			F_outputs.charge = batteryKW > 0;
			F_outputs.watts = abs(batteryKW);
		}
		if(Osys == 2)
		{
			soc_inputs.SoC = BatteryGetKWH(&sys->battery) * 60.0;
			soc_outputs.charge = batteryKW > 0;
			soc_outputs.watts = abs(batteryKW);
		}
		if (Osys == 3)
		{
			oc_inputs.SoC = BatteryGetKWH(&sys->battery) * 60.0;
			oc_outputs.charge = batteryKW > 0;
			oc_outputs.watts = abs(batteryKW);
		}
		#endif

		// #ifdef INTEGER
		// printf("Before: Enforcer inputs: %d;\tEnforcer outputs: %d, %d\n", 
		// 	F_inputs.SoC, F_outputs.charge, F_outputs.watts);
		// #else
		// printf("Before: Enforcer inputs: %f;\tEnforcer outputs: %d, %f\n", 
		// 	fix16_to_float(F_inputs.SoC), F_outputs.charge, fix16_to_float(F_outputs.watts));
		// #endif

		// Enforce the inputs/outputs
		#ifdef ENFORCER
		#ifdef SAFE
		ESS_run_output_enforcer_safe(&F_me, &F_inputs, &F_outputs);
		#endif
		#ifdef SOC
		soc_run_output_enforcer_safe(&soc_me, &soc_inputs, &soc_outputs);
		#endif
		#ifdef OVERCURRENT
		overcurrent_run_output_enforcer_safe(&oc_me, &oc_inputs, &oc_outputs);
		#endif
		#endif

		#ifdef OVERHEAD
		if(Osys == 1)
			ESS_run_output_enforcer_safe(&F_me, &F_inputs, &F_outputs);
		if (Osys == 2)
			soc_run_output_enforcer_safe(&soc_me, &soc_inputs, &soc_outputs);
		if (Osys == 3)
			overcurrent_run_output_enforcer_safe(&oc_me, &oc_inputs, &oc_outputs);
		#endif

		if(out_flag)
		{
			out_flag = 0;
			minutes[Osys]++;

			#ifdef OVERHEAD
			float batterykWh;
			if(Osys == 0)
			{
				batterykWh = batteryKW; // INTEGER
				if(batteryKW > 200)
					over_current[Osys]++;
				else if(batteryKW < -150)
					over_current[Osys]++;
			}
			if(Osys == 1)
			{
				batterykWh = (F_outputs.charge ? 1 : -1) * F_outputs.watts; // INTEGER
				if(F_outputs.charge && F_outputs.watts > 200)
					over_current[Osys]++;
				else if(F_outputs.charge == -1 && F_outputs.watts >150)
					over_current[Osys]++;
			}
			if (Osys == 2)
			{
				batterykWh = (soc_outputs.charge ? 1 : -1) * soc_outputs.watts; // INTEGER
				if(soc_outputs.charge && soc_outputs.watts > 200)
					over_current[Osys]++;
				else if(soc_outputs.charge == -1 && soc_outputs.watts >150)
					over_current[Osys]++;
			}
			if (Osys == 3)
			{
				batterykWh = (oc_outputs.charge ? 1 : -1) * oc_outputs.watts; // INTEGER
				if(oc_outputs.charge && oc_outputs.watts > 200)
					over_current[Osys]++;
				else if(oc_outputs.charge == -1 && oc_outputs.watts >150)
					over_current[Osys]++;
			}
			#endif

			sys->battery.SoC += batterykWh/((float)BATTERY_MAX_KWH * 60.0);

			// check if SoC is at a peak or trough
			if(sys->battery.SoC < lowest[Osys])
				lowest[Osys] = sys->battery.SoC;
			if(sys->battery.SoC > highest[Osys])
				highest[Osys] = sys->battery.SoC;

			// check if SoC is too low or too high
			if(sys->battery.SoC < BATTERY_MIN_SAFE)
				time_under[Osys]++;
			if(sys->battery.SoC > BATTERY_MAX_SAFE)
				time_over[Osys]++;

			// ============== BENCHMARK OUTPUT ==============
			// if(F_outputs.charge)
			// {
			// 	if(sys->battery.SoC + batterykWh/((float)BATTERY_MAX_KWH * 60.0) > 0.95)
			// 	{
			// 		printf("Here1\n");
			// 		batterykWh = 589 * 60 - sys->battery.SoC * (float)BATTERY_MAX_KWH * 60.0;
			// 		sys->battery.SoC = 0.95;
			// 	}
			// 	else
			// 		sys->battery.SoC += batterykWh/((float)BATTERY_MAX_KWH * 60.0);
			// }
			// else
			// {
			// 	if(sys->battery.SoC + batterykWh/((float)BATTERY_MAX_KWH * 60.0) < 0.25)
			// 	{
			// 		printf("Here2\n");
			// 		batterykWh = 155 * 60 - sys->battery.SoC * (float)BATTERY_MAX_KWH * 60.0;
			// 		sys->battery.SoC = 0.25;
			// 	}
			// 	else
			// 		sys->battery.SoC += batterykWh/((float)BATTERY_MAX_KWH * 60.0);
			// }
			// =========== END BENCHMARK ============

			// printf("Battery changing: %f\n", batterykWh);

			float demandThisTick = EnergyConsumerGetDemandKWH(&sys->consumer);

			float afterBattery = demandThisTick + batterykWh;

			amount_demanded += demandThisTick;
			amount_supplied += demandThisTick;

			// printf("Final demanded kW this minute: %f\n", demandThisTick);

			EnergyProducerProduceKWH(&sys->producer, afterBattery);
			expense[Osys] += afterBattery * EnergyProducerGetCurrentKWMINPrice(&sys->producer);
			UpdateChildren(sys);

			// Provide new system inputs
			ess_I_price(EnergyProducerGetCurrentMWHPrice(&sys->producer));   
			ess_I_demand(demandThisTick);
			ess_I_soc(BatteryGetKWH(&sys->battery));
		}
		ess();
	}

	#ifdef OVERHEAD
	printf("Overhead run time: %d\n", times);
	#endif
	#ifdef RESULTS
	printf("Expense after %d minutes: $ %f\n", minutes[Osys], expense[Osys]);
	printf("Total time taken is: %f s\n", time_taken[Osys]);
	printf("Number of times overcurrent was detected: %d\n", over_current[Osys]);
	printf("Number of ticks battery SoC was dangerously high: %d\n", time_over[Osys]);
	printf("Number of ticks battery SoC was dangerously low: %d\n", time_under[Osys]);
	printf("Lowest SoC this run (%f min safe): %f\n", BATTERY_MIN_SAFE, lowest[Osys]);
	printf("Highest SoC this run (%f max safe): %f\n", BATTERY_MAX_SAFE, highest[Osys]);
	printf("%f Watts demanded in total\n", amount_demanded);
	printf("%f Watts supplied in total\n", amount_supplied);

	char buffer[50];
	sprintf(buffer, "results_sys%d.txt", Osys);
	FILE *fp;
	fp = fopen(buffer, "w");

	fprintf(fp, "Expense after %d minutes: $ %f\n", minutes[Osys], expense[Osys]);
	fprintf(fp, "Total time taken is: %f s\n", time_taken[Osys]);
	fprintf(fp, "Number of times overcurrent was detected: %d\n", over_current[Osys]);
	fprintf(fp, "Number of ticks battery SoC was dangerously high: %d\n", time_over[Osys]);
	fprintf(fp, "Number of ticks battery SoC was dangerously low: %d\n", time_under[Osys]);
	fprintf(fp, "Lowest SoC this run (%f min safe): %f\n", BATTERY_MIN_SAFE, lowest[Osys]);
	fprintf(fp, "Highest SoC this run (%f max safe): %f\n", BATTERY_MAX_SAFE, highest[Osys]);
	fprintf(fp, "%f Watts demanded in total\n", amount_demanded);
	fprintf(fp, "%f Watts supplied in total\n", amount_supplied);

	fclose(fp);
	#endif

	#ifdef OVERHEAD
		end = clock();
		total_time[Osys] += ((double) (end - start)) / CLOCKS_PER_SEC;
	}
	printf("Overhead run time: %d\n", times);
	#endif

	}

	#ifdef OVERHEAD
	for(int k = 0; k < 4; k++)
	{
		printf("Total time taken: %f\n", total_time[k]);
		FILE * fp;

		if(k == 0)
			fp = fopen("noenf_results.txt", "w");
		if(k == 1)
			fp = fopen("AND_results.txt", "w");
		if(k == 2)
			fp = fopen("soc_results.txt", "w");
		if(k == 3)
			fp = fopen("oc_results.txt", "w");

		fprintf(fp, "Total time taken: %f\n", total_time[k]);

		fclose(fp);
	}
	#endif
	#endif 

    return 0;
}

// Esterel output function
void ess_O_watts(float KWH)
{ 
	out_flag = 1;
	batteryKW = KWH;
}

void ESS_run(inputs_ESS_t* inputs, outputs_ESS_t* outputs){}
void soc_run(inputs_soc_t* inputs, outputs_soc_t* outputs){}
void overcurrent_run(inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs){}
