/*
* RaceCarDriver.h
 *
 *  Created on: Spring, 2026
 *      Author: bill_booth
 */

#ifndef RACECARDRIVER_H_
#define RACECARDRIVER_H_

#include "Racer.h"
#include <map>
#include <queue>
#include <vector>
using namespace std;

class RaceCarDriver{
private:
    Racer* car;

public:
    RaceCarDriver(Racer* p = nullptr): car{p}{}

    DIRECTION nextMove(){
        // Remember which spots we have already been to.
        static vector<vector<bool>> visited(2 * row + 1,
                                            vector<bool>(2 * col + 1, false));
        // Keep the current path so we can backtrack later.
        static vector<point> pathStack;
        // Save the first spot as our reference point.
        static point origin;
        static bool originSet = false;
        // Remember the last relative position we were at.
        static point lastRelative(-1, -1);
        // Extra pathfinding data kept for later use. (BFS)
        static queue<point> q;
        static map<int, int> parent;
        static vector<DIRECTION> shortestPath;
        // Track which attempt we are on.
        static int runNumber = 1;
        // Track whether the car has actually left the start on this run.
        static bool leftOriginThisRun = false;

        // Shift relative coordinates into valid array indexes.
        const int yOffset = row;
        const int xOffset = col;




        // For Run 2
        static point end;
        static map<point,vector<bool>> graph;
        static map<point,point> backLinks; 
        static int run2phase = 0;

        // Run plan:
        // Run 1: use DFS and try to find the finish once.
        // Run 2: keep exploring unknown spots, then build the BFS path.
        // Run 3: follow the saved shortest path if it is ready.

        if (runNumber == 1) {
            // Get the car's current location.
            point current = car->getLocation();
            // Get the previous location from the last move.
            point previous = car->getPrevLocation();
            // Set the starting location once.
            if(!originSet){
                origin = current;
                originSet = true;
                backLinks[origin] = origin;
            }
            // Convert the current location into coordinates relative to the start.
            point relative(current.x - origin.x, current.y - origin.y);
            // Convert the previous location into coordinates relative to the start.
            point previousRelative(previous.x - origin.x, previous.y - origin.y);

            // Count a new run only when the main program resets the car to start.
            // Crossing over the start during DFS should not increment the run.
            if(relative.x == 0 && relative.y == 0 &&
               previousRelative.x == 0 && previousRelative.y == 0){
                // END OF RUN 1
                if(leftOriginThisRun){
                    runNumber++;
                    end = pathStack.back();
                    pathStack.clear();
                    leftOriginThisRun = false;
                    visited = vector<vector<bool>>(2 * row + 1, vector<bool>(2 * col + 1, false));
                }
               }
            else{
                leftOriginThisRun = true;
            }

            // Add this spot to the current path if it is not already on top.
            if(pathStack.empty() ||
               pathStack.back().x != relative.x ||
               pathStack.back().y != relative.y){
                pathStack.push_back(relative);
               }

            // Mark this spot as visited when we enter it.
            if(relative.x != lastRelative.x || relative.y != lastRelative.y){
                if(relative.y + yOffset >= 0 && relative.y + yOffset < (int)visited.size() &&
                   relative.x + xOffset >= 0 && relative.x + xOffset < (int)visited[0].size()){
                    visited[relative.y + yOffset][relative.x + xOffset] = true;
                   }
                lastRelative = relative;
            }

            // Collect all moves that are not blocked by a wall.
            vector<DIRECTION> safeMoves;
            if(!car->look(EAST))  safeMoves.push_back(EAST);
            if(!car->look(SOUTH)) safeMoves.push_back(SOUTH);
            if(!car->look(NORTH)) safeMoves.push_back(NORTH);
            if(!car->look(WEST))  safeMoves.push_back(WEST);

            // Try the first safe move that leads to a new spot.
            for(DIRECTION move : safeMoves){
                point next = relative;

                if(move == EAST)  next.x++;
                if(move == SOUTH) next.y++;
                if(move == NORTH) next.y--;
                if(move == WEST)  next.x--;

                if(next.y + yOffset >= 0 && next.y + yOffset < (int)visited.size() &&
                   next.x + xOffset >= 0 && next.x + xOffset < (int)visited[0].size() &&
                   !visited[next.y + yOffset][next.x + xOffset]){
                    return move;
                   }
            }

            // If all safe spots were visited, go back along the path.
            if(pathStack.size() >= 2){
                point parent = pathStack[pathStack.size() - 2];

                if(parent.x == relative.x + 1 && !car->look(EAST)){
                    pathStack.pop_back();
                    return EAST;
                }
                if(parent.y == relative.y + 1 && !car->look(SOUTH)){
                    pathStack.pop_back();
                    return SOUTH;
                }
                if(parent.y == relative.y - 1 && !car->look(NORTH)){
                    pathStack.pop_back();
                    return NORTH;
                }
                if(parent.x == relative.x - 1 && !car->look(WEST)){
                    pathStack.pop_back();
                    return WEST;
                }
            }

            // If needed, make any legal move as a fallback.
            return safeMoves[0];
        }
        if (runNumber == 2) {
            // Explore the rest of the maze.
            cout << "IRAN" << endl;
            if (run2phase == 0){
                // Get the car's current location.
                point current = car->getLocation();
                // Get the previous location from the last move.
                point previous = car->getPrevLocation();

                // Convert the current location into coordinates relative to the start.
                point relative(current.x - origin.x, current.y - origin.y);
                // Convert the previous location into coordinates relative to the start.
                point previousRelative(previous.x - origin.x, previous.y - origin.y);

                // Count a new run only when the main program resets the car to start.
                // Crossing over the start during DFS should not increment the run.
                if(relative.x == 0 && relative.y == 0 &&
                previousRelative.x == 0 && previousRelative.y == 0){
                    if(leftOriginThisRun){
                        runNumber++;
                        pathStack.clear();
                        leftOriginThisRun = false;
                    }
                }
                else{
                    leftOriginThisRun = true;
                }

                // Add this spot to the current path if it is not already on top.
                if(pathStack.empty() ||
                pathStack.back().x != relative.x ||
                pathStack.back().y != relative.y){
                    pathStack.push_back(relative);
                }

                // Mark this spot as visited when we enter it.
                if(relative.x != lastRelative.x || relative.y != lastRelative.y){
                    if(relative.y + yOffset >= 0 && relative.y + yOffset < (int)visited.size() &&
                    relative.x + xOffset >= 0 && relative.x + xOffset < (int)visited[0].size()){
                        visited[relative.y + yOffset][relative.x + xOffset] = true;
                    }
                    lastRelative = relative;
                }

                // Initialize every point as being walled off in all directions.
                if (graph.find(relative) == graph.end()) graph[relative] = vector<bool>(4, true);

                // Collect all moves that are not blocked by a wall.
                // Also store which directions each cell can go for BFS and set backlinks.
                vector<DIRECTION> safeMoves;
                point tmp = relative;

                if(!car->look(EAST)){
                    safeMoves.push_back(EAST);
                    graph[relative][EAST] = false;
                }
                if(!car->look(SOUTH)){
                    safeMoves.push_back(SOUTH);
                    graph[relative][SOUTH] = false;
                }
                if(!car->look(WEST)){
                    safeMoves.push_back(WEST);
                    graph[relative][WEST] = false;
                }
                if(!car->look(NORTH)){
                    safeMoves.push_back(NORTH);
                    graph[relative][NORTH] = false;
                }

                // Try the first safe move that leads to a new spot.
                for(DIRECTION move : safeMoves){
                    point next = relative;

                    if(move == EAST)  next.x++;
                    if(move == SOUTH) next.y++;
                    if(move == NORTH) next.y--;
                    if(move == WEST)  next.x--;

                    if(next.y + yOffset >= 0 && next.y + yOffset < (int)visited.size() &&
                    next.x + xOffset >= 0 && next.x + xOffset < (int)visited[0].size() &&
                    !visited[next.y + yOffset][next.x + xOffset]){
                        return move;
                    }
                }

                // If all safe spots were visited, go back along the path.
                if(pathStack.size() >= 2){
                    point parent = pathStack[pathStack.size() - 2];

                    if(parent.x == relative.x + 1 && !car->look(EAST)){
                        pathStack.pop_back();
                        return EAST;
                    }
                    if(parent.y == relative.y + 1 && !car->look(SOUTH)){
                        pathStack.pop_back();
                        return SOUTH;
                    }
                    if(parent.y == relative.y - 1 && !car->look(NORTH)){
                        pathStack.pop_back();
                        return NORTH;
                    }
                    if(parent.x == relative.x - 1 && !car->look(WEST)){
                        pathStack.pop_back();
                        return WEST;
                    }
                }

                // If needed, make any legal move as a fallback.
                return safeMoves[0];

                // If back at start
                // BFS from the start
            }
            
        }
        if (runNumber == 3) {
            // Run the fastest path
        }
    }

};


#endif /* RACECARDRIVER_H_ */
