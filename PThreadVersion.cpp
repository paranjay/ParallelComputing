//
// Created by paranjay on 10/27/15.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdexcept>
#include <ctime>
#include <time.h>

#define EPSILON 0.00000000001
#define DELTA 0.005

using namespace std;

struct gridBox
{
    double temperature;
    double newTemperature;
    int boxId;
    int upperLeftX;
    int upperLeftY;
    int upperRightX;
    int upperRightY;
    int downLeftX;
    int downLeftY;
    int downRightX;
    int downRightY;

    int height;
    int width;
    int leftNeighCount;
    int rightNeighCount;
    int topNeighCount;
    int bottomNeighCount;
    vector<int> leftNeigh;
    vector<int> rightNeigh;
    vector<int> topNeigh;
    vector<int> bottomNeigh;

};

vector<gridBox> parseInput()
{
    string line, firstLine;
//    filename = "data/testgrid_1";

    std::ifstream inputFile("data/testgrid_1");
//    std::ifstream inputFile("/home/paranjay/ClionProjects/SerialAMR/data/testgrid_1");

    int boxCount, gridRowCount, gridColumnCount;
    getline(inputFile, firstLine);
    istringstream firstStream(firstLine);
    firstStream >> boxCount >> gridRowCount >> gridColumnCount;
    vector<gridBox> gridBoxes;

    while (getline(inputFile, line))
    {
        gridBox currentBox = gridBox();
        istringstream lineStream1(line);
        lineStream1 >> currentBox.boxId;
        if(currentBox.boxId == -1)
            break;

        getline(inputFile, line);
        istringstream lineStream2(line);
        lineStream2 >> currentBox.upperLeftY >> currentBox.upperLeftX >> currentBox.height >> currentBox.width;
        currentBox.upperRightX = currentBox.upperLeftX + currentBox.width;
        currentBox.upperRightY = currentBox.upperLeftY;
        currentBox.downLeftX = currentBox.upperLeftX;
        currentBox.downLeftY = currentBox.upperLeftY + currentBox.height;
        currentBox.downRightX = currentBox.upperLeftX + currentBox.width;
        currentBox.downRightY = currentBox.upperLeftY + currentBox.height;

        getline(inputFile, line);
        istringstream lineStream3(line);
        lineStream3 >> currentBox.topNeighCount;
        for (int i=0; i<currentBox.topNeighCount; i++)
        {
            int neighbor;
            lineStream3 >> neighbor;
            currentBox.topNeigh.push_back(neighbor);
        }

        getline(inputFile, line);
        istringstream lineStream4(line);
        lineStream4 >> currentBox.bottomNeighCount;
        for (int i=0; i<currentBox.bottomNeighCount; i++)
        {
            int neighbor;
            lineStream4 >> neighbor;
            currentBox.bottomNeigh.push_back(neighbor);
        }

        getline(inputFile, line);
        istringstream lineStream5(line);
        lineStream5 >> currentBox.leftNeighCount;
        for (int i=0; i<currentBox.leftNeighCount; i++)
        {
            int neighbor;
            lineStream5 >> neighbor;
            currentBox.leftNeigh.push_back(neighbor);
        }

        getline(inputFile, line);
        istringstream lineStream6(line);
        lineStream6 >> currentBox.rightNeighCount;

        for (int i=0; i<currentBox.rightNeighCount; i++)
        {
            int neighbor;
            lineStream6 >> neighbor;
            currentBox.rightNeigh.push_back(neighbor);
        }

        getline(inputFile, line);
        istringstream lineStream7(line);
        lineStream7 >> currentBox.temperature;

        gridBoxes.push_back(currentBox);

    }

    return gridBoxes;
}

gridBox getBoxById(int id, vector<gridBox>& gridBoxes)
{
    for(int j=0; j < gridBoxes.size(); j++)
    {
        if(gridBoxes[j].boxId == id)
        {
            return gridBoxes[j];
        }
    }
    throw std::invalid_argument( "Nothing matched" );
}

int findDistanceOnYAxis(gridBox box1, gridBox box2)
{
    int distance1 = fabs(box1.downLeftY - box2.upperRightY);
    int distance2 = fabs(box1.downLeftY - box1.upperLeftY);
    int distance3 = fabs(box2.downLeftY - box2.upperLeftY);
    int distance4 = fabs(box2.downLeftY - box1.upperRightY);
    return min(min(min(distance1, distance2), distance3), distance4);
}

int findDistanceOnXAxis(gridBox box1, gridBox box2)
{
    int distance1 = fabs(box1.upperLeftX - box2.upperRightX);
    int distance2 = fabs(box1.upperLeftX - box1.upperRightX);
    int distance3 = fabs(box2.upperLeftX - box2.upperRightX);
    int distance4 = fabs(box2.downLeftX - box1.upperRightX);
    return min(min(min(distance1, distance2), distance3), distance4);
}

int getContactDistance(gridBox box1, gridBox box2)
{
    if(box1.upperLeftX == box1.downLeftX &&
       box1.downLeftX == box2.upperRightX &&
       box2.upperRightX == box2.downRightX)
    {
        return findDistanceOnYAxis(box1, box2);
    }
    else if(box2.upperLeftX == box2.downLeftX &&
            box2.downLeftX == box1.upperRightX &&
            box1.upperRightX == box1.downRightX)
    {
        return findDistanceOnYAxis(box1, box2);
    }
    else if (box1.downLeftY == box1.downRightY &&
             box1.downRightY == box2.upperLeftY &&
             box2.upperLeftY == box2.upperRightY)
    {
        return findDistanceOnXAxis(box1, box2);
    }
    else if (box2.downLeftY == box2.downRightY &&
             box2.downRightY == box1.upperLeftY &&
             box1.upperLeftY == box1.upperRightY)
    {
        return findDistanceOnXAxis(box1, box2);
    }
    else
    {
        printf("%d, box 1\n", box1.boxId);
        printf("(%d, %d) \n", box1.upperLeftX, box1.upperLeftY);
        printf("(%d, %d) \n", box1.upperRightX, box1.upperRightY);
        printf("(%d, %d) \n", box1.downRightX, box1.downRightY);
        printf("(%d, %d) \n", box1.downLeftX, box1.downLeftY);

        printf("%d, box 2\n", box2.boxId);
        printf("(%d, %d) \n", box2.upperLeftX, box2.upperLeftY);
        printf("(%d, %d) \n", box2.upperRightX, box2.upperRightY);
        printf("(%d, %d) \n", box2.downRightX, box2.downRightY);
        printf("(%d, %d) \n", box2.downLeftX, box2.downLeftY);

        throw std::invalid_argument( "This should not happen" );
    }
}

bool hasSimulationConverged(vector<gridBox>& gridBoxes)
{
    double maxTemperature = gridBoxes[0].temperature;
    double minTemperature = gridBoxes[0].temperature;
    for(int i=0; i<gridBoxes.size(); i++)
    {
        if(gridBoxes[i].temperature > maxTemperature)
        {
            maxTemperature = gridBoxes[i].temperature;
        }
        if(gridBoxes[i].temperature < minTemperature)
        {
            minTemperature = gridBoxes[i].temperature;
        }
    }

    if((maxTemperature - minTemperature) / maxTemperature < EPSILON)
        return true;
    else
        return false;
}

int runSimulation(vector<gridBox>& gridBoxes)
{
    bool loop = true;
    int iterationCount = 0;
    while(loop)
    {
        iterationCount++;
        // cout << "iteration number " << iterationCount<<endl;
        for(int i=0; i< gridBoxes.size(); i++)
        {
            int distanceSum = 0;
            double weightedSum = 0.0;
            int distance = 0;
            int j=0;
            gridBox currentBox = gridBoxes[i];

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
            for(j=0; j<currentBox.rightNeigh.size(); j++)
            {
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

void printGridBoxes(vector<gridBox>& gridBoxes)
{
    for(int i=0;i <gridBoxes.size(); i++)
    {
        cout << gridBoxes[i].boxId << endl;
    }
}

double diffTime(clock_t t1, clock_t t2)
{
    double elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC;
    return elapsed;
}


int main(int argc, char const *argv[])
{
    vector<gridBox> gridBoxes = parseInput();

//    printGridBoxes(gridBoxes);
    clock_t t1 = clock();

    int iterationCount = runSimulation(gridBoxes);
    clock_t t2 = clock();

    printf("number of iterations : %d\n", iterationCount);
    double change = diffTime(t1, t2);
    printf("time taken : %f seconds.\n", change);
    return 0;
}