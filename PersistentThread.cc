//
// Created by paranjay on 10/30/15.
//

#include "HelperMethods.h"

//
// Created by paranjay on 10/27/15.
//

#include <iostream>
#include <vector>
#include <ctime>
#include <stdlib.h>
#define DELTA 0.005

void *updateTemperatureOfBlock(void * parameterPack);

void updateTemperatureOfBox(int i);

using namespace std;

struct ParamterPack
{
    int start;
    int end;
};

vector<gridBox> gridBoxes;
pthread_barrier_t iterationEndBarrier, convergenceCheckBarrier, preUpdateBarrier;
bool loop = true;
int globalIterationCount = 0;

int runSimulation(int boxCount, int threadCount)
{

    int boundary = boxCount / threadCount;
    void* status;
    int rc;
    pthread_t threads[threadCount];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    pthread_barrier_init(&iterationEndBarrier, NULL, threadCount);
    pthread_barrier_init(&convergenceCheckBarrier, NULL, threadCount);
    pthread_barrier_init(&preUpdateBarrier, NULL, threadCount);

    for(int j=0; j<threadCount; j++)
    {
        ParamterPack* pack = (ParamterPack*)malloc(sizeof(ParamterPack));
        (*pack).start = j * boundary;
        (*pack).end = (*pack).start + boundary;
        pthread_create(&threads[j], &attr, updateTemperatureOfBlock, (void *)pack);
    }

    for(int j=0; j<threadCount; j++)
    {
        rc = pthread_join(threads[j], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

//    return iterationCount;
    return globalIterationCount;
}

void *updateTemperatureOfBlock(void* parameterPack) {
    ParamterPack *pack = static_cast<ParamterPack *>(parameterPack);
    int start = pack->start;
    int end = pack->end;


    int iterationCount = 0;
    while (loop)
    {
        iterationCount++;

        for (int i = start; i < end; i++)
        {
            updateTemperatureOfBox(i);
        }

        pthread_barrier_wait(&preUpdateBarrier);

        for (int i = start; i < end; i++)
        {
            gridBoxes[i].temperature = gridBoxes[i].newTemperature;
        }

        pthread_barrier_wait(&convergenceCheckBarrier);

        double max_temp = gridBoxes[0].newTemperature;
        double min_temp = 2000.0;
        if (start == 0) {
            for (int i = 0; i < gridBoxes.size(); i++) {
                if (gridBoxes[i].temperature > max_temp)
                    max_temp = gridBoxes[i].temperature;
                if (gridBoxes[i].temperature < min_temp)
                    min_temp = gridBoxes[i].temperature;
            }

            double conv = (max_temp - min_temp) / max_temp;
            if (conv < EPSILON) {
                printf("Max Temp : %f \nMin Temp : %f \n", max_temp, min_temp);
                loop = false;
                globalIterationCount = iterationCount;
            }
        }
        pthread_barrier_wait(&iterationEndBarrier);

    }
}

void updateTemperatureOfBox(int i) {
    gridBox currentBox = gridBoxes[i];
    int distanceSum = 0;
    double weightedSum = 0.0;
    int distance = 0;
    int j = 0;

    if (currentBox.leftNeighCount == 0) {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.height;
    }
    if (currentBox.rightNeighCount == 0) {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.height;
    }
    if (currentBox.topNeighCount == 0) {
        weightedSum += currentBox.width * currentBox.temperature;
        distanceSum += currentBox.width;
    }
    if (currentBox.bottomNeighCount == 0) {
        weightedSum += currentBox.height * currentBox.temperature;
        distanceSum += currentBox.width;
    }

    for (j = 0; j < currentBox.leftNeigh.size(); j++) {
        gridBox neighborBox = getBoxById(currentBox.leftNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for (j = 0; j < currentBox.rightNeigh.size(); j++) {
        gridBox neighborBox = getBoxById(currentBox.rightNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for (j = 0; j < currentBox.topNeigh.size(); j++) {
        gridBox neighborBox = getBoxById(currentBox.topNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    for (j = 0; j < currentBox.bottomNeigh.size(); j++) {
        gridBox neighborBox = getBoxById(currentBox.bottomNeigh[j], gridBoxes);
        distance = getContactDistance(currentBox, neighborBox);
        weightedSum += distance * neighborBox.temperature;
        distanceSum += distance;
    }
    double weightedTemperature = weightedSum / distanceSum;
    if (weightedTemperature > currentBox.temperature) {
        gridBoxes[i].newTemperature =
                currentBox.temperature + DELTA * (weightedTemperature - currentBox.temperature);
    }
    else {
        gridBoxes[i].newTemperature =
                currentBox.temperature - DELTA * (currentBox.temperature - weightedTemperature);
    }
}

int main(int argc, char const *argv[])
{
    gridBoxes = parseInput();

    int boxCount = gridBoxes.size();
    int threadCount;

    do
    {
        cout << "Enter the number of threads: ";
        cin >> threadCount;

        if(boxCount % threadCount != 0)
        {
            cout << "The thread count should divide the number of boxes evenly, please choose again (box size is "
            << boxCount << "): ";
            cin >> threadCount;
        }

    }while(boxCount % threadCount != 0);

    struct timeval start, end;
    gettimeofday(&start, NULL);
    clock_t start_t = clock();

    int iterationCount = runSimulation(boxCount, threadCount);

    gettimeofday(&end, NULL);
    clock_t end_t = clock();

    double time = diffTime(start, end);
    double avgClocks = averageCPUClocks(start_t, end_t, threadCount);

    printf("number of iterations : %d\n", iterationCount);
    printf("time taken : %f seconds.\n", time);
    printf("average clock cycles per thread : %f .\n", avgClocks);
    return 0;
}
