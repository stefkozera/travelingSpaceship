/*
 * traveling_spaceship.cpp
 *
 *  Created on: Apr 15, 2025
 *      Author: stefk
 */


#include "traveling_spaceship.h"
#include "traveling_spaceship_ext.h"
#include "main.h"

/* Generate target coordinates randomly
*/
void generateCities(int (&f_targets)[NUM_TARGETS][3])
{
    for(int i = 0; i < NUM_TARGETS; ++i){
        for(int j = 0; j < 3; ++j){
            f_targets[i][j] = distanceRand(rng);
        }
    }
}

/* Calcuate distances of all combinations of targets in 3D space, for easy fitness referencing later
 */
void calcDistanceMatrix(int (&f_targets)[NUM_TARGETS][3],
                        double (&f_targetDistance)[NUM_TARGETS][NUM_TARGETS])
{
    double xDist, yDist, zDist;

    for(int i = 0; i < NUM_TARGETS; ++i){
        for(int j = 0; j < NUM_TARGETS; ++j){
            if(i == j) {
                f_targetDistance[i][j] = 0;
            }
            else {
                xDist = pow(f_targets[i][0] - f_targets[j][0], 2);
                yDist = pow(f_targets[i][1] - f_targets[j][1], 2);
                zDist = pow(f_targets[i][2] - f_targets[j][2], 2);

                f_targetDistance[i][j] = sqrt(xDist + yDist + zDist);
            }
        }
    }
}

/* Generate initial population for genetic algorithm.  Fill arrays in order, then apply fisher-yates shuffle
 */
void generateInitPopulation(int (&population)[PARENTS][NUM_TARGETS])
{
    int temp, num;

    //Seed initial population with values in order, will go to shuffle function
    for(int i = 0; i < PARENTS; ++i){
        for (int j = 0; j < NUM_TARGETS; ++j){
            population[i][j] = j;
        }
    }

    for(int i = 0; i < PARENTS; ++i){
        for (int j = NUM_TARGETS - 1; j > 0; --j) {
            num = targetsRand(rng);
            temp = population[i][num];
            population[i][num] = population[i][j];
            population[i][j] = temp;
        }
    }

    for(int i = 0; i < PARENTS; ++i){
        for (int j = 0; j < NUM_TARGETS; ++j){
        }
    }
}

/* Calculate fitness of a single individual member in the population
 */
double fitnessCalc(int *f_individual,
                   double (&f_targetDistance)[NUM_TARGETS][NUM_TARGETS])
{
    double totalDistance = 0;

    for(int i = 0; i < NUM_TARGETS - 1; ++i)
    {
        totalDistance += f_targetDistance[f_individual[i]][f_individual[i+1]];
    }
    totalDistance += f_targetDistance[f_individual[NUM_TARGETS - 1]][f_individual[0]];

    return totalDistance;
}

/* Crossover: randomly generate one pivot point, then take up to the pivot point from parent one
 * and fill in the other half of the pivot point from parent 2 in order, skipping targets already used
*/
void crossover(int *f_child, int *f_parent1, int *f_parent2)
{
    int pivot = pivotRand(rng);
    int parent2Index = pivot + 1;

    for(int i = 0; i <= pivot; ++i){
        f_child[i] = f_parent1[i];
    }

    for(int i = pivot + 1; i < NUM_TARGETS; ++i){

        while(true){

            int alreadyUsed = 0;
            for(int j = 0; j < i; ++j){
                if(f_child[j] == f_parent2[parent2Index]){
                    alreadyUsed = 1;
                }
            }

            if(alreadyUsed == 1){
                parent2Index++;
                if(parent2Index >= NUM_TARGETS){
                    parent2Index = 0;
                }
            }
            else{
                f_child[i] = f_parent2[parent2Index];
                break;

            }
        }
    }
}

/* Randomly pick two targets and swap them
*/
void mutate(int *f_child)
{
    int mutate1ind = targetsRand(rng);
    int mutate2ind = targetsRand(rng);
    int mutateChance = mutateRand(rng);

    if(mutateChance <= MUTATIONRATE){
        int temp = f_child[mutate1ind];
        f_child[mutate1ind] = f_child[mutate2ind];
        f_child[mutate2ind] = temp;
    }
}

extern "C" double geneticAlgorithm(timerCycleDataStruct *f_timeCycleData)
{

	/* Initialization */
    //////////////////////////////////////////////////

	uint32_t firstCycleCount, secondCycleCount, firstTimestamp, secondTimestamp;

#ifdef TIMINGDATA
	uint32_t firstCycleBreakout, secondCycleBreakout, firstTimeBreakout, secondTimeBreakout;
#endif

	*SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	*DWT_LAR = 0xC5ACCE55; // enable access
	*DWT_CYCCNT = 0; // reset the counter
	*DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk ; // enable the counter

    // Targets array, NUM_TARGETS x 3 ()
    int targets[NUM_TARGETS][3] = { { } };

    generateCities(targets);

    //Genetic Algorithm Start!
    ////////////////////////////////////////////////////////

    firstCycleCount = *DWT_CYCCNT;
    firstTimestamp = HAL_GetTick();

#ifdef TIMINGDATA
    firstCycleBreakout =  *DWT_CYCCNT;
    firstTimeBreakout = HAL_GetTick();
#endif

    // Distances between all targets, NUM_TARGETS x NUM_TARGETS
    double targetDistance[NUM_TARGETS][NUM_TARGETS] = { { } };

    // Initial population array, PARENTS x NUM_TARGETS
    int population[PARENTS][NUM_TARGETS] = { { } };

    // Selection Population, POPULATION x NUM_TARGETS
    // Top half is best 50% from population
    // Bottom half is children obtained from crossover
    int selectionPop[PARENTS][NUM_TARGETS] = { { } };

    // One single created child population, NUM_TARGETS
    int child[NUM_TARGETS] = { };

    // Vector for nth_element
    std::vector<std::pair<double, int> > fitnessPop;
    fitnessPop.reserve(PARENTS);
    for(int i = 0; i < PARENTS; ++i){
        fitnessPop.push_back(std::make_pair(0, i));
    }

    calcDistanceMatrix(targets, targetDistance);

#ifdef TIMINGDATA
    secondCycleBreakout =  *DWT_CYCCNT;
    secondTimeBreakout = HAL_GetTick();

    f_timeCycleData->DistanceMatrixCycle = secondCycleBreakout - firstCycleBreakout;
    f_timeCycleData->DistanceMatrixTime = secondTimeBreakout - firstTimeBreakout;

    firstCycleBreakout =  *DWT_CYCCNT;
    firstTimeBreakout = HAL_GetTick();
#endif

    generateInitPopulation(population);

#ifdef TIMINGDATA
    secondCycleBreakout =  *DWT_CYCCNT;
    secondTimeBreakout = HAL_GetTick();

    f_timeCycleData->GeneratePopulationCycle = secondCycleBreakout - firstCycleBreakout;
    f_timeCycleData->GeneratePopulationTime = secondTimeBreakout - firstTimeBreakout;

    firstCycleBreakout =  *DWT_CYCCNT;
    firstTimeBreakout = HAL_GetTick();
#endif

    /* Genetic algorithm iteration */
    //////////////////////////////////////////////////
   for(int gen = 0; gen < GENERATIONS; gen++){

        //vector pair, with fitness of entire population, and indexes of those fitness location
        //when compared to original population
        for (int i = 0; i < PARENTS; ++i){
            double fitness = fitnessCalc(population[i], targetDistance);
            fitnessPop[i] = std::make_pair(fitness, i);
        }

#ifdef TIMINGDATA
    secondCycleBreakout =  *DWT_CYCCNT;
    secondTimeBreakout = HAL_GetTick();

    f_timeCycleData->CalculatePopulationFitnessCycle = secondCycleBreakout - firstCycleBreakout;
    f_timeCycleData->CalculatePopulationFitnessTime = secondTimeBreakout - firstTimeBreakout;

    firstCycleBreakout =  *DWT_CYCCNT;
    firstTimeBreakout = HAL_GetTick();
#endif

        std::nth_element(fitnessPop.begin(), fitnessPop.begin() + HALF_PARENTS, fitnessPop.end());

        //Fill in the selection matrix with top picks from sorting
        for (int i = 0; i < HALF_PARENTS; ++i){
            for(int j = 0; j < NUM_TARGETS; ++j){
                selectionPop[i][j] = population[fitnessPop[i].second][j];
            }
        }

#ifdef TIMINGDATA
    secondCycleBreakout =  *DWT_CYCCNT;
    secondTimeBreakout = HAL_GetTick();

    f_timeCycleData->SelectionSortingCycle = secondCycleBreakout - firstCycleBreakout;
    f_timeCycleData->SelectionSortingTime = secondTimeBreakout - firstTimeBreakout;

    firstCycleBreakout =  *DWT_CYCCNT;
    firstTimeBreakout = HAL_GetTick();
#endif

    	//Crossover and mutation
        for(int i = HALF_PARENTS; i < PARENTS; ++i)
        {
            int parent1ind = crossoverRand(rng);
            int parent2ind = crossoverRand(rng);

            crossover(child, selectionPop[parent1ind], selectionPop[parent2ind]);
            mutate(child);

            for(int j = 0; j < NUM_TARGETS; ++j){
                selectionPop[i][j] = child[j];
            }

        }

        //replace population with selection population to prepare for next generation
        for(int i = 0; i < PARENTS; ++i){
            for(int j = 0; j < NUM_TARGETS; ++j)
            {
                population[i][j] = selectionPop[i][j];
            }
        }

#ifdef TIMINGDATA
    secondCycleBreakout =  *DWT_CYCCNT;
    secondTimeBreakout = HAL_GetTick();

    f_timeCycleData->CrossoverMutationCycle = secondCycleBreakout - firstCycleBreakout;
    f_timeCycleData->CrossoverMutationTime = secondTimeBreakout - firstTimeBreakout;
#endif

    }

    //Genetic Algorithm End!
    ////////////////////////////////////////////////////////

    secondCycleCount = *DWT_CYCCNT;
    secondTimestamp = HAL_GetTick();

    f_timeCycleData->cycleCount = secondCycleCount - firstCycleCount;
    f_timeCycleData->timeElapsed = secondTimestamp - firstTimestamp;

    double finalFitness;

    for (int i = 0; i < PARENTS; ++i){
        double fitness = fitnessCalc(population[i], targetDistance);

        if (i == 0)
        {
            finalFitness = fitness;
        }
        else
        {
            if(finalFitness > fitness)
            {
                finalFitness = fitness;
            }
        }
    }

    //return best finalFitness
    return finalFitness;
}

