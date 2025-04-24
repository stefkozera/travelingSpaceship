/*
 * traveling_spaceship_ext.h
 *
 *  Created on: Apr 15, 2025
 *      Author: stefk
 */

#ifndef INC_TRAVELING_SPACESHIP_EXT_H_
#define INC_TRAVELING_SPACESHIP_EXT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//Enable to save timing of individual portions at expense of total timing data
//Best to enable this with a generation count of 1.  It will only save data from the last generation

//#define TIMINGDATA

//Timing structure
typedef struct{
	uint32_t timeElapsed;
	uint32_t cycleCount;
#ifdef TIMINGDATA
	uint32_t DistanceMatrixTime;
	uint32_t DistanceMatrixCycle;
	uint32_t GeneratePopulationTime;
	uint32_t GeneratePopulationCycle;
	uint32_t CalculatePopulationFitnessTime;
	uint32_t CalculatePopulationFitnessCycle;
	uint32_t SelectionSortingTime;
	uint32_t SelectionSortingCycle;
	uint32_t CrossoverMutationTime;
	uint32_t CrossoverMutationCycle;
#endif
}timerCycleDataStruct;

//external interface
double geneticAlgorithm(timerCycleDataStruct *f_timeCycleData);

#ifdef __cplusplus
}
#endif


#endif /* INC_TRAVELING_SPACESHIP_EXT_H_ */
