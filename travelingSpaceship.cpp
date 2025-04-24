#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <algorithm>

#define NUM_TARGETS 35
#define PARENTS 100
#define GENERATIONS 500
#define MAX_DISTANCE 1000
#define MUTATIONRATE 10 //Out of 100

#define HALF_PARENTS PARENTS/2

static std::random_device rd;
static std::mt19937 rng(rd()); 
std::uniform_int_distribution<int> targetsRand(0, NUM_TARGETS - 1);
std::uniform_int_distribution<int> pivotRand(0, NUM_TARGETS - 2);
std::uniform_int_distribution<int> distanceRand(0, MAX_DISTANCE);
std::uniform_int_distribution<int> crossoverRand(0, HALF_PARENTS - 1);
std::uniform_int_distribution<int> mutateRand(1, 100);

void generateCities(int (&f_targets)[NUM_TARGETS][3])
{
    for(int i = 0; i < NUM_TARGETS; ++i){
        for(int j = 0; j < 3; ++j){
            f_targets[i][j] = distanceRand(rng);
        }
    }
}

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

int main()
{
    // Targets array, NUM_TARGETS x 3 ()
    int targets[NUM_TARGETS][3] = { { } };

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

    std::vector<std::pair<double, int> > fitnessPop;
    fitnessPop.reserve(PARENTS);
    for(int i = 0; i < PARENTS; ++i){
        fitnessPop.push_back(std::make_pair(0, i));
    }

    /* Initialization */
    //////////////////////////////////////////////////

    generateCities(targets);
    calcDistanceMatrix(targets, targetDistance);
    generateInitPopulation(population);

    /* Genetic algorithm iternation */
    //////////////////////////////////////////////////

   for(int gen = 0; gen < GENERATIONS; gen++){

        //vector pair, with fitnesses of entire population, and indexes of those fitnesses location
        //when compared to original population
        double bestFitness;

        for (int i = 0; i < PARENTS; ++i){
            double fitness = fitnessCalc(population[i], targetDistance);
            fitnessPop[i] = std::make_pair(fitness, i);

            if (i == 0){
                bestFitness = fitness;
            }
            else{
                if(bestFitness > fitness){
                    bestFitness = fitness;
                }
            }
        }

        printf("Gen: %d, Best Fitness: %f\n", gen, bestFitness);

        std::nth_element(fitnessPop.begin(), fitnessPop.begin() + HALF_PARENTS, fitnessPop.end());

        for (int i = 0; i < HALF_PARENTS; ++i){
            for(int j = 0; j < NUM_TARGETS; ++j){
                selectionPop[i][j] = population[fitnessPop[i].second][j];
            }
        }

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

        for(int i = 0; i < PARENTS; ++i){
            for(int j = 0; j < NUM_TARGETS; ++j)
            {
                population[i][j] = selectionPop[i][j];
            }
        }
    }

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

    printf("Final Fitness: %f\n", finalFitness);
}