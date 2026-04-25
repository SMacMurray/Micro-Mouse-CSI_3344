/*
* DisJointSet.h
 *
 *  Created on: Apr 6, 2017
 *      Author: booth
 */

#ifndef DISJOINTSET_H_
#define DISJOINTSET_H_

#include <vector>

using namespace std;

//Disjoint Set

class disjointSet{

private:
    vector<int> pset;
public:

    disjointSet(int MAX = 100){
        pset.assign(MAX,0);
    }

    void initSet(int N ){

        for(size_t i = 0; i < N; i++)
            pset[i] = i;
    }

    int findSet(int i){
        if (pset[i] == i)
            return i;
        else
            return (pset[i]=findSet(pset[i]));
    }

    bool isSameSet(int i, int j){
        return findSet(i) == findSet(j);
    }

    void unionSet(int i, int j){
        pset[findSet(i)] = findSet(j);
    }

    int countSets(int N){
        int count = 0;

        for(size_t i = 0; i < N; i++){
            if(pset[i] == i) count++;
        }

        return count;
    }

};




#endif /* DISJOINTSET_H_ */