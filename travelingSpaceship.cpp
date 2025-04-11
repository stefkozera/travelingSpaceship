#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>

#define NUM_TARGETS 10
#define POPULATION 20
#define GENERATIONS 500
#define MAX_DISTANCE 1000

static std::random_device rd;
static std::mt19937 rng(rd()); 
std::uniform_int_distribution<int> targetsRand(0, NUM_TARGETS - 1);
std::uniform_int_distribution<int> distanceRand(0, MAX_DISTANCE);

void generateCities(int **f_targets)
{
    for(int i = 0; i < NUM_TARGETS; ++i){
        for(int j = 0; j < 3; ++j){
            f_targets[i][j] = distanceRand(rng);
            printf("%d ",f_targets[i][j]);
        }
        printf("\n");
    }
}

void calcDistanceMatrix(int **f_targets, double **f_targetDistance)
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
            printf("%f ", f_targetDistance[i][j]);
        }
        printf("\n");
    }
}

void generateInitPopulation(int **population)
{
    int temp, num;

    //Seed initial population with values in order, will go to shuffle function
    for(int i = 0; i < POPULATION; ++i){
        for (int j = 0; j < NUM_TARGETS; ++j){
            population[i][j] = j;
        }
    }

    for(int i = 0; i < POPULATION; ++i){
        for (int j = NUM_TARGETS - 1; j > 0; --j) {
            num = targetsRand(rng);
            temp = population[i][num];
            population[i][num] = population[i][j];
            population[i][j] = temp;
        }
    }

    for(int i = 0; i < POPULATION; ++i){
        for (int j = 0; j < NUM_TARGETS; ++j){
            printf("%d ", population[i][j]);
        }
        printf("\n");
    }
}

double fitnessCalc(int *f_individual, double **f_targetDistance)
{
    double totalDistance = 0;

    for(int i = 0; i < NUM_TARGETS - 1; ++i)
    {
        totalDistance += f_targetDistance[f_individual[i]][f_individual[i+1]];
        //printf("%f ", totalDistance);
    }
    totalDistance += f_targetDistance[f_individual[NUM_TARGETS - 1]][f_individual[0]];
    //printf("%f\n", totalDistance);

    return totalDistance;
}
void selection()
{
    
}

int main()
{
    // Targets array, NUM_TARGETS x 3 ()
    int **targets = new int*[NUM_TARGETS];
    for(int i = 0; i < NUM_TARGETS; ++i){
        targets[i] = new int[3];
    }

     // Distances between all targets, NUM_TARGETS x NUM_TARGETS
    double **targetDistance = new double*[NUM_TARGETS];
    for(int i = 0; i < NUM_TARGETS; ++i){
        targetDistance[i] = new double[NUM_TARGETS];
    }

    // Initial population array, NUM_TARGETS x POPULATION
    int **population = new int*[POPULATION];
    for(int i = 0; i < POPULATION; ++i){
        population[i] = new int[NUM_TARGETS];
    }

    generateCities(targets);

    calcDistanceMatrix(targets, targetDistance);

    generateInitPopulation(population);

    double *fitnessPop = new double[POPULATION];

    for (int i = 0; i < POPULATION; ++i){
        fitnessPop[i] = fitnessCalc(population[i], targetDistance);
        printf("%f\n", fitnessPop[i]);
    }



 


    for(int gen = 0; gen < GENERATIONS; gen++){

    }
}