#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <set>
#include "DisJointSet.h"
#include "SDL_Plotter.h"
#include "Racer.h"
#include "RaceCarDriver.h"
#include "constants.h"
#include "timer.h"
using namespace std;

void drawCircle(point loc, int size, color c, SDL_Plotter& g);
void drawMaze(string fName, SDL_Plotter& g);


void drawBlock(int x, int y, Color, SDL_Plotter&);
void drawWall(int cell1, int cell2, Color, SDL_Plotter&);
void drawFrame(Color, SDL_Plotter& g);
void solveMaze(set<pair<int,int> > walls, SDL_Plotter& g);
bool drawSolution(int start, Color color, set<pair<int,int> >& walls, SDL_Plotter& g);
bool drawSolution2(int start, int current, disjointSet& cells, set<pair<int,int> >& walls,SDL_Plotter& g);
void buildMaze(bool fast, disjointSet& cells,set<pair<int,int> >& walls, SDL_Plotter& g);
void drawMaze(set<pair<int,int> >& walls, SDL_Plotter& g);


void saveMaze(set<pair<int,int> >& walls){

}

void printMaze(set<pair<int,int> >& walls){
	for(auto i: walls){
		cout << i.first << " " << i.second << endl;
	}
}

void saveMaze(set<pair<int,int> >& walls, string fName){
	ofstream file{fName};
	for(auto i: walls){
		file << i.first << " " << i.second << endl;
	}
	file.close();
}

void readMaze(set<pair<int,int> >& walls, string fName){
	int x,y;
	walls.clear();
	ifstream file{fName};
	while(file >> x >> y){
		walls.insert(make_pair(x,y));
	}
	file.close();
}

bool legalMove(Racer& r, const set<pair<int,int> >& walls);
bool legalMove(Racer* r, const set<pair<int,int> >& walls);


int main(int argc, char ** argv)
{

	Timer clock;
	bool doneOnce = false;
	bool nextRun = true;
    point p;
    color c;
    char next;
    int size;
    int runCount = 0;
    Uint32 RGB;
    SDL_Plotter g(row*block+2*buffer,col*block+2*buffer);
    Racer robot(&g);
    RaceCarDriver driver(&robot);


	disjointSet cells(row*col);
	set<pair<int,int> > walls;
	cells.initSet(row*col);

	Color color = {255,0,0};
	color.r = color.g = color.b = 0;

	srand(time(0));

	//buildMaze(false, cells, walls, g);
    //saveMaze(walls, "wallMaze3.txt");

    readMaze(walls, "wallMaze1.txt");
    //readMaze(walls, "wallMaze2.txt");
    //readMaze(walls, "wallMaze3.txt");

	drawMaze(walls,g);
	//drawMaze("maze5.txt",g);

	robot.draw(g);

	g.update();

int i = 1;
clock.reset();
    while (!g.getQuit()  && runCount < 3)
    {

		if(robot.racing() and !robot.Finished()){

			if(g.kbhit()){
				switch(toupper(g.getKey())){

					case 'C': g.clear();           break;
					case 'S': solveMaze(walls, g); break;
					case 'R': saveMaze(walls);     break;
					case 'P': printMaze(walls);    break;
					case RIGHT_ARROW: robot.move(EAST); break;
					case LEFT_ARROW:  robot.move(WEST); break;
					case UP_ARROW:    robot.move(NORTH); break;
					case DOWN_ARROW:  robot.move(SOUTH); break;

				}
				robot.move(driver.nextMove());
				legalMove(robot, walls);
				robot.draw(g);
				g.update();
				//g.Sleep(500);

			}

			if(g.mouseClick()){
			}

			robot.move(driver.nextMove());
			legalMove(robot, walls);
			robot.draw(g);
			g.update();
		}
		else{
			if(robot.Finished()){
			    cout << clock.elapsed() << endl;
			}
			else{
				cout << "DIE!" << endl;
			}
			clock.reset();
			runCount++;
			robot.setAlive(true);
			robot.setLocation(point(0,0));
			robot.setPrevLocation(point(0,0));
			robot.setFinished(false);
			g.clear();
			drawMaze(walls,g);
			robot.draw(g);
			g.update();
		}
    }

    g.Sleep(5000);
}

bool legalMove(Racer& r, const set<pair<int,int> >& walls){
	bool flag;
	point p1 = r.getPrevLocation();
	point p2 = r.getLocation();


	int cell1 = p1.y * col + p1.x;
	int cell2 = p2.y * col + p2.x;

	if(cell1 == cell2) return true;
	if(p1.x < 0 || p1.y < 0 || p2.x < 0 || p2.y < 0) r.die();
	if(p1.x >= col || p1.y >= row || p2.x >= col || p2.y >= row) r.die();

	flag =  (walls.find(make_pair(cell1,cell2)) != walls.end());
	if(!flag){
		r.die();
	}
	return flag;

}

bool legalMove(Racer* r, const set<pair<int,int> >& walls){
	bool flag;
	point p1 = r->getPrevLocation();
	point p2 = r->getLocation();


	int cell1 = p1.y * col + p1.x;
	int cell2 = p2.y * col + p2.x;

	if(cell1 == cell2) return true;
	if(p1.x < 0 || p1.y < 0 || p2.x < 0 || p2.y < 0) return 1;
	if(p1.x >= col || p1.y >= row || p2.x >= col || p2.y >= row) return 1;

	flag =  (walls.find(make_pair(cell1,cell2)) != walls.end());
	if(!flag){
		r->die();
	}
	return flag;

}


void drawMaze(string fName, SDL_Plotter& g){
	ifstream file(fName);
	char c;
	for(int y = 0; y < g.getRow(); y++){
		for(int x = 0; x < g.getCol(); x++){
			file.get(c);
			if(c == '1'){
				g.plotPixel(x,y,0,0,0);
			}
		}
		file.get(c);
	}

	file.close();
	return;
}


void drawCircle(point loc, int size, color c, SDL_Plotter& g){
	for(double i = -size; i <= size;i+=0.1){
		for(double j = -size; j <= size; j+=0.1){
			if(i*i + j*j <= size*size){
				g.plotPixel(round(loc.x+i),round(loc.y+j),c);
			}
		}
	}

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawMaze(set<pair<int,int> >& walls, SDL_Plotter& g){
	Color color = {255,0,0};
	color.r = color.g = color.b = 0;
	g.clear();
	for(int cell = 0; cell < row*col; cell++){
		if(cell+1 < row*col && walls.find(make_pair(cell,cell+1)) == walls.end())
			drawWall(cell, cell+1, color , g);

		if(cell-1 > 0 && walls.find(make_pair(cell,cell-1)) == walls.end())
				drawWall(cell, cell-1, color , g);

		if(cell+col < row*col && walls.find(make_pair(cell,cell+col)) == walls.end())
				drawWall(cell, cell+col, color , g);

		if(cell-col >0 && walls.find(make_pair(cell,cell-col)) == walls.end())
				drawWall(cell, cell-col, color , g);
	}
	drawFrame(color,g);
	g.update();

}

void buildMaze(bool fast, disjointSet& cells,set<pair<int,int> >& walls, SDL_Plotter& g){
	Color color = {255,0,0};
	color.r = color.g = color.b = 0;
	int cell1;
	int cell2;
	while(!cells.isSameSet(0,row*col-1)){
		cell1 = rand()%(row*col);
		cell2 = -1;
		do{
			switch(rand()%4){
				case 0: if(cell1%col < col-1) cell2 = cell1 + 1; break;
				case 1: if(cell1%col > 0) cell2 = cell1 - 1; break;
				case 2: cell2 = cell1 + col; break;
				case 3: cell2 = cell1 - col; break;
			}
		}while(cell2<0 || cell2 >= col*row);

		if(!cells.isSameSet(cell1,cell2)){
			cells.unionSet(cell1, cell2);
			walls.insert(make_pair(cell1,cell2));
			walls.insert(make_pair(cell2,cell1));
		}
		if(!fast) drawMaze(walls, g);
	}

	drawMaze(walls, g);
}

void drawBlock(int x, int y, Color color, SDL_Plotter& g){
	x *= block;
	y *= block;
	for(int r = padding; r < block-padding; r++){
		for(int c = padding; c < block-padding; c++){
				g.plotPixel(buffer+x+c, buffer+y+r, color.r, color.g, color.b);
		}
	}

}


void drawWall(int cell1, int cell2, Color color, SDL_Plotter& g){
	if(cell1 < 0 || cell2 < 0) return;
	if(cell1 > row*col || cell2 > row*col) return;

	bool horizontal;

	int r = (cell1/col);
	int c = (cell1 - r*col);

	r *= block;
	c *= block;

	if(cell1 == cell2 - 1){ // right wall
		horizontal = false;
		c += block;
	}
	if(cell1 == cell2 + 1){ //left wall
		horizontal = false;

	}
	if(cell1-col == cell2){ //top wall
		horizontal = true;

	}
	if(cell1+col == cell2){ //bottom wall
		horizontal = true;
		r += block;

	}

	if(horizontal){
		for(int t = -wallThickness/2; t <= wallThickness/2; t++){
			for(int x = 0; x < block; x++){
				g.plotPixel(buffer+x+c, buffer+r+t, color.r, color.g, color.b);
			}
		}
	}
	else{
		for(int t = -wallThickness/2; t <= wallThickness/2; t++){
			for(int y = 0; y < block; y++){
				g.plotPixel(buffer+c+t, buffer+y+r, color.r, color.g, color.b);
			}
		}
	}

	//g.update();

}


void drawFrame(Color color, SDL_Plotter& g){
	for(int t = -wallThickness/2; t <= wallThickness/2; t++){
		//for(int r = block; r < row*block; r++){
			for(int r = 0; r < row*block; r++){
				g.plotPixel(buffer+t,buffer+r,color.r,color.g,color.b);
			}
	}
	for(int t = -wallThickness/2; t <= wallThickness/2; t++){
		//for(int r = 0; r < (row-1)*block; r++){
		for(int r = 0; r < (row)*block; r++){
			g.plotPixel(buffer+col*block+t,buffer+r,color.r,color.g,color.b);
		}
	}

	for(int t = -wallThickness/2; t <= wallThickness/2; t++){
		for(int c = 0; c < col*block; c++){
			g.plotPixel(buffer+c,buffer+row*block+t,color.r,color.g,color.b);
			g.plotPixel(buffer+c,buffer+t,color.r,color.g,color.b);
		}
	}
}

void* solveMazeThread(void*p){
	set<pair<int,int> > *walls = static_cast<Parameter*>(p)->walls;
	SDL_Plotter* g = static_cast<Parameter*>(p)->g;
	int start = 0;
	int finish = row*col -1;

	bool flag = false;
	Color red   = {255,0,0},
		  green = {0,255,0},
          blue  = {0,0,255},
		  black = {0,0,0} ,
		  white = {255,200,200},
		  color = {255,255,0};

	stack<int> path;
	path.push(start); // start location

	while(!path.empty()){

		int current = path.top();
		path.pop();

		int r = current/col;
		int c = current - col*r;



		if(current == finish){
			drawBlock(c,r, color, *g);
			g->update();
			flag =  true;
		}

		if(!flag){
			if( walls->find(make_pair(current,current+1)) != walls->end() ||
			    walls->find(make_pair(current+1,current)) != walls->end()){
					drawBlock(c,r, red, *g);
					g->update();

					walls->erase(make_pair(current,current+1));
					walls->erase(make_pair(current+1,current));
					path.push(current+1);
					flag = path.top() == finish;
					//continue;
			}

			if(flag){
				drawBlock(c,r, green, *g);
				g->update();
				flag = true;
			}
			else{
				drawBlock(c,r, white, *g);
				g->update();
			}
		}
		if(!flag){
			if( walls->find(make_pair(current,current-1)) != walls->end() ||
			    walls->find(make_pair(current-1,current)) != walls->end()){
					drawBlock(c,r, red, *g);
					g->update();

					walls->erase(make_pair(current,current-1));
					walls->erase(make_pair(current-1,current));
					path.push(current-1);
					flag = path.top() == finish;
					//continue;
			}

			if(flag){
				drawBlock(c,r, green, *g);
				g->update();
				flag = true;
			}
			else{
				drawBlock(c,r, white, *g);
				g->update();
			}

		}
		if(!flag){
			if( walls->find(make_pair(current,current+col)) != walls->end() ||
			    walls->find(make_pair(current+col,current)) != walls->end()){
					drawBlock(c,r, red, *g);
					g->update();

					walls->erase(make_pair(current,current+col));
					walls->erase(make_pair(current+col,current));
					path.push(current);
					flag = path.top() == finish;
					//continue;
			}

			if(flag){
				drawBlock(c,r, green, *g);
				g->update();
				flag =  true;
			}
			else{
				drawBlock(c,r, white, *g);
				g->update();
			}

		}
		if(!flag){
			if( walls->find(make_pair(current,current-col)) != walls->end() ||
			    walls->find(make_pair(current-col,current)) != walls->end()){
					drawBlock(c,r, red, *g);
					g->update();

					walls->erase(make_pair(current,current-col));
					walls->erase(make_pair(current-col,current));
					path.push(current-col);
					flag = path.top() == finish;
					//continue;
			}

			if(flag){
				drawBlock(c,r, green, *g);
				g->update();
				flag = true;
			}
			else{
				drawBlock(c,r, white, *g);
				g->update();
			}

		}
	}

}

void solveMaze(set<pair<int,int> > walls, SDL_Plotter& g){
	Color color;
	color.r = 255;
	color.g = 225;
	color.b = 0;


	drawSolution(0, color, walls, g);

}


bool drawSolution(int start, Color color, set<pair<int,int> >& walls, SDL_Plotter& g){

	int r = start/col;
	int c = start - col*r;
	bool flag = false;
	Color red   = {255,0,0},
		  green = {0,255,0},
		  white = {255,200,200};



	if(start == row*col-1){
		drawBlock(c,r, color, g);
		g.update();
		return true;
	}

	if(!flag){
		if( walls.find(make_pair(start,start+1)) != walls.end() ||
		    walls.find(make_pair(start+1,start)) != walls.end()){
				drawBlock(c,r, red, g);
				g.update();

				walls.erase(make_pair(start,start+1));
				walls.erase(make_pair(start+1,start));
				flag =  ( drawSolution(start+1, color, walls, g));
		}

		if(flag){
			drawBlock(c,r, green, g);
			g.update();
			return true;
		}
		else{
			drawBlock(c,r, white, g);
			g.update();
		}
	}
	if(!flag){
		if( walls.find(make_pair(start,start-1)) != walls.end() ||
		    walls.find(make_pair(start-1,start)) != walls.end()){
				drawBlock(c,r, red, g);
				g.update();

				walls.erase(make_pair(start,start-1));
				walls.erase(make_pair(start-1,start));
				flag =  ( drawSolution(start-1, color, walls, g));
		}

		if(flag){
			drawBlock(c,r, green, g);
			g.update();
			return true;
		}
		else{
			drawBlock(c,r, white, g);
			g.update();
		}

	}
	if(!flag){
		if( walls.find(make_pair(start,start+col)) != walls.end() ||
		    walls.find(make_pair(start+col,start)) != walls.end()){
				drawBlock(c,r, red, g);
				g.update();

				walls.erase(make_pair(start,start+col));
				walls.erase(make_pair(start+col,start));
				flag =  ( drawSolution(start+col, color, walls, g));
		}

		if(flag){
			drawBlock(c,r, green, g);
			g.update();
			return true;
		}
		else{
			drawBlock(c,r, white, g);
			g.update();
		}

	}
	if(!flag){
		if( walls.find(make_pair(start,start-col)) != walls.end() ||
		    walls.find(make_pair(start-col,start)) != walls.end()){
				drawBlock(c,r, red, g);
				g.update();

				walls.erase(make_pair(start,start-col));
				walls.erase(make_pair(start-col,start));
				flag =  ( drawSolution(start-col, color, walls, g));
		}

		if(flag){
			drawBlock(c,r, green, g);
			g.update();
			return true;
		}
		else{
			drawBlock(c,r, white, g);
			g.update();
		}

	}

	return false;
}



bool drawSolution2(int start, int current, disjointSet& cells, set<pair<int,int> >& walls,SDL_Plotter& g){

	int r = current/col;
	int c = current - col*r;
	bool flag = false;
	Color red   = {255,0,0},
		  green = {0,255,0},
		  white = {255,200,200};



	if(current < 0 || current >= row*col){
		return false;
	}

	if(!cells.isSameSet(start, current)){
		return false;
	}

	if(current == row*col-1){
		drawBlock(c,r, green, g);
		g.update();
		return true;
	}

	drawBlock(c,r, red, g);

	if(drawSolution2(start, current+1, cells,walls, g)){
				drawBlock(c,r, green, g);
				g.update();
				return true;
	}
	else{
			drawBlock(c,r, white, g);
			g.update();
			return false;
	}

	if(drawSolution2(start, current-1, cells,walls, g)){
				drawBlock(c,r, green, g);
				g.update();
				return true;
	}
	else{
			drawBlock(c,r, white, g);
			g.update();
			return false;
	}

	if(drawSolution2(start, current+col, cells,walls, g)){
				drawBlock(c,r, green, g);
				g.update();
				return true;
	}
	else{
			drawBlock(c,r, white, g);
			g.update();
			return false;
	}

	if(drawSolution2(start, current-col, cells,walls, g)){
				drawBlock(c,r, green, g);
				g.update();
				return true;
	}
	else{
			drawBlock(c,r, white, g);
			g.update();
			return false;
	}

	return false;
}