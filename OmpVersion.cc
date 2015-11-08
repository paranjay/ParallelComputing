//
// Created by paranjay on 11/5/15.
//

#include <iostream>
#include <vector>
#include <ctime>
#include "HelperMethods.h"
#include <stdlib.h>
#include <omp.h>
#define DELTA 0.005

void updateTemperatureOfBox(int start, int end);

using namespace std;

struct ParamterPack
{
    int start;
    int end;
};

vector<gridBox> gridBoxes;

int runSimulation(int boxCount, int threadCount)
{
    bool loop = true;
    int iterationCount = 0;

    int boundary = boxCount / threadCount;
    while(loop)
    {
        iterationCount++;

        omp_set_num_threads(threadCount);
        int actualThreadCount = 0;
        #pragma omp parallel
        {
            int threadId = omp_get_thread_num();
            actualThreadCount = omp_get_num_threads();
            int boundary = boxCount / actualThreadCount;
            updateTemperatureOfBox(threadId * boundary, (threadId + 1) * boundary);
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

        double conv = (max_temp - min_temp) / max_temp;
//        cout << min_temp << " " << max_temp << endl;
        if(conv < EPSILON)
        {
            printf("Max Temp : %f \nMin Temp : %f \n", max_temp, min_temp);
            printf("number of threads from omp : %d \n", actualThreadCount);
            loop = false;
        }
    }

    return iterationCount;
}

void updateTemperatureOfBox(int start, int end)
{
//    ParamterPack *pack = static_cast<ParamterPack*>(parameterPack);
//    int start = pack->start;
//    int end = pack->end;
//    if(start == 0)
//        cout << "start : " << start << " end : "<< end << endl;

    for(int i = start; i<end; i++)
    {
        int distanceSum = 0;
        double weightedSum = 0.0;
        int distance = 0;
        int j = 0;
        gridBox currentBox = gridBoxes[i];

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
    // printf("%d ended Iteration, old %f, new %f \n", currentBox.boxId, gridBoxes[i].temperature, gridBoxes[i].newTemperature);
    // bring current temperature closer to the weighted average by 10%

    //return gridBoxes;
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
    printf("time taken by wall clock : %f seconds.\n", time);
    printf("average clock cycles per thread : %f .\n", avgClocks);
    return 0;
}
