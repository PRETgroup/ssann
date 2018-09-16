#ifndef EP_H
#define EP_H

#pragma once

#include "system_config.h"

struct EnergyProducer {
    double currentKWHPrice;
    double futureKWHPrice;
    double previousKWHPrice;
    double totalProducedKWH;
    int currentTick;
    int day;
};

void EnergyProducerInit(struct EnergyProducer* me, int day, int pinc);
void EnergyProducerUpdatePrice(struct EnergyProducer* me, int deltaTick);
void EnergyProducerProduceKWH(struct EnergyProducer* me, double numKWH);
double EnergyProducerGetCurrentMWHPrice(struct EnergyProducer* me);
double EnergyProducerGetCurrentKWMINPrice(struct EnergyProducer* me);

static const int TOD_Custom[11][2] = {{0, 2}, {3, 1}, {5, 0}, {12, 2}, {14, 0}, {16, 1}, {20, 2}, {24, 1}, {34, 0}, {38, 2}, {45, 0}};

static const float price[1440 / TICKS_PER_PRICE_PERIOD][7] = {
{97.42, 74.13, 97.23, 135.8, 76.83, 76.83, 74.51},
{91.02, 72.05, 86.03, 99.16, 75.35, 73.7, 67.84},
{96.5, 69.26, 75.61, 77.19, 75.49, 68.11, 68.07},
{85.71, 67.8, 73.04, 74.17, 73.12, 67.29, 68.37},
{82.15, 66.81, 67.72, 72.15, 65.56, 66.07, 62.73},
{76.81, 65.69, 67.26, 67.65, 64.77, 63.53, 62.09},
{79.32, 64.69, 65.7, 66.95, 64.38, 63.51, 61.37},
{70.9, 64.69, 64.92, 66.97, 64.28, 63.96, 60.38},
{78.12, 64.37, 65.4, 67.02, 64.26, 63.5, 61.19},
{80.53, 64.46, 65.65, 66.92, 64.36, 63.71, 60.13},
{80.76, 65.5, 67, 67.75, 64.55, 64, 61.49},
{83.05, 67.23, 72.43, 75.05, 64.81, 64.96, 61.37},
{98.35, 69.89, 75.53, 77.89, 65.44, 64.93, 62.3},
{110.12, 72.45, 85.04, 98.85, 66.77, 66.4, 62.47},
{71.73, 64.45, 65.91, 68.47, 68.13, 67.86, 63.53},
{79.58, 70.71, 71.29, 76.68, 67.91, 72.03, 63.02},
{81.7, 73.99, 73.15, 105.23, 73.22, 70.72, 66.23},
{82.15, 74.68, 78.59, 95.63, 76.78, 74.51, 67.33},
{91.04, 82.67, 93.97, 105.34, 77.26, 77.02, 68.37},
{97.54, 93.85, 124.65, 83.92, 85.82, 95.78, 70.4},
{99.7, 125.15, 190.4, 94.34, 81.95, 129.48, 69.29},
{146.29, 238.16, 250.55, 77.87, 95.21, 239.49, 70.35},
{100.31, 224.94, 279.31, 76.84, 95.76, 254.78, 70.27},
{98.44, 141.39, 201.3, 70.96, 94.65, 138.42, 70.39},
{90.21, 100, 189.92, 68.93, 88.92, 114.4, 70.19},
{91.72, 110.28, 249.42, 70.13, 88.84, 94.57, 71.01},
{91.51, 159.82, 308.78, 72.38, 89.13, 92.31, 72.5},
{91.67, 351.89, 347.53, 71.55, 86.44, 92.33, 72.89},
{91.57, 352.79, 279.5, 69.62, 86.28, 86.84, 73.18},
{86.31, 304.96, 240.44, 73.15, 86.37, 85.65, 72.48},
{89.48, 241.81, 178.38, 77.08, 86.1, 84.6, 73.27},
{86.32, 241.2, 140.48, 125.86, 78.77, 77.59, 73.29},
{84.99, 241.27, 135.37, 99.95, 78.91, 76.91, 73.11},
{84.86, 191.32, 98.9, 133.17, 78.03, 75.19, 72.46},
{81.26, 102.62, 90.95, 97.58, 77.72, 75.02, 72.22},
{83.17, 111.18, 76.08, 77.12, 91.99, 74.04, 75.12},
{82.01, 87.08, 75.74, 76.41, 99.15, 74.61, 77.74},
{83.12, 88.39, 94.26, 76.8, 92.51, 83.88, 77.98},
{97.21, 98.43, 96.21, 81.65, 137.24, 96.98, 84.59},
{99.82, 99.09, 104.69, 81.69, 178.22, 98.05, 97.38},
{97.87, 101.06, 112.54, 77.33, 198.32, 105.6, 134.27},
{191.42, 240.8, 239.95, 97.41, 222.7, 111.01, 141.57},
{157.95, 240.73, 243.69, 77.14, 246.81, 96.42, 136.77},
{124.96, 122.65, 179.28, 72.94, 138.33, 77.41, 124.99},
{110.87, 97.61, 140.85, 73.1, 86.83, 79.65, 100.33},
{78.32, 76.43, 99.22, 67.88, 81.91, 76.37, 83.32},
{75.25, 74.22, 85.51, 68.13, 108.2, 74.55, 95.04},
{71.28, 66.91, 76.99, 64.56, 77.62, 74.78, 79.81}
};

#endif