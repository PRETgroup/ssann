#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#pragma once

#include <math.h>

#define TICKS_PER_HOUR          				60
#define TICKS_PER_PRICE_PERIOD  				30
#define TICKS_PER_DEMAND_PERIOD                 1

#define BATTERY_STORAGE_EFFICIENCY              1
#define BATTERY_START_KWH                       0.9
#define BATTERY_MAX_KWH                         620
#define BATTERY_MIN_SAFE                        0.25  // never discharge less than 25 %
#define BATTERY_MAX_SAFE                        0.95  // only charge if less than 95 %
#define BATTERY_MAX_CHARGE_KWH                  200
#define BATTERY_MAX_DISCHARGE_KWH               150
#define KWMIN                                   60000.0

#define PEAK_DISCHARGE_KWH                      130

#define PI 										3.14159265

#define GAMMA									0.9
#define NUM_TRAIN_EPOCHS_MAIN                   50
#define NUM_RUN_TICKS 							1440
#define NUM_TRAIN_TICKS_MAIN					1440
#define EXPLORE_FACTOR                          1.5
#define EXPLORE_INCREASE						(EXPLORE_FACTOR * NUM_TRAIN_EPOCHS_MAIN)
#define EXPLORE_MAX								0.9
#define EXPLORE_START							0.1

#define LEARNING_RATE_Q                         0.2
#define EPOCH_TRAINING                          0
#define EXPLORE_EPOCH_MOD                       (EPOCH_TRAINING == 1 ? 0 : 0.0 * NUM_TRAIN_EPOCHS_MAIN)
#define REWARD_FACTOR 							0.0
#define FUTURE_VALUE_FACTOR						0.5

#define PREVIOUS_VALUES      					1
#define NUM_TRAIN_TICKS      					1440
#define NUM_TRAIN_EPOCHS						100
#define TRAIN_ERROR								0.01
//#define TICK_STATES        					((int)(log2(NUM_TRAIN_TICKS)) + 1)
#define TICK_STATES                             10



#endif


























