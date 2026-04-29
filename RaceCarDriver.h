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
        // Keep maze knowledge between calls and between attempts.
        static vector<vector<bool>> visited(maxRow, vector<bool>(maxCol, false));
        static vector<point> dfsPath;
        static point lastLocation(-1, -1);

        point current = car->getLocation();

        // A new attempt restarts at the beginning, so rebuild the DFS stack there.
        if(current.x == 0 && current.y == 0 &&
           !(lastLocation.x == 0 && lastLocation.y == 0)){
            dfsPath.clear();
        }

        if(dfsPath.empty() ||
           dfsPath.back().x != current.x ||
           dfsPath.back().y != current.y){
            dfsPath.push_back(current);
        }

        if(current.x != lastLocation.x || current.y != lastLocation.y){
            visited[current.y][current.x] = true;
            lastLocation = current;
        }

        vector<DIRECTION> safeMoves;
        if(!car->look(EAST))  safeMoves.push_back(EAST);
        if(!car->look(SOUTH)) safeMoves.push_back(SOUTH);
        if(!car->look(NORTH)) safeMoves.push_back(NORTH);
        if(!car->look(WEST))  safeMoves.push_back(WEST);

        // DFS: always try an unvisited safe neighbor first.
        for(DIRECTION move : safeMoves){
            point next = current;

            if(move == EAST)  next.x++;
            if(move == SOUTH) next.y++;
            if(move == NORTH) next.y--;
            if(move == WEST)  next.x--;

            if(!visited[next.y][next.x]){
                dfsPath.push_back(next);
                return move;
            }
        }

        // If there is no new cell to explore, backtrack along the DFS path.
        if(dfsPath.size() >= 2){
            point parent = dfsPath[dfsPath.size() - 2];

            if(parent.x == current.x + 1 && !car->look(EAST)){
                dfsPath.pop_back();
                return EAST;
            }
            if(parent.y == current.y + 1 && !car->look(SOUTH)){
                dfsPath.pop_back();
                return SOUTH;
            }
            if(parent.y == current.y - 1 && !car->look(NORTH)){
                dfsPath.pop_back();
                return NORTH;
            }
            if(parent.x == current.x - 1 && !car->look(WEST)){
                dfsPath.pop_back();
                return WEST;
            }
        }

        // return any legal move to avoid undefined behavior.
        return safeMoves[0];
    }

};


#endif /* RACECARDRIVER_H_ */
