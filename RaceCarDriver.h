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
        static vector<vector<bool>> bfsVisited(2 * row + 1,
                                            vector<bool>(2 * col + 1, false));
        static vector<vector<pair<DIRECTION, point>>> bfsData(2 * row + 1,
                                                              vector<pair<DIRECTION, point>>(2 * col + 1));
        
        // Keep the current path so we can backtrack later.
        static vector<point> pathStack;
        // Save the first spot as our reference point.
        static point origin;
        static point exit;
        static bool originSet = false;
        static bool exitSet = false;
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
            }
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
        if (runNumber == 2 && exitSet) {
            // Explore the rest of the maze.
            bfsData[origin.y + yOffset][origin.x + xOffset] = make_pair(static_cast<DIRECTION>(5), origin);
            queue<point> q;
            q.push(origin);
            
            point u;
            // Running BFS
            while (!q.empty()) {
                u = q.front();
                q.pop();
                
                // If the current point is the exit, end the BFS
                if (u.y == exit.y && u.x == exit.x) {
                    q = {};
                }
                // If is not the exit, look for surrounding points, whose Direction back to the start and previous point will be kept.
                else {
                    if (visited[u.y + yOffset][u.x + 1 + xOffset] &&
                        !bfsVisited[u.y + yOffset][u.x + 1 + xOffset]) {
                        point v(u.x + 1, u.y);
                        q.push(v);
                        bfsData[v.y + xOffset][v.x + yOffset].first = WEST;
                        bfsData[v.y + xOffset][v.x + yOffset].second = u;
                        bfsVisited[v.y + xOffset][v.x + yOffset] = true;
                    }
                    if (visited[u.y + 1 + xOffset][u.x + xOffset] &&
                        !bfsVisited[u.y + 1 + xOffset][u.x + xOffset]) {
                        point v = point(u.x, u.y + 1);
                        q.push(v);
                        bfsData[v.y + xOffset][v.x + yOffset].first = NORTH;
                        bfsData[v.y + xOffset][v.x + yOffset].second = u;
                        bfsVisited[v.y + xOffset][v.x + yOffset] = true;
                    }
                    if (visited[u.y + yOffset][u.x - 1 + xOffset] &&
                        !bfsVisited[u.y + yOffset][u.x - 1 + xOffset]) {
                        point v(u.x + 1, u.y);
                        q.push(v);
                        bfsData[v.y + xOffset][v.x + yOffset].first = EAST;
                        bfsData[v.y + xOffset][v.x + yOffset].second = u;
                        bfsVisited[v.y + xOffset][v.x + yOffset] = true;
                    }
                    if (visited[u.y - 1 + xOffset][u.x + xOffset] &&
                        !bfsVisited[u.y - 1 + xOffset][u.x + xOffset]) {
                        point v(u.x, u.y + 1);
                        q.push(v);
                        bfsData[v.y + xOffset][v.x + yOffset].first = SOUTH;
                        bfsData[v.y + xOffset][v.x + yOffset].second = u;
                        bfsVisited[v.y + xOffset][v.x + yOffset] = true;
                    }
                }
                
            }
            
            // Tracing the path back to the exit.
            point iterPoint(exit.x, exit.y);
            while (iterPoint.y != origin.y && iterPoint.x != origin.x) {
                DIRECTION iterDirection = bfsData[iterPoint.y + yOffset][iterPoint.x + xOffset].first;
                
                // To inverse direction.
                switch(iterDirection) {
                    case WEST:
                        shortestPath.push_back(EAST);
                        break;
                    case NORTH:
                        shortestPath.push_back(SOUTH);
                        break;
                    case EAST:
                        shortestPath.push_back(WEST);
                        break;
                    case SOUTH:
                        shortestPath.push_back(NORTH);
                        break;
                    
                }
//                shortestPath.push_back(bfsData[iterPoint.y + yOffset][iterPoint.x + xOffset].first);
                
                // Going back to the predecessor
                iterPoint = bfsData[iterPoint.y + yOffset][iterPoint.x + xOffset].second;
            }
            
            // Ending run
            runNumber++;
            return static_cast<DIRECTION>(5); // Forcing the car to die.
            // If back at start
            // BFS from the start
        }
        else { //if (runNumber == 3) {
            // Run the fastest path
        }
    }

};


#endif /* RACECARDRIVER_H_ */
