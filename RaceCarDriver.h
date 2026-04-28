/*
* RaceCarDriver.h
 *
 *  Created on: Spring, 2026
 *      Author: bill_booth
 */

#ifndef RACECARDRIVER_H_
#define RACECARDRIVER_H_

#include "Racer.h"
#include <vector>
using namespace std;

class RaceCarDriver{
private:
    Racer* car;

public:
    RaceCarDriver(Racer* p = nullptr): car{p}{}

    DIRECTION nextMove(){
        #include <deque>
        
        static deque<deque<point>> adjMatrix;
        static int lowestX = 0, lowestY = 0; // Subtract the point's x/y to get to the deque[0][0]
        // adjMatrix[0 - lowestX][0 - lowestY];
        
        vector<DIRECTION> pool;

        if(!car->look(EAST))   pool.push_back(EAST);
        if(!car->look(SOUTH))  pool.push_back(SOUTH);
        if(!car->look(NORTH))  pool.push_back(NORTH);
        if(!car->look(WEST))   pool.push_back(WEST);

        return pool[rand() % pool.size()];
    }

};


#endif /* RACECARDRIVER_H_ */
