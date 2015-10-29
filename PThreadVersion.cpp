//
// Created by paranjay on 10/27/15.
//

#include <iostream>
#include <vector>
#include <ctime>
#include "HelperMethods.h"
#include <pthread.h>
#define DELTA 0.005
#define THREAD_COUNT 2

void *updateTemperatureOfBox(void * parameterPack);

using namespace std;

struct ParamterPack
{
    vector<gridBox> gridBoxes;
    int i;
};

vector<gridBox> gridBoxes;

int runSimulation()
{
    bool loop = true;
    int iterationCount = 0;
    while(loop)
    {
        iterationCount++;
        // cout << "iteration number " << iterationCount<<endl;
        pthread_t threads[THREAD_COUNT];
        for(int i=0; i< gridBoxes.size(); i++)
        {
            ParamterPack pack1 = ParamterPack();
            pack1.gridBoxes = gridBoxes;
            pack1.i = i;


            updateTemperatureOfBox((void *) &pack1);
        }

        double max_temp = gridBoxes[0].newTemperature;
        double min_temp = gridBoxes[0].newTemperature;
        for(int i = 0; i<gridBoxes.size(); i++)
        {
            gridBoxes[i].temperature = gridBoxes[i].newTemperature;
            if(gridBoxes[i].temperature > max_temp)
                max_temp = gridBoxes[i].temperature;
            if(gridBoxes[i].temperature < min_temp)
                min_temp = gridBoxes[i].temperature;
        }

        if(hasSimulationConverged(gridBoxes))
        {
            printf("Max Temp : %f \nMin Temp : %f \n", max_temp, min_temp);
            loop = false;
        }
    }

    return iterationCount;
}

void *updateTemperatureOfBox(void* parameterPack) {
    // struct ParameterPack *pack = (struct ParameterPack*)parameterPack;
//    vector<gridBox> gridBoxes = (ParamterPack)parameterPack;
    ParamterPack *pack = static_cast<ParamterPack*>(parameterPack);
    vector<gridBox> gridBoxes1 = pack->gridBoxes;
    int i = pack->i;
    int distanceSum = 0;
    double weightedSum = 0.0;
    int distance = 0;
    int j=0;
    gridBox currentBox = gridBoxes1[i];

    if (currentBox.leftNeighCount == 0)
    {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.height;
    }
    if (currentBox.rightNeighCount == 0)
    {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.height;
    }
    if (currentBox.topNeighCount == 0)
    {
        weightedSum += currentBox.width * currentBox.temperature;
        distanceSum += currentBox.width;
    }
    if (currentBox.bottomNeighCount == 0)
    {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.width;
    }

    for(j=0; j<currentBox.leftNeigh.size(); j++)
    {
        gridBox neighborBox = getBoxById(currentBox.leftNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for(j=0; j<currentBox.rightNeigh.size(); j++) {
        gridBox neighborBox = getBoxById(currentBox.rightNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for(j=0; j<currentBox.topNeigh.size(); j++)
    {
        gridBox neighborBox = getBoxById(currentBox.topNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for(j=0; j<currentBox.bottomNeigh.size(); j++)
    {
        gridBox neighborBox = getBoxById(currentBox.bottomNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    double weightedTemperature = weightedSum / distanceSum;
    if (weightedTemperature > currentBox.temperature)
    {
        gridBoxes[i].newTemperature = currentBox.temperature + DELTA * (weightedTemperature - currentBox.temperature);
    }
    else
    {
        gridBoxes[i].newTemperature = currentBox.temperature - DELTA * (currentBox.temperature - weightedTemperature);
    }
    // printf("%d ended Iteration, old %f, new %f \n", currentBox.boxId, gridBoxes[i].temperature, gridBoxes[i].newTemperature);
    // bring current temperature closer to the weighted average by 10%

    //return gridBoxes;
}


int main(int argc, char const *argv[])
{
    gridBoxes = parseInput();

//    printGridBoxes(gridBoxes);
    clock_t t1 = clock();

    int iterationCount = runSimulation();
    clock_t t2 = clock();

    printf("number of iterations : %d\n", iterationCount);
    double change = diffTime(t1, t2);
    printf("time taken : %f seconds.\n", change);
    return 0;
}