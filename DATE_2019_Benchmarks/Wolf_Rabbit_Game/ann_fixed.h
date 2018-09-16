/*
 * ANN.h
 *
 *  Created on: Mar 5, 2018
 *      Author: keyan
 */

// This Fixed Point Artificial Neural Network implementation uses the C Fixed Point library "libfixmath" by MIT: 
// Copyright <2018> <MIT>

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef ANNF_H_
#define ANNF_H_

#pragma once

// includes
#include "ann.h"
#include "libfixmath/fix16.h"
#include "inttypes.h"

// macros
#define DO_PRAGMA_(x) _Pragma (#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)


#define ANN_CLEAR_SUMS(ANN, MAX_NEURONS)\
	DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
	for(i = 0; i < ANN->layers[0]; i++) \
	{\
		ANN->sums[0][i] = 0; \
	}

#define RUN_ANN_FIXED(INPUTS, OUTPUTS, ANN, i, j, k, weighted_sum, MAX_NEURONS, MAX_LAYERS_LESS1)\
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
				weighted_sum = fix16_add(weighted_sum, fix16_mul(ANN->neurons[i - 1][k], ANN->weights[i - 1][j * ANN->layers[i - 1] + k]));\
			}\
			if(ANN->bias)\
			{\
				weighted_sum = fix16_add(weighted_sum, ANN->weights[i - 1][ANN->layers[i] * ANN->layers[i - 1] + j]);\
			}\
			ANN->sums[i][j] = weighted_sum; \
			ANN->neurons[i][j] = ann_activation_fixed(ANN->activation[i - 1], weighted_sum);\
		}\
	}\
	DO_PRAGMA(loopbound min 0 max MAX_NEURONS)\
	for(i = 0; i < ANN->layers[ANN->num_layers - 1]; i++)\
	{\
		OUTPUTS[i] = ANN->neurons[ANN->num_layers - 1][i];\
	}

#define RUN_ANN_LAYER_FIXED(INPUTS, OUTPUTS, ANN, LAYER, i, j, k, weighted_sum, MAX_NEURONS1, MAX_NEURONS2)\
	if(LAYER > 0)\
	{\
		i = LAYER;\
		j = 0;\
		k = 0;\
DO_PRAGMA(loopbound min 0 max MAX_NEURONS2)\
		for(j = 0; j < ANN->layers[i]; j++)\
		{\
			weighted_sum = 0;\
DO_PRAGMA(loopbound min 0 max MAX_NEURONS1)\
			for(k = 0; k < ANN->layers[i - 1]; k++)\
			{\
				weighted_sum = fix16_add(weighted_sum, fix16_mul(INPUTS[k], ANN->weights[i - 1][j * ANN->layers[i - 1] + k]));\
			}\
			if(ANN->bias)\
			{\
				weighted_sum = fix16_add(weighted_sum, ANN->weights[i - 1][ANN->layers[i] * ANN->layers[i - 1] + j]);\
			}\
			ANN->sums[i][j] = weighted_sum;\
			ANN->neurons[i][j] = ann_activation_fixed(ANN->activation[i - 1], weighted_sum);\
		}\
DO_PRAGMA(loopbound min 0 max MAX_NEURONS2)\
		for(j = 0; j < ANN->layers[i]; j++)\
		{\
			OUTPUTS[j] = ANN->neurons[i][j];\
		}\
	}

#define INIT_ANN_CUSTOM_FIXED(ANN, NUM_LAYERS, LAYERS, WEIGHTS, BIAS, ACTIVATION, i, j, num_weights, MAX_WEIGHTS_BIAS, MAX_LAYERS, MAX_LAYERS_LESS1)\
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

#define sigmoid_f(sum) 		(fix16_div(fix16_from_int(1), fix16_add(fix16_from_int(1), fix16_exp(fix16_mul(fix16_from_int(-1), sum))))) // [0, 1]
#define relu_f(sum) 		(sum > 0 ? sum : 0)
#define linear_f(sum)		(fix16_mul(fix16_from_int(LINEAR_A), sum))
#define tanh_f(sum)			(fix16_div(\
							fix16_sub(fix16_from_int(1), fix16_exp(fix16_mul(fix16_from_int(-2), sum))),\
							fix16_add(fix16_from_int(1), fix16_exp(fix16_mul(fix16_from_int(-2), sum)))\
							))
#define cosh_f(sum)			(fix16_div(\
							fix16_add(fix16_from_int(1), fix16_exp(fix16_mul(fix16_from_int(-2), sum))),\
							fix16_mul(fix16_from_int(2), fix16_exp(fix16_mul(fix16_from_int(-1), sum)))\
							))

#define LEARNING_RATE_FIXED fix16_from_float(0.7)

// ANN type struct
typedef struct{
	int num_layers; // stores number of layers (I + H + ... + O)
	int layers[MAX_LAYERS]; // stores number of neurons per layer (I, H, ..., O)
	fix16_t weights[MAX_LAYERS][MAX_WEIGHTS_BIAS]; // stores weight values
	fix16_t delta_weights[MAX_LAYERS][MAX_WEIGHTS_BIAS]; // stores previous weight update values for use in momentum calculations
	int max_weights;
	// weights are assigned by future neuron, i.e. the first x weights belong to future neuron 1, the next x to future neuron 2, etc...
	// weights per layer = (neurons in previous layer + 1) * neurons in next layer
	fix16_t neurons[MAX_LAYERS][MAX_NEURONS]; // stores neuron output values during calculations (current training values)
	fix16_t sums[MAX_LAYERS][MAX_NEURONS]; // stores neuron weighted sum values during calculations (current training values)
	int bias;
	int activation[MAX_LAYERS];
}ANN_F;

// train data struct
struct Train_Data_Fixed{
	int size; // stores the number of samples
	fix16_t inputs[MAX_DATA][MAX_NEURONS]; // stores the data inputs
	fix16_t outputs[MAX_DATA][MAX_NEURONS]; // stored the data expected outputs
};

// functions

// Initialization
void ann_init_float(ANN * ann, ANN_F * annf);
void ann_init_fixed(ANN_F * ann, int num_layers, int layers[], int bias, int activation[]);
void ann_init_fixed_custom(ANN_F * ann, int num_layers, int layers[], int max_weights, fix16_t weights[][max_weights], int bias, int activation[]);
void ann_init_fixed_file(ANN_F * ann, int num_layers, int layers[], int bias, int activation[], char * filename);

// Run functions
void ann_run_fixed(fix16_t inputs[], fix16_t outputs[], ANN_F *ann);
void ann_run_fixed_layer(fix16_t inputs[], fix16_t outputs[], ANN_F *ann, int layer);
fix16_t ann_activation_fixed(int activation, fix16_t sum);

// Training
void ann_train_batch_fixed(ANN_F *ann, char * filename, int epochs, float error, int debug);
void ann_train_online_fixed(ANN_F *ann, int dataSize, fix16_t trainData[][2 * MAX_NEURONS], int epochs, float error, int debug);
void ann_get_deltas_fixed(ANN_F *ann, fix16_t outputs[], fix16_t expected_outputs[], int max_weights, fix16_t delta_accumulate[][max_weights], fix16_t lr, int resetAccumulate);

// Output
void ann_print_fixed(ANN_F *ann, fix16_t inputs[], int weights_only);
void ann_save_to_file_fixed(ANN_F *ann, char * filename);












#endif /* ANN_H_ */
