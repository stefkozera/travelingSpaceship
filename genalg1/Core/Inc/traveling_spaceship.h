#ifndef __TRAVELING_SPACESHIP_H
#define __TRAVELING_SPACESHIP_H

#include <stdlib.h>
#include <math.h>
#include <random>
#include <algorithm>

//Number of cities, targets, in 3D space.  Will get random locations in 3 coordinates from 0 to MAX_DISTANCE
#define NUM_TARGETS 16
//Number of parents, chromosomes in genetic algorithm
#define PARENTS 64
//Number of generations to run genetic algorithm for
#define GENERATIONS 500
//Max distance for targets, all target coordinates will be between 0 and this value
#define MAX_DISTANCE 1000
//% mutation rate out of 100
#define MUTATIONRATE 10

#define HALF_PARENTS PARENTS/2

//Clock cycle register counting

volatile unsigned int *DWT_CYCCNT   = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL  = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *DWT_LAR      = (volatile unsigned int *)0xE0001FB0; //address of the register
volatile unsigned int *SCB_DEMCR    = (volatile unsigned int *)0xE000EDFC; //address of the register

/*Random number generator*/
static std::random_device rd;
static std::mt19937 rng(rd());
std::uniform_int_distribution<int> targetsRand(0, NUM_TARGETS - 1);
std::uniform_int_distribution<int> pivotRand(0, NUM_TARGETS - 2);
std::uniform_int_distribution<int> distanceRand(0, MAX_DISTANCE);
std::uniform_int_distribution<int> crossoverRand(0, HALF_PARENTS - 1);
std::uniform_int_distribution<int> mutateRand(1, 100);


/*function declarations */
void generateCities(int (&f_targets)[NUM_TARGETS][3]);
void calcDistanceMatrix(int (&f_targets)[NUM_TARGETS][3],
                        double (&f_targetDistance)[NUM_TARGETS][NUM_TARGETS]);
void generateInitPopulation(int (&population)[PARENTS][NUM_TARGETS]);
double fitnessCalc(int *f_individual,
                   double (&f_targetDistance)[NUM_TARGETS][NUM_TARGETS]);
void crossover(int *f_child, int *f_parent1, int *f_parent2);
void mutate(int *f_child);

#endif
