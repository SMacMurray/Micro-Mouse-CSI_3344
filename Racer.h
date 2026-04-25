/*
 * Racer.h
 *
 *  Created on: Nov 12, 2025
 *      Author: bill_booth
 */

#ifndef RACER_H_
#define RACER_H_

#include "SDL_Plotter.h"
#include "constants.h"

using namespace std;

enum DIRECTION {NORTH, SOUTH, EAST, WEST};


class Racer{

private:
	point location, prevLocation;
	color color;
	int   size;
	bool  alive;
	bool  finished;
	SDL_Plotter* screen;

public:
	Racer(SDL_Plotter* g);
	Racer(const Racer&);
	Racer operator=(const Racer&);

	bool   racing(){return alive;}
	void   move(int direction);
	void   draw(SDL_Plotter& g);
	void   erase(SDL_Plotter& g);
	bool   look(DIRECTION direction);

	point  getLocation(){return location;}
	point  getPrevLocation(){return prevLocation;}

	bool Finished(){return finished;}
	void die(){alive = false;}

	void  setFinished(bool f){finished = f;}
	void  setLocation(point p){location = p;}
	void  setPrevLocation(point p){prevLocation = p;}
	void  setAlive(bool f){alive = f;}


	virtual void run(){
		//do nothing
	}

	virtual void run(SDL_Plotter& g){
		//do nothing
	}

};

inline Racer::Racer(SDL_Plotter* g): location(point(0,0)),
		                      prevLocation(point(0,0)),
							  size(block-padding),
							  alive{true},
							  finished{false},
							  screen{g}{}

inline void Racer::move(int direction){
	prevLocation = location;
	switch(direction){
		case EAST: location.x++; break;
		case NORTH: location.y--; break;
		case WEST: location.x--; break;
		case SOUTH: location.y++; break;
	}
	if(location.x == finishCol and location.y == finishRow){
		finished = true;
	}
}


inline void Racer::draw(SDL_Plotter& g){
	erase(g);
	int startX = location.x * block + buffer + padding/2;
	int startY = location.y * block + buffer + padding/2;

	for(int x = 0; x < size; x++){
		for(int y = 0; y < size; y++){
			g.plotPixel(startX + x, startY + y,0,0,255);
		}
	}

}


inline bool Racer::look(DIRECTION direction){
	int startX = location.x * block + buffer + padding/2;
	int startY = location.y * block + buffer + padding/2;
    Uint32 see;

    switch(direction){
		case NORTH: startY -= padding/2+1; break;
		case SOUTH: startY += size + padding/2+1; break;
		case EAST:  startX += size + (padding)/2; break;
		case WEST:  startX -= (padding)/2; break;
    }

	see = screen->getColor(startX , startY);
	return !see;
}

inline void Racer::erase(SDL_Plotter& g){

	int startX = prevLocation.x * block + buffer + padding/2;
	int startY = prevLocation.y * block + buffer + padding/2;

	for(int x = 0; x < size; x++){
		for(int y = 0; y < size; y++){
			g.plotPixel(startX + x, startY + y,255,255,255);
		}
	}


}


#endif /* RACER_H_ */
