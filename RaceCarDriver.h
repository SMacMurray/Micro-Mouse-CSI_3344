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
#include <queue>
#include <set>
#include <algorithm>
#include <thread>
#include <chrono>
using namespace std;


class RaceCarDriver{
private:
    Racer* car;
    DIRECTION facing = EAST;
    int runCount = 1;
    /* If the car will always be reset at 0,0 */
    point lastLocation;
    vector<vector<int> > graph;
    vector<int> bfsRoute;
    vector<int> dfsPath;
    set<int> scanned;
    set<int> dfsVisited;

    int cellId(point p) {
        return p.y * col + p.x;
    }

    bool inBounds(int cell) {
        return cell >= 0 && cell < row * col;
    }

    bool validNeighbor(int cell, DIRECTION direction) {
        switch (direction) {
            case NORTH: return cell >= col;
            case SOUTH: return cell + col < row * col;
            case EAST:  return cell % col < col - 1;
            case WEST:  return cell % col > 0;
        }
    }

    int neighborCell(int cell, DIRECTION direction) {
        switch (direction) {
            case NORTH: return cell - col;
            case SOUTH: return cell + col;
            case EAST:  return cell + 1;
            case WEST:  return cell - 1;
        }
    }

    DIRECTION directionTo(int from, int to) {
        if (to == from - col) return NORTH;
        if (to == from + col) return SOUTH;
        if (to == from + 1) return EAST;
        return WEST;
    }

    void resetSearch() {
        graph.assign(row * col, vector<int>());
        bfsRoute.clear();
        dfsPath.clear();
        scanned.clear();
        dfsVisited.clear();
    }

    void addPassage(int from, int to) {
        if (!inBounds(from) || !inBounds(to)) {
            return;
        }
        if (find(graph[from].begin(), graph[from].end(), to) == graph[from].end()) {
            graph[from].push_back(to);
        }
        if (find(graph[to].begin(), graph[to].end(), from) == graph[to].end()) {
            graph[to].push_back(from);
        }
    }

    void scanCurrentCell() {
        int current = cellId(car->getLocation());
        if (!inBounds(current) || scanned.find(current) != scanned.end()) {
            return;
        }

        vector<DIRECTION> directions = {EAST, SOUTH, NORTH, WEST};
        for (DIRECTION direction: directions) {
            int next = neighborCell(current, direction);
            if (validNeighbor(current, direction) && inBounds(next) && !car->look(direction)) {
                addPassage(current, next);
            }
        }

        scanned.insert(current);
    }

    vector<int> bfsPath(int start, bool toFinish) {
        int finish = finishRow * col + finishCol;
        vector<int> parent(row * col, -1);
        vector<bool> visited(row * col, false);
        queue<int> cells;

        visited[start] = true;
        cells.push(start);

        int target = -1;
        while (!cells.empty()) {
            int current = cells.front();
            cells.pop();

            if ((toFinish && current == finish) ||
                (!toFinish && scanned.find(current) == scanned.end())) {
                target = current;
                break;
            }

            for (int next: graph[current]) {
                if (!visited[next]) {
                    visited[next] = true;
                    parent[next] = current;
                    cells.push(next);
                }
            }
        }

        vector<int> path;
        if (target == -1) {
            return path;
        }

        for (int current = target; current != -1; current = parent[current]) {
            path.push_back(current);
        }
        reverse(path.begin(), path.end());
        return path;
    }

    DIRECTION wallFollowMove() {
        vector<DIRECTION> pool;
        DIRECTION left = leftOf(facing);
        DIRECTION right = rightOf(facing);
        DIRECTION back = backOf(facing);

        if (!car->look(left)) {
            pool.push_back(left);
        }
        else if (!car->look(facing)) {
            pool.push_back(facing);
        }
        else if (!car->look(right)) {
            pool.push_back(right);
        }
        else if (!car->look(back)) {
            pool.push_back(back);
        }

        if(pool.empty()){
            car->die();
            return EAST;
        }

        facing = pool[0];
        return pool[0];
    }

    DIRECTION bfsMove() {
        scanCurrentCell();

        int current = cellId(car->getLocation());
        if (bfsRoute.size() < 2 || bfsRoute[0] != current) {
            bfsRoute = bfsPath(current, true);
            if (bfsRoute.empty()) {
                bfsRoute = bfsPath(current, false);
            }
        }

        if (bfsRoute.size() < 2) {
            return wallFollowMove();
        }

        DIRECTION move = directionTo(bfsRoute[0], bfsRoute[1]);
        bfsRoute.erase(bfsRoute.begin());
        facing = move;
        return move;
    }

    DIRECTION dfsMove() {
        scanCurrentCell();

        int current = cellId(car->getLocation());
        if (dfsPath.empty() || dfsPath.back() != current) {
            dfsPath.clear();
            dfsPath.push_back(current);
            dfsVisited.insert(current);
        }

        for (int next: graph[current]) {
            if (dfsVisited.find(next) == dfsVisited.end()) {
                dfsVisited.insert(next);
                dfsPath.push_back(next);
                DIRECTION move = directionTo(current, next);
                facing = move;
                return move;
            }
        }

        if (dfsPath.size() > 1) {
            dfsPath.pop_back();
            DIRECTION move = directionTo(current, dfsPath.back());
            facing = move;
            return move;
        }

        return wallFollowMove();
    }

    DIRECTION leftOf(DIRECTION facing) {
        switch (facing) {
            case NORTH: return WEST;
            case SOUTH: return EAST;
            case EAST:  return NORTH;
            case WEST:  return SOUTH;
        }
    }

    DIRECTION rightOf(DIRECTION facing) {
        switch (facing) {
            case NORTH: return EAST;
            case SOUTH: return WEST;
            case EAST:  return SOUTH;
            case WEST:  return NORTH;
        }
    }

    DIRECTION backOf(DIRECTION facing) {
        switch (facing) {
            case NORTH: return SOUTH;
            case SOUTH: return NORTH;
            case EAST:  return WEST;
            case WEST:  return EAST;
        }
    }

public:
    RaceCarDriver(Racer* p = nullptr):
        car{p}, lastLocation{0,0} {
        resetSearch();
    }

    DIRECTION nextMove(){
        // this_thread::sleep_for(chrono::milliseconds(10));
        point currentLocation = car->getLocation();
        point previousLocation = car->getPrevLocation();

        if (currentLocation.x == 0 && currentLocation.y == 0 &&
            previousLocation.x == 0 && previousLocation.y == 0 &&
            !(lastLocation.x == 0 && lastLocation.y == 0)) {
            runCount++;
            cout << runCount << endl;
            resetSearch();
        }

        lastLocation = currentLocation;

        if (runCount == 2) {
            return bfsMove();
        }
        if (runCount == 3) {
            return dfsMove();
        }

        return wallFollowMove();
    }

};


#endif /* RACECARDRIVER_H_ */
