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



        // Run 2
        static int phase = 0;
        static map<pair<int,int>, vector<bool>> graph;
        static map<pair<int,int>, point> backLinks;
        static point end;
        static int i = -1; // For iterating through shortestPath without destroying it

        
        // Debugging flags
        static bool run1 = false;
        static bool run2 = false;
        static bool run3 = false;
        static bool phase1 = false;
        static bool phase2 = false;
        static bool phase3 = false;
        


        // Run plan:
        // Run 1: use DFS and try to find the finish once.
        // Run 2: keep exploring unknown spots, then build the BFS path.
        // Run 3: follow the saved shortest path if it is ready.

        if (runNumber == 1) {
            if (!run1){
                cout << "RUN 1 STARTED" << endl;
                run1 = true;
            }

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
                    end = pathStack.back();
                    visited = vector<vector<bool>>(2 * row + 1,
                                            vector<bool>(2 * col + 1, false));
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
        if (runNumber == 2) {
            if (!run2){
                cout << "RUN 2 STARTED" << endl;
                run2 = true;
            }
            // Explore the rest of the maze.
            // PHASE 1
            // WARNING PHASE 1'S DFS IS BROKEN AND DOES NOT VISIT ALL NODES
            // Let Lucas put his working DFS here
            if (phase == 0){
                if (!phase1){
                    cout << "PHASE 1 STARTED" << endl;
                    phase1 = true;
                }

                // Get the car's current location.
                point current = car->getLocation();
                // Get the previous location from the last move.
                point previous = car->getPrevLocation();
                // Convert the current location into coordinates relative to the start.
                point relative(current.x - origin.x, current.y - origin.y);
                // Convert the previous location into coordinates relative to the start.
                point previousRelative(previous.x - origin.x, previous.y - origin.y);

                pair<int,int> relativePair = make_pair(relative.x, relative.y);

                //cout << "AT " << relativePair.first << ", " << relativePair.second << endl;
                // Initialize every cell to have all walls when we find it.
                if (graph.find(relativePair) == graph.end()){
                    graph[relativePair] = vector<bool>(4, true);
                }

                if(!car->look(EAST)){
                    graph.at(relativePair)[EAST] = false;
                }
                if(!car->look(SOUTH)){
                    graph.at(relativePair)[SOUTH] = false;
                }
                if(!car->look(WEST)){
                    graph.at(relativePair)[WEST] = false;
                }
                if(!car->look(NORTH)){
                    graph.at(relativePair)[NORTH] = false;
                }

                if (relative.x == 0 && relative.y == 0 && pathStack.size() == 1){
                    phase++;
                }
                else {
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
                    if(!car->look(EAST)){
                        safeMoves.push_back(EAST);
                        graph.at(relativePair)[EAST] = false;
                    }
                    if(!car->look(SOUTH)){
                        safeMoves.push_back(SOUTH);
                        graph.at(relativePair)[SOUTH] = false;
                    }
                    if(!car->look(WEST)){
                        safeMoves.push_back(WEST);
                        graph.at(relativePair)[WEST] = false;
                    }
                    if(!car->look(NORTH)){
                        safeMoves.push_back(NORTH);
                        graph.at(relativePair)[NORTH] = false;
                    }

                    // Try the first safe move that leads to a new spot.
                    for(DIRECTION move : safeMoves){
                        point next = relative;

                        if(move == EAST)  next.x++;
                        if(move == SOUTH) next.y++;
                        if(move == NORTH) next.y--;
                        if(move == WEST)  next.x--;

                        // DO NOT GO TO THE END
                        if (next.x == end.x && next.y == end.y) continue;

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
                // END OF PHASE 1

                // PHASE 2
                // If back at start
                // BFS from the start
                if (!phase2){
                    cout << "PHASE 2 STARTED" << endl;
                    phase2 = true;
                }

                bool foundEnd = false;
                visited = vector<vector<bool>>(2 * row + 1,
                                            vector<bool>(2 * col + 1, false));
                q.push(point(0,0));
                visited[yOffset][xOffset] = true;
                while (!q.empty() && !foundEnd){
                    current = q.front();
                    q.pop();
                    pair<int,int> currentPair = make_pair(current.x,current.y);
                    //cout << "LOOKING AT " << current.x << ", " << current.y << endl; 
                    for (int i = 0; i < 4; i++){
                        if (graph.find(currentPair) == graph.end()) cout << "THIS NODE SHOULD HAVE BEEN VISITED BY DFS ALREADY (" << currentPair.first << ", " << currentPair.second << ")" << endl;
                        else if (!graph[currentPair][i]){
                            point next = current;
                            if (i == EAST) next.x++;
                            else if (i == SOUTH) next.y++;
                            else if (i == WEST) next.x--;
                            else if (i == NORTH) next.y--;
                            if (!visited[next.y+yOffset][next.x+xOffset]){
                                //cout << "FOUND " << next.x << ", " << next.y << endl;
                                q.push(next);
                                backLinks[make_pair(next.x, next.y)] = current;
                                visited[next.y+yOffset][next.x+xOffset] = true;
                                if (next.x == end.x && next.y == end.y){
                                    foundEnd = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!foundEnd) cout << "!!!!!COULD NOT FIND EXIT!!!!!" << endl;

                current = end;
                relativePair = make_pair(current.x, current.y);
                while (backLinks.find(relativePair) != backLinks.end()){
                    point previous = backLinks[relativePair];
                    if (previous.x < current.x) shortestPath.push_back(EAST);
                    else if (previous.y < current.y) shortestPath.push_back(SOUTH);
                    else if (previous.x > current.x) shortestPath.push_back(WEST);
                    else if (previous.y > current.y) shortestPath.push_back(NORTH);
                    current = previous;
                    relativePair = make_pair(current.x, current.y);
                }
                // Path printing for sanity checks
                for (int i = shortestPath.size()-1; i >= 0; i--){
                    cout << shortestPath.size() - i << " ";
                    switch (shortestPath[i]){
                        case EAST: cout << "EAST" << endl; break;
                        case SOUTH: cout << "SOUTH" << endl; break;
                        case WEST: cout << "WEST" << endl; break;
                        case NORTH: cout << "NORTH" << endl; break;
                    }
                }
            }

            // PHASE 3;
            if (!phase3){
                cout << "PHASE 3 STARTED" << endl;
                phase3 = true;
            }

            if (i == -1) i = shortestPath.size() - 1;
            return shortestPath[i--];
        }
        if (runNumber == 3) {
            if (!run3){
                cout << "RUN 3 STARTED" << endl;
                run3 = true;
            }
            // Run the fastest path
            if (i == -1) i = shortestPath.size() - 1;
            return shortestPath[i--];
        }

        // Dummy return
        cin >> run2;
        return EAST;
    }    
};


#endif /* RACECARDRIVER_H_ */