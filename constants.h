/*
* constants.h
 *
 *  Created on: Nov 12, 2025
 *      Author: bill_booth
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <set>
#include <utility>

class SDL_Plotter;

using std::pair;
using std::set;

struct Color{
    int r,g,b;
};

struct Parameter{
    set<pair<int,int> >* walls;
    SDL_Plotter* g;
};

const int minRow = 2000;
const int maxRow = 2000;
const int minCol = 2000;
const int maxCol = 2000;

extern int row;
extern int col;
extern int finishRow;
extern int finishCol;

const int block   = 22;
const int buffer  = 100;
const int padding = 6;
const int wallThickness = 3; // must be odd






#endif /* CONSTANTS_H_ */
