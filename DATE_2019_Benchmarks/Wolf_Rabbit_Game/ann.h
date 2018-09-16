/*
 * ANN.h
 *
 *  Created on: Mar 5, 2018
 *      Author: keyan
 */

#ifndef ANN_H_
#define ANN_H_

#pragma once

// includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// macros
#define DO_PRAGMA_(x) _Pragma (#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)

#define RUN_ANN(INPUTS, OUTPUTS, ANN, i, j, k, weighted_sum, MAX_NEURONS, MAX_LAYERS_LESS1)\
	DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
	for(i = 0; i < ANN->layers[0]; i++) \
	{\
		ANN->neurons[0][i] = INPUTS[i];\
		ANN->sums[0][i] = 0; \
	}\
	DO_PRAGMA(loopbound min 0 max MAX_LAYERS_LESS1)\
	for(i = 1; i < ANN->num_layers; i++) \
	{\
		DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
		for(j = 0; j < ANN->layers[i]; j++) \
		{\
			weighted_sum = 0;\
			DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
			for(k = 0; k < ANN->layers[i - 1]; k++)\
			{\
				weighted_sum += ANN->neurons[i - 1][k] * ANN->weights[i - 1][j * ANN->layers[i - 1] + k];\
			}\
			if(ANN->bias){weighted_sum += ANN->weights[i - 1][ANN->layers[i] * ANN->layers[i - 1] + j];}\
			ANN->sums[i][j] = weighted_sum; \
			ANN->neurons[i][j] = ann_activation(ANN->activation[i - 1], weighted_sum);\
		}\
	}\
	DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
	for(i = 0; i < ANN->layers[ANN->num_layers - 1]; i++)\
	{\
		OUTPUTS[i] = ANN->neurons[ANN->num_layers - 1][i];\
	}

#define INIT_ANN_CUSTOM(ANN, NUM_LAYERS, LAYERS, WEIGHTS, BIAS, ACTIVATION, i, j, num_weights, MAX_WEIGHTS_BIAS, MAX_LAYERS, MAX_LAYERS_LESS1)\
	i = 0;\
	j = 0;\
	num_weights = 0;\
	ANN->max_weights = 0;\
	ANN->num_layers = NUM_LAYERS;\
DO_PRAGMA(loopbound min 0 max MAX_LAYERS)\
	for(i = 0; i < NUM_LAYERS; i++)\
	{\
		ANN->layers[i] = LAYERS[i];\
		if(i > 0 && i < NUM_LAYERS + 1)\
		{\
			num_weights = LAYERS[i - 1]*LAYERS[i];\
			if(BIAS)\
			{\
				num_weights += LAYERS[i];\
			}\
DO_PRAGMA(loopbound min 0 max MAX_WEIGHTS_BIAS)\
			for(j = 0; j < num_weights; j++)\
			{\
				ANN->weights[i - 1][j] = WEIGHTS[i - 1][j];\
			}\
			if(num_weights > ANN->max_weights)\
			{\
				ANN->max_weights = num_weights;\
			}\
		}\
	}\
	ANN->bias = BIAS;\
DO_PRAGMA(loopbound min 0 max MAX_LAYERS_LESS1)\
	for(i = 0; i < ANN->num_layers - 1; i++)\
	{\
		ANN->activation[i] = ACTIVATION[i];\
	}

// definitions
#define MAX_LAYERS 			3
#define MAX_LAYERS_LESS1	2
#define MAX_NEURONS     	50
#define MAX_WEIGHTS      	1000
#define MAX_WEIGHTS_BIAS  	1000
#define MAX_DATA       		1000 
#define MAX_EPOCHS			1000
#define LEARNING_RATE 		0.7
#define LR_STEP 			0.9
#define LR_EPOCHS 			100
#define MOMENTUM 			0.0
#define LINEAR_A 			1

// sigmoid activation functions
//#define tanh(sum) (2.0f/(1.0f + exp(-2.0f * sum)) - 1.0f) // [-1, 1] // math.h -> activation: tanh(double), derivative: pow(1/cosh(double), 2)
#define sigmoid(sum) 	(1.0f/(1.0f + exp(-1.0f * sum))) // [0, 1]
#define relu(sum) 		(sum > 0 ? sum : 0)
#define linear(sum)		(LINEAR_A * sum)

// ANN type struct
typedef struct{
	int num_layers; // stores number of layers (I + H + ... + O)
	int layers[MAX_LAYERS]; // stores number of neurons per layer (I, H, ..., O)
	float weights[MAX_LAYERS][MAX_WEIGHTS_BIAS]; // stores weight values
	float delta_weights[MAX_LAYERS][MAX_WEIGHTS_BIAS]; // stores previous weight update values for use in momentum calculations
	int max_weights;
	// weights are assigned by future neuron, i.e. the first x weights belong to future neuron 1, the next x to future neuron 2, etc...
	// weights per layer = (neurons in previous layer + 1) * neurons in next layer
	float neurons[MAX_LAYERS][MAX_NEURONS]; // stores neuron output values during calculations (current training values)
	float sums[MAX_LAYERS][MAX_NEURONS]; // stores neuron weighted sum values during calculations (current training values)
	int bias;
	int activation[MAX_LAYERS];
}ANN;

// train data struct
struct Train_Data{
	int size; // stores the number of samples
	float inputs[MAX_DATA][MAX_NEURONS]; // stores the data inputs
	float outputs[MAX_DATA][MAX_NEURONS]; // stored the data expected outputs
};

// functions
void ann_init(ANN * ann, int num_layers, int layers[], int bias, int activation[]);
void ann_init_custom(ANN * ann, int num_layers, int layers[], int max_weights, float weights[][max_weights], int bias, int activation[]);
void ann_init_file(ANN * ann, int num_layers, int layers[], int bias, int activation[], char * filename);
void ann_run(float inputs[], float outputs[], ANN *ann);
void ann_run_layer(float inputs[], float outputs[], ANN *ann, int layer);
float ann_activation(int activation, float sum);
void ann_train_online(ANN *ann, char * filename, int epochs, float error);
void ann_get_deltas_online(ANN *ann, float outputs[], float expected_outputs[], int max_weights, float delta_accumulate[][max_weights], float lr);
void ann_train_batch(ANN *ann, char * filename, int epochs, float error, int debug);
void ann_get_deltas_batch(ANN *ann, float outputs[], float expected_outputs[], int max_weights, float delta_accumulate[][max_weights], float lr);
void ann_print(ANN *ann, float inputs[], int weights_only);
void ann_save_to_file(ANN *ann, char * filename);












#endif /* ANN_H_ */
