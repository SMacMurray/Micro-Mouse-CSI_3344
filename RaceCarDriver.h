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
        vector<DIRECTION> pool;

        if(!car->look(EAST))   pool.push_back(EAST);
        if(!car->look(SOUTH))  pool.push_back(SOUTH);
        if(!car->look(NORTH))  pool.push_back(NORTH);
        if(!car->look(WEST))   pool.push_back(WEST);

        return pool[rand() % pool.size()];
    }

};


#endif /* RACECARDRIVER_H_ */