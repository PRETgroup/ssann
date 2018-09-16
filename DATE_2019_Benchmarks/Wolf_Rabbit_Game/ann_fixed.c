/*
 * ANN.c
 *
 *  Created on: Mar 5, 2018
 *      Author: keyan
 */

#include "ann_fixed.h"

// create a fixed ann from an existing float ann
void ann_init_float(ANN * ann, ANN_F * annf)
{
	int i, j;

	// copy integer values
	annf->bias = ann->bias;
	annf->max_weights = ann->max_weights;
	annf->num_layers = ann->num_layers;

	// zero delta weights
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		annf->activation[i] = ann->activation[i];
	}
	#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < MAX_LAYERS; i++)
	{
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
		for(j = 0; j < MAX_WEIGHTS_BIAS; j++)
		{
			annf->delta_weights[i][j] = 0;
		}
	}

	// copy weight values
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < annf->num_layers; i++)
	{
		annf->layers[i] = ann->layers[i];
		if(i > 0 && i < annf->num_layers + 1) // assign weights to layers between first and last set of neurons
		{
			int num_weights = annf->layers[i - 1]*annf->layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(annf->bias)
				num_weights += annf->layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(j = 0; j < num_weights; j++)
			{
				annf->weights[i - 1][j] = fix16_from_float(ann->weights[i - 1][j]);
			}
		}
	}
}

// create NN with random weights
void ann_init_fixed(ANN_F * ann, int num_layers, int layers[], int bias, int activation[])
{
	int i = 0;
	int j = 0;
	ann->max_weights = 0;
	//srand(time(NULL));

#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < MAX_LAYERS; i++)
	{
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
		for(j = 0; j < MAX_WEIGHTS_BIAS; j++)
		{
			ann->delta_weights[i][j] = 0;
		}
	}

	ann->num_layers = num_layers;
	// printf("ANN -> Constructing layers.\n");
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < num_layers; i++)
	{
		ann->layers[i] = layers[i];
		if(i > 0 && i < num_layers + 1) // assign weights to layers between first and last set of neurons
		{
			int num_weights = layers[i - 1]*layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(bias)
				num_weights += layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(j = 0; j < num_weights; j++)
			{
				fix16_t weight = fix16_from_float((float)rand()/(float)(RAND_MAX) - 0.5); // random fixed point between -0.5 and 0.5
				ann->weights[i - 1][j] = weight;
			}

			if(num_weights > ann->max_weights) // reassign max_weights if needed (with bias weights)
				ann->max_weights = num_weights;
		}
	}

	// printf("ANN -> Applying bias and activation.\n");
	ann->bias = bias;
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		ann->activation[i] = activation[i];
	}
}

// create NN with custom weights
void ann_init_fixed_custom(ANN_F * ann, int num_layers, int layers[], int max_weights, fix16_t weights[][max_weights], int bias, int activation[])
{
	// printf("ANN -> Beginning initialization of ANN.\n");
	int i = 0;
	int j = 0;
	//srand(time(NULL));
	ann->max_weights = 0;

	ann->num_layers = num_layers;
	// printf("ANN -> Constructing layers.\n");
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < num_layers; i++)
	{
		ann->layers[i] = layers[i];
		if(i > 0 && i < num_layers + 1) // assign weights to layers between first and last set of neurons
		{
			int num_weights = layers[i - 1]*layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(bias)
				num_weights += layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(j = 0; j < num_weights; j++)
			{
				ann->weights[i - 1][j] = weights[i - 1][j];
			}
			if(num_weights > ann->max_weights) // reassign max_weights if needed (with bias weights)
				ann->max_weights = num_weights;
		}
	}

	// printf("ANN -> Applying bias and activation.\n");
	ann->bias = bias;
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		ann->activation[i] = activation[i];
	}
}

// initialise an ANN from an existing file
void ann_init_fixed_file(ANN_F * ann, int num_layers, int layers[], int bias, int activation[], char * filename)
{
	// printf("ANN -> Beginning initialization of ANN.\n");
	int i = 0;
	int j = 0;
	//srand(time(NULL));
	ann->max_weights = 0;
	FILE *fp;
	fp = fopen(filename, "r");

	// skip first line (shows layer structure)
	fscanf(fp, "%*[^\n]\n", NULL);

	ann->num_layers = num_layers;
	// printf("ANN -> Constructing layers.\n");
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < num_layers; i++)
	{
		ann->layers[i] = layers[i];
		if(i > 0 && i < num_layers + 1) // assign weights to layers between first and last set of neurons
		{
			int num_weights = layers[i - 1]*layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(bias)
			{
				num_weights += layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
				//printf("Has bias neurons: %d\n", num_weights);
			}
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(j = 0; j < num_weights; j++)
			{
				fix16_t weight;
				fscanf(fp, "%" SCNd32 , &weight);  // scan in next fixed weight

				ann->weights[i - 1][j] = weight;
				//printf("Adding weight %f to weights\n", weight);
			}
			if(num_weights > ann->max_weights) // reassign max_weights if needed (with bias weights)
				ann->max_weights = num_weights;
		}
	}

	// printf("ANN -> Applying bias and activation.\n");
	ann->bias = bias;
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		ann->activation[i] = activation[i];
	}

	fclose(fp);
}

// calculates activation value depending on activation type
fix16_t ann_activation_fixed(int activation, fix16_t sum)
{
	fix16_t result;
	switch(activation){
	case 0:
		result = sigmoid_f(sum);
		break;
	case 1:
		result = tanh_f(sum);
		break;
	case 2:
		result = relu_f(sum);
		break;
	case 3:
		result = linear_f(sum);
		break;
	default:
		result = sigmoid_f(sum);
		break;
	}

	return result;
}

// pass inputs through ANN, i.e. run the ANN
void ann_run_fixed(fix16_t inputs[], fix16_t outputs[], ANN_F *ann)
{
	int i = 0;
	int j = 0;
	int k = 0;

#pragma loopbound min 0 max MAX_NEURONS
	for(i = 0; i < ann->layers[0]; i++) // run through inputs and add to NN
	{
		//printf("Adding input: %f\n", inputs[i]);
		ann->neurons[0][i] = inputs[i];
		ann->sums[0][i] = 0; // no sums for inputs
	}

#pragma loopbound min 0 max MAX_LAYERS
	for(i = 1; i < ann->num_layers; i++) // run for every layer except input layer
	{
		//printf("Running through layer: %d\n", i);
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[i]; j++) // run through every neuron in current layer
		{	
			//printf("Running through current neuron: %d\n", j);
			fix16_t weighted_sum = 0;
#pragma loopbound min 0 max MAX_NEURONS
			for(k = 0; k < ann->layers[i - 1]; k++) // run through every neuron (input) in previous layer
			{
				//printf("Running through previous neuron %d with value: %f\n", k, ann->neurons[i - 1][k]);
				//printf("Multiplying it by weight: %f\n", ann->weights[i - 1][j * ann->layers[i - 1] + k]);
				weighted_sum = fix16_add(weighted_sum, fix16_mul(ann->neurons[i - 1][k], ann->weights[i - 1][j * ann->layers[i - 1] + k]));
			}

			if(ann->bias) // add bias if necessary (from last position in previous layer)
			{
				//printf("Bias present\n");
				weighted_sum = fix16_add(weighted_sum, ann->weights[i - 1][ann->layers[i] * ann->layers[i - 1] + j]); // add bias weight for respective neuron in current layer
			}

			ann->sums[i][j] = weighted_sum; // add weighted sum to sum array for future use
			//printf("Weighted sum is: %f\n", weighted_sum);

			ann->neurons[i][j] = ann_activation_fixed(ann->activation[i - 1], weighted_sum);
			//printf("Value after activation is: %f\n", activation);
		}
	}

#pragma loopbound min 0 max MAX_NEURONS
	for(i = 0; i < ann->layers[ann->num_layers - 1]; i++) // fill outputs for return
	{
		outputs[i] = ann->neurons[ann->num_layers - 1][i];
	}
}

// run only a single layer given the inputs (neurons outputs of previous layer) to that layer
void ann_run_fixed_layer(fix16_t inputs[], fix16_t outputs[], ANN_F *ann, int layer)
{
	if(layer > 0)
	{
		int i = layer;  // let 'i' be the current layer
		int j = 0;
		int k = 0;

		//printf("Running through layer: %d\n", i);
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[i]; j++) // run through every neuron in current layer
		{
			//printf("Running through current neuron: %d\n", j);
			fix16_t weighted_sum = 0;
#pragma loopbound min 0 max MAX_NEURONS
			for(k = 0; k < ann->layers[i - 1]; k++) // run through every neuron (input) in previous layer
			{
				//printf("Running through previous neuron %d with value: %f\n", k, ann->neurons[i - 1][k]);
				//printf("Multiplying it by weight: %f\n", ann->weights[i - 1][j * ann->layers[i - 1] + k]);
				weighted_sum = fix16_add(weighted_sum, fix16_mul(ann->neurons[i - 1][k], ann->weights[i - 1][j * ann->layers[i - 1] + k]));
			}

			if(ann->bias) // add bias if necessary (from last position in previous layer)
			{
				//printf("Bias present\n");
				weighted_sum = fix16_add(weighted_sum, ann->weights[i - 1][ann->layers[i] * ann->layers[i - 1] + j]); // add bias weight for respective neuron in current layer
			}
			ann->sums[i][j] = weighted_sum; // add weighted sum to sum array for future use
			//printf("Weighted sum is: %f\n", weighted_sum);

			ann->neurons[i][j] = ann_activation_fixed(ann->activation[i - 1], weighted_sum);
			//printf("Value after activation is: %f\n", activation);
		}

		// set outputs to current layer
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[i]; j++) // fill outputs for return
		{
			outputs[j] = ann->neurons[i][j];
		}
	}
}

//BATCH TRAINING -> NOW WORKING
void ann_train_batch_fixed(ANN_F *ann, char * filename, int epochs, float error, int debug)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int size;
	int num_outputs = ann->layers[ann->num_layers - 1];

	FILE *fp;
	fp = fopen(filename, "r");
	fscanf(fp, "%d", &size);
	if(size > MAX_DATA) // too many samples
		size = MAX_DATA;
	if(epochs > MAX_EPOCHS)
		epochs = MAX_EPOCHS; // too many epochs

	struct Train_Data_Fixed data; // copying file data to struct to prevent multiple file reads
#pragma loopbound min 0 max MAX_DATA
	for(i = 0; i < size; i++)
	{
		//printf("INPUTS:\n");
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[0]; j++) // reading inputs
		{
			fscanf(fp, "%ld", &(data.inputs[i][j]));
			//printf("%d[%d]: %d\n", i, j, data.inputs[i][j]);
		}
		//printf("OUTPUTS:\n");
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < num_outputs; j++) // reading outputs
		{
			fscanf(fp, "%ld", &(data.outputs[i][j]));
			//printf("%d[%d]: %d\n", i, j, data.outputs[i][j]);
		}
	}

	// if(debug)
	// {
	// 	printf("\n======================= TRAINING ======================\n\n");
	// 	printf("ANN -> Training with %d samples, over a maximum of %d epochs and error goal of %f.\n", size, epochs, error);
	// }

	// training only variables
	int num_epochs = 0;
	int num_weights = ann->num_layers - 1;
	fix16_t delta_accumulate[num_weights][ann->max_weights]; // same size as number of layers of weights
	float mse = 0; // error average of epoch
	fix16_t lr = fix16_from_float(LEARNING_RATE);

#pragma loopbound min 0 max MAX_EPOCHS
	do
	{
		// adapt learning rate (step reduction)
		if(num_epochs%LR_EPOCHS == 0 && num_epochs != 0)
		{
			lr = fix16_mul(lr, fix16_from_float(LR_STEP));
		}
		//printf("ANN -> Current learning rate is %f\n", lr);

		// zero delta_accumulate
#pragma loopbound min 0 max MAX_LAYERS_LESS1
		for(j = 0; j < num_weights; j++)
		{
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(k = 0; k < ann->max_weights; k++)
			{
				delta_accumulate[j][k] = 0;
			}
		}

		mse = 0; // zero mse for addition
#pragma loopbound min 0 max MAX_DATA
		for(i = 0; i < size; i++) // run through full set of data
		{
			fix16_t result[num_outputs]; // stores the result of the ann_run
			ann_run_fixed(data.inputs[i], result, ann); // run ANN with selected inputs
			// for(j = 0; j < num_outputs; j++)
			// 	printf("Result: %d\n", result[j]);

#pragma loopbound min 0 max MAX_NEURONS
			for(j = 0; j < num_outputs; j++) // run through outputs and get error
			{
				float output_error = fix16_to_float(fix16_sub(data.outputs[i][j], result[j])); // calculate output error
				mse += pow(output_error, 2); // add squared error to mse
			}

			ann_get_deltas_fixed(ann, result, data.outputs[i], ann->max_weights, delta_accumulate, lr, 0);
		}

		// calculate error
		mse /= (size * num_outputs); // divide error sum by total number of outputs
		num_epochs++;


		// average weight deltas and correct weights
#pragma loopbound min 0 max MAX_LAYERS_LESS1
		for(i = 0; i < num_weights; i++) // run through each layer
		{
			//printf("ANN -> Updating %d weights\n", ann->layers[i] * ann->layers[i + 1]);
#pragma loopbound min 0 max MAX_WEIGHTS
			for(j = 0; j < ann->layers[i] * ann->layers[i + 1]; j++) // run through each delta weight sum
			{
				fix16_t delta_weight = fix16_div(delta_accumulate[i][j], fix16_from_int(size));  // average delta accumulate by dividing by number of training samples
				//float delta_weight = delta_accumulate[i][j]; // DO NOT AVERAGE THE WEIGHTS
				//printf("ANN -> delta_weight is: %d/%d = %d\n", delta_accumulate[i][j], size, delta_weight);
				ann->weights[i][j] = fix16_add(ann->weights[i][j], delta_weight); // add to the corresponding weight
				ann->delta_weights[i][j] = delta_weight;
			}

			// add to bias neuron if necessary
			if(ann->bias)
			{
#pragma loopbound min 0 max MAX_NEURONS
				for(j = 0; j < ann->layers[i + 1]; j++) // run through each bias weight at end of the weights
				{
					// update bias' in the positions of the final weights
					fix16_t delta_weight = fix16_div(delta_accumulate[i][ann->layers[i] * ann->layers[i + 1] + j], fix16_from_int(size));
					//printf("Updating bias delta by %f\n", delta_weight);
					ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j] = fix16_add(ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j], delta_weight);
					ann->delta_weights[i][ann->layers[i] * ann->layers[i + 1] + j] = delta_weight;
				}
			}
		}

		// if(debug)
		// 	printf("EPOCH: %d		MSE: %f		LEARNING RATE: %f\n", num_epochs, mse, fix16_to_float(lr));
	}
	while(epochs > num_epochs && mse > error);

	// if(debug)
	// {
	// 	printf("\n");
	// 	printf("\n============= FINISHED TRAINING ==============\n\n");
	// }

	fclose(fp);
}

void ann_train_online_fixed(ANN_F *ann, int size, fix16_t trainData[][2 * MAX_NEURONS], int epochs, float error, int debug)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int num_outputs = ann->layers[ann->num_layers - 1];

	if(size > MAX_DATA) // too many samples
		size = MAX_DATA;
	if(epochs > MAX_EPOCHS)
		epochs = MAX_EPOCHS; // too many epochs

	// printf("Data size is: %d\n", size);

	// fix16_t trainData structured -> Input/Output set X: {Input1, Input2, Input3... InputN, Output1, Output2... OutputM}
	struct Train_Data_Fixed data; // copying file data to struct to prevent multiple file reads
#pragma loopbound min 0 max MAX_DATA
	for(i = 0; i < size; i++)
	{
		//printf("INPUTS:\n");
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[0]; j++) // reading inputs
		{
			data.inputs[i][j] = trainData[i][j];
			//printf("%d[%d]: %d\n", i, j, data.inputs[i][j]);
		}
		//printf("OUTPUTS:\n");
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < num_outputs; j++) // reading outputs
		{
			data.outputs[i][j] = trainData[i][j + ann->layers[0]];
			//printf("%d[%d]: %d\n", i, j, data.outputs[i][j]);
		}
	}

	// training only variables
	int num_epochs = 0;
	int num_weights = ann->num_layers - 1;
	fix16_t delta_weights[num_weights][ann->max_weights]; // same size as number of layers of weights
	float mse = 0; // error average of epoch
	fix16_t lr = fix16_from_float(LEARNING_RATE);

#pragma loopbound min 0 max MAX_EPOCHS
	do
	{
		// adapt learning rate (step reduction)
		if(num_epochs%LR_EPOCHS == 0 && num_epochs != 0)
		{
			lr = fix16_mul(lr, fix16_from_float(LR_STEP));
		}
		//printf("ANN -> Current learning rate is %f\n", lr);

		// zero delta_weights
#pragma loopbound min 0 max MAX_LAYERS_LESS1
		for(j = 0; j < num_weights; j++)
		{
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(k = 0; k < ann->max_weights; k++)
			{
				delta_weights[j][k] = 0;
			}
		}

		mse = 0; // zero mse for addition
#pragma loopbound min 0 max MAX_DATA
		for(i = 0; i < size; i++) // run through full set of data, updating weights after EACH input/output set
		{
			fix16_t result[num_outputs]; // stores the result of the ann_run
			ann_run_fixed(data.inputs[i], result, ann); // run ANN with selected inputs
			// for(j = 0; j < num_outputs; j++)
			// 	printf("Result: %d\n", result[j]);

#pragma loopbound min 0 max MAX_NEURONS
			for(j = 0; j < num_outputs; j++) // run through outputs and get error
			{
				float output_error = fix16_to_float(fix16_sub(data.outputs[i][j], result[j])); // calculate output error
				mse += pow(output_error, 2); // add squared error to mse
			}

			// get weight update for this input/output pair
			ann_get_deltas_fixed(ann, result, data.outputs[i], ann->max_weights, delta_weights, lr, 1);

			// update weights
			// average weight deltas and correct weights
			int x, y;
#pragma loopbound min 0 max MAX_LAYERS_LESS1
			for(x = 0; x < num_weights; x++) // run through each layer
			{
				//printf("ANN -> Updating %d weights\n", ann->layers[i] * ann->layers[i + 1]);
#pragma loopbound min 0 max MAX_WEIGHTS
				for(y = 0; y < ann->layers[x] * ann->layers[x + 1]; y++) // run through each delta weight sum
				{
					//printf("ANN -> delta_weight is: %d/%d = %d\n", delta_accumulate[i][j], size, delta_weight);
					ann->weights[x][y] = fix16_add(ann->weights[x][y], delta_weights[x][y]); // add to the corresponding weight
					ann->delta_weights[x][y] = delta_weights[x][y];
				}

				// add to bias neuron if necessary
				if(ann->bias)
				{
#pragma loopbound min 0 max MAX_NEURONS
					for(y = 0; y < ann->layers[x + 1]; y++) // run through each bias weight at end of the weights
					{
						// update bias' in the positions of the final weights
						fix16_t delta_weight = delta_weights[x][ann->layers[x] * ann->layers[x + 1] + y];
						//printf("Updating bias delta by %f\n", delta_weight);
						ann->weights[x][ann->layers[x] * ann->layers[x + 1] + y] = fix16_add(
							ann->weights[x][ann->layers[x] * ann->layers[x + 1] + y], delta_weight);
						ann->delta_weights[x][ann->layers[x] * ann->layers[x + 1] + y] = delta_weight;
					}
				}
			}
		}

		// calculate error
		mse /= (size * num_outputs); // divide error sum by total number of outputs
		num_epochs++;


		// if(debug)
		// 	printf("EPOCH: %d		MSE: %f		LEARNING RATE: %f\n", num_epochs, mse, fix16_to_float(lr));
	}
	while(epochs > num_epochs && mse > error);
}

// helper function to get the delta values of a single pass
void ann_get_deltas_fixed(ANN_F *ann, fix16_t outputs[], fix16_t expected_outputs[], int max_weights, fix16_t delta_accumulate[][max_weights], fix16_t lr, int resetAccumulate)
{
	int i = 0;
	int j = 0;
	int num_weights = ann->num_layers - 1;
	int layer = num_weights; // start at output layer
	fix16_t delta_sums[num_weights][max_weights]; // delta_sums

	//printf("Delta sums array of size: %d x %d\n", num_weights, max_weights);
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < num_weights; i++)
	{
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
		for(j = 0; j < max_weights; j++)
		{
			delta_sums[i][j] = 0;
		}
	}

	fix16_t learning_rate = lr;

#pragma loopbound min 0 max MAX_NEURONS
	for(i = 0; i < ann->layers[num_weights]; i++) // transform output layer into initial delta_sum
	{
		delta_sums[num_weights - 1][i] = fix16_sub(expected_outputs[i], outputs[i]); // calculate output error
		// printf("Output: %f\nDesired output: %f\nDelta sums output layer: %f\n", 
		// 		fix16_to_float(outputs[i]), fix16_to_float(expected_outputs[i]), fix16_to_float(delta_sums[num_weights - 1][i]));
	}

#pragma loopbound min 0 max MAX_LAYERS
	for(; layer > 0; layer--) // iterate through each layer, calculating the delta_sum and adding them to delta_sums
	{
		//printf("ANN -> In layer %d\n", layer);
#pragma loopbound min 0 max MAX_NEURONS
		for(i = 0; i < ann->layers[layer]; i++) // run through each neuron in the current layer
		{
			//printf("ANN -> In current neuron %d\n", i);
#pragma loopbound min 0 max MAX_NEURONS
			for(j = 0; j < ann->layers[layer - 1]; j++) // run through each neuron in the previous layer
			{
				//printf("ANN -> In previous neuron %d\n", j);
				if(layer > 1)
				{
					if(i == 0) // first neuron, so zero the delta_sums in previous layer
						delta_sums[layer - 2][j] = 0;
					// delta_sum i = wij * delta_j + wik * delta_k + ...
//					printf("ANN -> Adding %f * %f = %f to the current delta sum of %f\n", delta_sums[layer - 1][i],
//										ann->weights[layer - 1][i * ann->layers[layer - 1] + j],
//										delta_sums[layer - 1][i] * ann->weights[layer - 1][i * ann->layers[layer - 1] + j], delta_sums[layer - 2][j]);
					//delta_sums[layer - 2][j] += delta_sums[layer - 1][i] * ann->weights[layer - 1][i * ann->layers[layer - 1] + j]; // add to delta_sums for current layer
					delta_sums[layer - 2][j] = fix16_add(delta_sums[layer - 2][j], 
					fix16_mul(delta_sums[layer - 1][i], ann->weights[layer - 1][i * ann->layers[layer - 1] + j]));


//					if(delta_sums[layer - 2][j] < 1000)
//						printf("ANN -> Delta sum for previous neuron is currently %f\n", delta_sums[layer - 2][j]);
				}

				// at the same time, calculate weight updates for this current layer using the previous layer's delta values
				// weight update w'ij = learning_rate * delta_j * dy_j/d_sum * y_i
				// for now, default learning rate is 0.7

				// calculate gradient of error
				fix16_t error_gradient;
				switch(ann->activation[layer - 1]){
				case 0: // differentiate sigmoid(x) = f(x): f'(x) = f(x)[1 - f(x)]
					error_gradient = fix16_mul(ann->neurons[layer][i], fix16_sub(fix16_from_int(1), ann->neurons[layer][i])); // delta_sum = sigmoid(sum)(1 - sigmoid(sum))*output_error
					//rintf("ANN -> Error gradient of current neuron is %f * (1 - %f) = %f\n", ann->neurons[layer][i], ann->neurons[layer][i], error_gradient);
					break;
				case 1: // differentiate tanh(x) = f(x): f'(x) = sech(x)^2 = 1/cosh(x)^2
					error_gradient = fix16_mul(fix16_div(fix16_from_int(1), cosh_f(ann->sums[layer][i])), 
									fix16_div(fix16_from_int(1), cosh_f(ann->sums[layer][i]))); // delta_sum = (1/cosh(sum))^2 * output_error (ann->sums starts at 1, not 0)
					//printf("ANN -> Error gradient of current neuron is (1/cosh(%f))^2 = (1/%f)^2 = %f\n", ann->sums[layer][i], cosh(ann->sums[layer][i]), error_gradient);
					break;
				case 2:
					error_gradient = (ann->sums[layer][i] > 0 ? fix16_from_int(1) : 0);
					break;
				case 3:
					error_gradient = fix16_from_float(LINEAR_A);
					break;
				default: // differentiate sigmoid(x) = f(x): f'(x) = f(x)[1 - f(x)]
					error_gradient = fix16_mul(ann->neurons[layer][i], fix16_sub(fix16_from_int(1), ann->neurons[layer][i])); // delta_sum = sigmoid(sum)(1 - sigmoid(sum))*output_error
					break;
				}

				// calculate weight update
				fix16_t weight_update = fix16_mul(learning_rate, delta_sums[layer - 1][i]);
				weight_update = fix16_mul(weight_update, error_gradient);
				weight_update = fix16_mul(weight_update, ann->neurons[layer - 1][j]);
				weight_update = fix16_add(weight_update, 
				fix16_mul(fix16_from_float(MOMENTUM), ann->delta_weights[layer - 1][ann->layers[layer] * ann->layers[layer - 1] + i]));
				// printf("ANN -> Adding to accumulate delta weight %d by %d * %d * %d * %d * %d * %d = %d\n", (i * ann->layers[layer - 1] + j), 
				// learning_rate, delta_sums[layer - 1][i], error_gradient, ann->neurons[layer - 1][j], 
				// fix16_from_float(MOMENTUM), ann->delta_weights[layer - 1][i * ann->layers[layer - 1] + j], weight_update);

//				if(weight_update > 100)
//				{
//					printf("Large weight update: %f\n", weight_update);
//					printf("ANN -> Adding to accumulate delta weight %d by %f * %f * %f * %f + %f * %f = %f\n", (i * ann->layers[layer - 1] + j), learning_rate, delta_sums[layer - 1][i],
//						error_gradient, ann->neurons[layer - 1][j], MOMENTUM, ann->delta_weights[layer - 1][i * ann->layers[layer - 1] + j], weight_update);
//				}

				if(resetAccumulate)
					delta_accumulate[layer - 1][i * ann->layers[layer - 1] + j] = weight_update;
				else
					delta_accumulate[layer - 1][i * ann->layers[layer - 1] + j] = fix16_add(delta_accumulate[layer - 1][i * ann->layers[layer - 1] + j], weight_update);
			}

			// calculate bias delta weights
			if(ann->bias)
			{
				// calculate gradient of error
				fix16_t error_gradient;
				switch(ann->activation[layer - 1]){
				case 0: // differentiate sigmoid(x) = f(x): f'(x) = f(x)[1 - f(x)]
					error_gradient = fix16_mul(ann->neurons[layer][i], fix16_sub(fix16_from_int(1), ann->neurons[layer][i])); // delta_sum = sigmoid(sum)(1 - sigmoid(sum))*output_error
					//rintf("ANN -> Error gradient of current neuron is %f * (1 - %f) = %f\n", ann->neurons[layer][i], ann->neurons[layer][i], error_gradient);
					break;
				case 1: // differentiate tanh(x) = f(x): f'(x) = sech(x)^2 = 1/cosh(x)^2
					error_gradient = fix16_mul(fix16_div(fix16_from_int(1), cosh_f(ann->sums[layer][i])), 
									fix16_div(fix16_from_int(1), cosh_f(ann->sums[layer][i]))); // delta_sum = (1/cosh(sum))^2 * output_error (ann->sums starts at 1, not 0)
					//printf("ANN -> Error gradient of current neuron is (1/cosh(%f))^2 = (1/%f)^2 = %f\n", ann->sums[layer][i], cosh(ann->sums[layer][i]), error_gradient);
					break;
				case 2:
					error_gradient = (ann->sums[layer][i] > 0 ? fix16_from_int(1) : 0);
					break;
				case 3:
					error_gradient = fix16_from_float(LINEAR_A);
					break;
				default: // differentiate sigmoid(x) = f(x): f'(x) = f(x)[1 - f(x)]
					error_gradient = fix16_mul(ann->neurons[layer][i], fix16_sub(fix16_from_int(1), ann->neurons[layer][i])); // delta_sum = sigmoid(sum)(1 - sigmoid(sum))*output_error
					break;
				}
				// update bias weight and add to accumulate for bias connection to this neuron
				fix16_t weight_update = fix16_mul(learning_rate, delta_sums[layer - 1][i]);
				weight_update = fix16_mul(weight_update, error_gradient);
				weight_update = fix16_add(weight_update, 
				fix16_mul(fix16_from_float(MOMENTUM), ann->delta_weights[layer - 1][ann->layers[layer] * ann->layers[layer - 1] + i]));

				if(resetAccumulate)
					delta_accumulate[layer - 1][ann->layers[layer] * ann->layers[layer - 1] + i] = weight_update;
				else
					delta_accumulate[layer - 1][ann->layers[layer] * ann->layers[layer - 1] + i] = fix16_add(delta_accumulate[layer - 1][ann->layers[layer] * ann->layers[layer - 1] + i], weight_update);
			}
		}
	}
}

// display the ANN
void ann_print_fixed(ANN_F *ann, fix16_t inputs[], int weights_only)
{
	int i = 0;
	int j = 0;
	int k = 0;

	printf("\n+++++++++++++++ PRINTING NEURAL NETWORK STRUCTURE +++++++++++++++\n");
	printf("NUMBER OF LAYERS: %d\n", ann->num_layers);
	printf("NUMBER OF INPUTS: %d\n", ann->layers[0]);
	printf("NUMBER OF OUTPUTS: %d\n", ann->layers[ann->num_layers - 1]);
	printf("NEURONS PER LAYER: %d", ann->layers[0]);
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 1; i < ann->num_layers; i++)
	{
		printf(" -> %d", ann->layers[i]);
	}
	printf("\n");
	if(ann->bias)
		printf("THIS NEURAL NETWORK HAS BIAS NEURONS\n");
	else
		printf("THIS NEURAL NETWORK DOES NOT HAVE BIAS NEURONS\n");
	printf("ACTIVATION FUNCTION: [");
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		switch(ann->activation[i]){
		case 0:
			printf("SIGMOID");
			break;
		case 1:
			printf("TANH (SYMMETRIC SIGMOID)");
			break;
		case 2:
			printf("ReLU");
			break;
		case 3:
			printf("LINEAR");
			break;
		default:
			printf("SIGMOID");
			break;
		}
		if(i != ann->num_layers - 2)
			printf(", ");
		else
			printf("]");
	}


	if(!weights_only) // runs the neural network to get layer values if necessary
	{
		fix16_t outputs[ann->layers[ann->num_layers - 1]];
		ann_run_fixed(inputs, outputs, ann);
	}

#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++) // run through all layers except last layer
	{
		printf("\n<============ Listing LAYER %d ============>\n\n", i);
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[i]; j++) // run through all neurons in current layer
		{
			if(!weights_only) // only prints layer values if necessary
				printf("LAYER %d NEURON %d has a WEIGHTED INPUT SUM of %f (%d) and an ACTIVATION OUTPUT of %f (%d)\n", i, j, 
				fix16_to_float(ann->sums[i][j]), ann->sums[i][j], fix16_to_float(ann->neurons[i][j]), ann->neurons[i][j]);
			printf("LAYER %d NEURON %d has %d connections to LAYER %d:\n", i, j, ann->layers[i + 1], (i + 1));
#pragma loopbound min 0 max MAX_NEURONS
			for(k = 0; k < ann->layers[i + 1]; k++) // through all neurons in following layer
			{
				printf("-> Connection to NEURON %d in LAYER %d has a WEIGHT of %f (%d)\n", k, (i + 1), 
				fix16_to_float(ann->weights[i][k * ann->layers[i] + j]), ann->weights[i][k * ann->layers[i] + j]);
			}
		}
		if(ann->bias)
		{
			printf("LAYER %d has a BIAS NEURON with %d connections\n", i, ann->layers[i + 1]);
#pragma loopbound min 0 max MAX_NEURONS
			for(j = 0; j < ann->layers[i + 1]; j++) // run through bias neurons at the end
			{
				printf("-> Connection to NEURON %d in LAYER %d has a WEIGHT of %f (%d)\n", j, (i + 1), 
				fix16_to_float(ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j]), ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j]);
			}
		}
	}
	// display output layer
	if(!weights_only) // only finds output layer if necessary
	{
		printf("\n<============ Listing OUTPUT LAYER ============>\n\n");
		int output_layer = ann->num_layers - 1;
#pragma loopbound min 0 max MAX_NEURONS
		for(i = 0; i < ann->layers[output_layer]; i++)
		{
			printf("LAYER %d NEURON %d has a WEIGHTED INPUT SUM of %f (%d) and an ACTIVATION OUTPUT of %f (%d)\n",
					output_layer, i, fix16_to_float(ann->sums[output_layer][i]), ann->sums[output_layer][i], 
					fix16_to_float(ann->neurons[output_layer][i]), ann->neurons[output_layer][i]);
		}
	}

	printf("NUMBER OF LAYERS: %d\n", ann->num_layers);
	printf("NUMBER OF INPUTS: %d\n", ann->layers[0]);
	printf("NUMBER OF OUTPUTS: %d\n", ann->layers[ann->num_layers - 1]);
	printf("NEURONS PER LAYER: %d", ann->layers[0]);
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 1; i < ann->num_layers; i++)
	{
		printf(" -> %d", ann->layers[i]);
	}
	printf("\n");
	printf("\n+++++++++++++++ FINISHED PRINTING NEURAL NETWORK STRUCTURE +++++++++++++++\n\n");
}

void ann_save_to_file_fixed(ANN_F *ann, char * filename)
{
	int i = 0;
	int j = 0;
	int k = 0;

	FILE *fp;
	fp = fopen(filename, "w");

	fprintf(fp, "[");
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < ann->num_layers; i++)
	{
		if(i < ann->num_layers - 1)
			fprintf(fp, "%d, ", ann->layers[i]);
		else
			fprintf(fp, "%d", ann->layers[i]);
	}
	fprintf(fp, "]\n");

#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < ann->num_layers; i++)
	{
		if(i > 0 && i < ann->num_layers + 1) // print weights in-between layers between first and last set of neurons
		{
			int num_weights = ann->layers[i - 1]*ann->layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(ann->bias)
				num_weights += ann->layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			for(j = 0; j < num_weights; j++)
			{
				fprintf(fp, "%" PRId32 " " , ann->weights[i - 1][j]);  // print the weight
			}
			fprintf(fp, "\n");
		}
	}

	fprintf(fp, "\n{\n");
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 0; i < ann->num_layers; i++)
	{
		if(i > 0 && i < ann->num_layers + 1) // print weights in-between layers between first and last set of neurons
		{
			int num_weights = ann->layers[i - 1]*ann->layers[i]; // number of weights = no. in previous layer * no. in current layer
			if(ann->bias)
				num_weights += ann->layers[i]; // add the number of weights as neurons in following layer for bias (at last positions)
#pragma loopbound min 0 max MAX_WEIGHTS_BIAS
			fprintf(fp, "\t{");
			for(j = 0; j < num_weights; j++)
			{
				if(j != num_weights - 1)
					fprintf(fp, "%" PRId32 ", ", ann->weights[i - 1][j]);  // print the weight
				else
					fprintf(fp, "%" PRId32, ann->weights[i - 1][j]);  // print the weight
			}
			if(i != ann->num_layers - 1)
				fprintf(fp, "},\n");
			else
				fprintf(fp, "}\n");
		}
	}
	fprintf(fp, "};\n\n");

	fprintf(fp, "NUMBER OF LAYERS: %d\n", ann->num_layers);
	fprintf(fp, "NUMBER OF INPUTS: %d\n", ann->layers[0]);
	fprintf(fp, "NUMBER OF OUTPUTS: %d\n", ann->layers[ann->num_layers - 1]);
	fprintf(fp, "NEURONS PER LAYER: %d", ann->layers[0]);
#pragma loopbound min 0 max MAX_LAYERS
	for(i = 1; i < ann->num_layers; i++)
	{
		fprintf(fp, " -> %d", ann->layers[i]);
	}
	fprintf(fp, "\n");
	if(ann->bias)
		fprintf(fp, "THIS NEURAL NETWORK HAS BIAS NEURONS\n");
	else
		fprintf(fp, "THIS NEURAL NETWORK DOES NOT HAVE BIAS NEURONS\n");
	fprintf(fp, "ACTIVATION FUNCTION: ");
#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++)
	{
		switch(ann->activation[i]){
		case 0:
			fprintf(fp, "SIGMOID");
			break;
		case 1:
			fprintf(fp, "TANH (SYMMETRIC SIGMOID)");
			break;
		case 2:
			fprintf(fp, "ReLU");
			break;
		case 3:
			fprintf(fp, "LINEAR");
			break;
		default:
			fprintf(fp, "SIGMOID");
			break;
		}
		if(i != ann->num_layers - 2)
			fprintf(fp, ", ");
		else
			fprintf(fp, "]");
	}

#pragma loopbound min 0 max MAX_LAYERS_LESS1
	for(i = 0; i < ann->num_layers - 1; i++) // run through all layers except last layer
	{
		fprintf(fp, "\n<============ Listing LAYER %d ============>\n\n", i);
#pragma loopbound min 0 max MAX_NEURONS
		for(j = 0; j < ann->layers[i]; j++) // run through all neurons in current layer
		{
			fprintf(fp, "LAYER %d NEURON %d has %d connections to LAYER %d:\n", i, j, ann->layers[i + 1], (i + 1));
#pragma loopbound min 0 max MAX_NEURONS
			for(k = 0; k < ann->layers[i + 1]; k++) // through all neurons in following layer
			{
				fprintf(fp, "-> Connection to NEURON %d in LAYER %d has a WEIGHT of %f (%d)\n", k, (i + 1), 
				fix16_to_float(ann->weights[i][k * ann->layers[i] + j]), ann->weights[i][k * ann->layers[i] + j]);
			}
		}
		if(ann->bias)
		{
			fprintf(fp, "LAYER %d has a BIAS NEURON with %d connections\n", i, ann->layers[i + 1]);
#pragma loopbound min 0 max MAX_NEURONS
			for(j = 0; j < ann->layers[i + 1]; j++) // run through bias neurons at the end
			{
				fprintf(fp, "-> Connection to NEURON %d in LAYER %d has a WEIGHT of %f (%d)\n", j, (i + 1), 
				fix16_to_float(ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j]), ann->weights[i][ann->layers[i] * ann->layers[i + 1] + j]);
			}
		}
	}

	fclose(fp);
}

