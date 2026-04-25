#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <set>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cctype>
#include <stack>
#include <queue>
#include <filesystem>
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
void buildMaze(bool fast, disjointSet& cells,set<pair<int,int> >& walls, SDL_Plotter* g);
void drawMaze(set<pair<int,int> >& walls, SDL_Plotter& g);
void randomizeMazeSize();
void randomizeFinishLocation();
void buildRandomMaze(disjointSet& cells, set<pair<int,int> >& walls, SDL_Plotter& g);
void buildRandomMaze(disjointSet& cells, set<pair<int,int> >& walls, SDL_Plotter* g);
void drawFinish(SDL_Plotter& g);
int nextMazeId();
void logRun(int mazeId, int runNumber, double elapsed, bool finished, set<pair<int,int> >& walls);
void logRun(int mazeId, int runNumber, string searchName, double elapsed, bool finished, set<pair<int,int> >& walls, bool saveSlowMazeFile);
void saveSlowMaze(int mazeId, int runNumber, double elapsed, set<pair<int,int> >& walls);
bool wallFollowerSearch(const set<pair<int,int> >& walls);
bool breadthFirstSearch(const set<pair<int,int> >& walls);
bool depthFirstSearch(const set<pair<int,int> >& walls);
void runDataCollection();
void runVisualDataCollection();
vector<int> wallFollowerPath(const set<pair<int,int> >& walls);
vector<int> breadthFirstPath(const set<pair<int,int> >& walls);
vector<int> depthFirstPath(const set<pair<int,int> >& walls);
void drawScaledMaze(const set<pair<int,int> >& walls, SDL_Plotter& g);
void drawScaledPath(const vector<int>& path, Color color, SDL_Plotter& g);
void drawScaledCell(int cell, Color color, SDL_Plotter& g);
void drawScaledLine(int x1, int y1, int x2, int y2, Color color, SDL_Plotter& g);


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

bool readMaze(set<pair<int,int> >& walls, string fName){
	int x,y;
	walls.clear();
	ifstream file{fName};
	if(!file){
		return false;
	}
	while(file >> x >> y){
		walls.insert(make_pair(x,y));
	}
	file.close();
	return true;
}

bool legalMove(Racer& r, const set<pair<int,int> >& walls);
bool legalMove(Racer* r, const set<pair<int,int> >& walls);

filesystem::path projectRoot(){
	filesystem::path sourcePath = filesystem::absolute(__FILE__).parent_path();
	if(filesystem::exists(sourcePath / "CMakeLists.txt")){
		return sourcePath;
	}

	filesystem::path currentPath = filesystem::current_path();
	while(!currentPath.empty()){
		if(filesystem::exists(currentPath / "CMakeLists.txt")){
			return currentPath;
		}
		filesystem::path parent = currentPath.parent_path();
		if(parent == currentPath){
			break;
		}
		currentPath = parent;
	}

	return filesystem::current_path();
}

filesystem::path logDirectory(){
	return projectRoot() / "maze_logs";
}


int main(int argc, char ** argv)
{
	bool visualMode = false;
	for(int arg = 1; arg < argc; arg++){
		string option = argv[arg];
		if(option == "--visual"){
			visualMode = true;
		}
	}

	if(!visualMode){
		runDataCollection();
		return 0;
	}

	runVisualDataCollection();
	return 0;

	Timer clock;
	bool doneOnce = false;
	bool nextRun = true;
    point p;
    color c;
    char next;
    int size;
    int runCount = 0;
    Uint32 RGB;
    SDL_Plotter g(maxRow*block+2*buffer,maxCol*block+2*buffer);
    Racer robot(&g);
    RaceCarDriver driver(&robot);


	disjointSet cells(maxRow*maxCol);
	set<pair<int,int> > walls;

	Color color = {255,0,0};
	color.r = color.g = color.b = 0;

	srand(time(0));

	int mazeId = nextMazeId();
	bool useMazeFile = false;
	string mazeFile = "wallMaze1.txt";

	for(int arg = 1; arg < argc; arg++){
		string option = argv[arg];
		if(option == "--maze-file" && arg + 1 < argc){
			useMazeFile = true;
			mazeFile = argv[++arg];
		}
		else if(option == "--maze-id" && arg + 1 < argc){
			mazeId = stoi(argv[++arg]);
		}
		else if(option == "--random"){
			useMazeFile = false;
		}
	}

	if(useMazeFile){
		if(!readMaze(walls, mazeFile)){
			cerr << "Could not open " << mazeFile << ". Building a random maze instead." << endl;
			buildRandomMaze(cells, walls, g);
		}
		cout << "Maze ID: " << mazeId << " from " << mazeFile << endl;
	}
	else{
		buildRandomMaze(cells, walls, g);
		cout << "Maze ID: " << mazeId << endl;
	}

	drawMaze(walls,g);
	drawFinish(g);
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
				g.Sleep(5000);

			}

			if(g.mouseClick()){
			}

			robot.move(driver.nextMove());
			legalMove(robot, walls);
			robot.draw(g);
			g.update();
		}
		else{
			double elapsed = clock.elapsed();
			bool finished = robot.Finished();
			if(robot.Finished()){
			    cout << elapsed << " seconds" << endl;
			}
			else{
				cout << "DIE!" << endl;
			}
			logRun(mazeId, runCount + 1, elapsed, finished, walls);
			clock.reset();
			runCount++;
			robot.setAlive(true);
			robot.setLocation(point(0,0));
			robot.setPrevLocation(point(0,0));
			robot.setFinished(false);
			g.clear();
			drawMaze(walls,g);
			drawFinish(g);
			robot.draw(g);
			g.update();
		}
    }

    g.Sleep(5000);
}

void randomizeMazeSize(){
	static random_device seed;
	static mt19937 rng(seed());
	uniform_int_distribution<int> rowPicker(minRow, maxRow);
	uniform_int_distribution<int> colPicker(minCol, maxCol);

	row = rowPicker(rng);
	col = colPicker(rng);
}

void buildRandomMaze(disjointSet& cells, set<pair<int,int> >& walls, SDL_Plotter& g){
	buildRandomMaze(cells, walls, &g);
}

void buildRandomMaze(disjointSet& cells, set<pair<int,int> >& walls, SDL_Plotter* g){
	randomizeMazeSize();
	randomizeFinishLocation();
	walls.clear();
	cells = disjointSet(row*col);
	cells.initSet(row*col);
	buildMaze(true, cells, walls, g);
	cout << "Maze size: " << col << " x " << row << endl;
	cout << "Finish: " << finishCol << ", " << finishRow << endl;
}

void randomizeFinishLocation(){
	finishRow = row - 1;
	finishCol = col - 1;
}

void drawFinish(SDL_Plotter& g){
	Color green = {0, 180, 0};
	drawBlock(finishCol, finishRow, green, g);
}

int nextMazeId(){
	filesystem::path logs = logDirectory();
	filesystem::create_directories(logs);

	int mazeId = 1;
	ifstream in{logs / "next_maze_id.txt"};
	if(in){
		in >> mazeId;
	}
	in.close();

	ofstream out{logs / "next_maze_id.txt"};
	out << mazeId + 1 << endl;
	out.close();

	return mazeId;
}

void logRun(int mazeId, int runNumber, double elapsed, bool finished, set<pair<int,int> >& walls){
	logRun(mazeId, runNumber, "RUN_" + to_string(runNumber), elapsed, finished, walls, true);
}

void logRun(int mazeId, int runNumber, string searchName, double elapsed, bool finished, set<pair<int,int> >& walls, bool saveSlowMazeFile){
	filesystem::path logs = logDirectory();
	filesystem::create_directories(logs);

	ofstream file{logs / "run_times.txt", ios::app};
	file << mazeId << " "
		 << runNumber << " "
		 << searchName << " "
		 << col << "x" << row << " "
		 << finishCol << "," << finishRow << " "
		 << elapsed << " seconds "
		 << (finished ? "FINISHED" : "DIED") << endl;
	file.close();

	if(saveSlowMazeFile && elapsed > 30.0){
		saveSlowMaze(mazeId, runNumber, elapsed, walls);
	}
}

void saveSlowMaze(int mazeId, int runNumber, double elapsed, set<pair<int,int> >& walls){
	filesystem::path slowMazeDirectory = logDirectory() / "slow_mazes";
	filesystem::create_directories(slowMazeDirectory);

	filesystem::path fileName = slowMazeDirectory / ("maze_" + to_string(mazeId) +
						  "_run_" + to_string(runNumber) + ".txt");
	ofstream file{fileName};
	file << "Maze ID: " << mazeId << endl;
	file << "Run: " << runNumber << endl;
	file << "Elapsed seconds: " << elapsed << endl;
	file << "Rows: " << row << endl;
	file << "Columns: " << col << endl;
	file << "Finish row: " << finishRow << endl;
	file << "Finish column: " << finishCol << endl;
	file << "Open passages:" << endl;
	for(auto wall: walls){
		file << wall.first << " " << wall.second << endl;
	}
	file.close();
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
	buildMaze(fast, cells, walls, &g);
}

void buildMaze(bool fast, disjointSet& cells,set<pair<int,int> >& walls, SDL_Plotter* g){
	Color color = {255,0,0};
	color.r = color.g = color.b = 0;
	static random_device seed;
	static mt19937 rng(seed());
	vector<pair<int,int> > edges;
	edges.reserve(row * (col - 1) + col * (row - 1));

	for(int cell = 0; cell < row * col; cell++){
		if(cell % col < col - 1){
			edges.push_back(make_pair(cell, cell + 1));
		}
		if(cell + col < row * col){
			edges.push_back(make_pair(cell, cell + col));
		}
	}

	shuffle(edges.begin(), edges.end(), rng);

	for(auto edge: edges){
		int cell1 = edge.first;
		int cell2 = edge.second;

		if(!cells.isSameSet(cell1, cell2)){
			cells.unionSet(cell1, cell2);
			walls.insert(make_pair(cell1, cell2));
			walls.insert(make_pair(cell2, cell1));
			if(g != nullptr && !fast) drawMaze(walls, *g);
		}
	}

	if(g != nullptr) drawMaze(walls, *g);
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

bool canMove(int cell, DIRECTION direction, const set<pair<int,int> >& walls){
	int next;
	switch(direction){
		case NORTH:
			if(cell < col) return false;
			next = cell - col;
			break;
		case SOUTH:
			if(cell + col >= row * col) return false;
			next = cell + col;
			break;
		case EAST:
			if(cell % col == col - 1) return false;
			next = cell + 1;
			break;
		case WEST:
			if(cell % col == 0) return false;
			next = cell - 1;
			break;
	}
	return walls.find(make_pair(cell, next)) != walls.end();
}

int moveToCell(int cell, DIRECTION direction){
	switch(direction){
		case NORTH: return cell - col;
		case SOUTH: return cell + col;
		case EAST:  return cell + 1;
		case WEST:  return cell - 1;
	}
	return cell;
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

bool wallFollowerSearch(const set<pair<int,int> >& walls){
	int current = 0;
	int finish = finishRow * col + finishCol;
	DIRECTION facing = EAST;
	long long maxSteps = static_cast<long long>(row) * col * 8;

	for(long long steps = 0; current != finish && steps < maxSteps; steps++){
		vector<DIRECTION> choices = {leftOf(facing), facing, rightOf(facing), backOf(facing)};
		bool moved = false;
		for(DIRECTION direction: choices){
			if(canMove(current, direction, walls)){
				current = moveToCell(current, direction);
				facing = direction;
				moved = true;
				break;
			}
		}
		if(!moved){
			return false;
		}
	}

	return current == finish;
}

bool breadthFirstSearch(const set<pair<int,int> >& walls){
	int finish = finishRow * col + finishCol;
	vector<bool> visited(row * col, false);
	queue<int> cells;
	vector<DIRECTION> directions = {EAST, SOUTH, NORTH, WEST};

	visited[0] = true;
	cells.push(0);

	while(!cells.empty()){
		int current = cells.front();
		cells.pop();

		if(current == finish){
			return true;
		}

		for(DIRECTION direction: directions){
			if(canMove(current, direction, walls)){
				int next = moveToCell(current, direction);
				if(!visited[next]){
					visited[next] = true;
					cells.push(next);
				}
			}
		}
	}

	return false;
}

bool depthFirstSearch(const set<pair<int,int> >& walls){
	int finish = finishRow * col + finishCol;
	vector<bool> visited(row * col, false);
	stack<int> cells;
	vector<DIRECTION> directions = {WEST, NORTH, SOUTH, EAST};

	cells.push(0);

	while(!cells.empty()){
		int current = cells.top();
		cells.pop();

		if(visited[current]){
			continue;
		}
		visited[current] = true;

		if(current == finish){
			return true;
		}

		for(DIRECTION direction: directions){
			if(canMove(current, direction, walls)){
				int next = moveToCell(current, direction);
				if(!visited[next]){
					cells.push(next);
				}
			}
		}
	}

	return false;
}

void runDataCollection(){
	Timer clock;
	disjointSet cells(maxRow * maxCol);
	set<pair<int,int> > walls;

	cout << "Starting infinite data collection with " << minCol << "x" << minRow
		 << " mazes. Press Ctrl+C to stop." << endl;

	while(true){
		int mazeId = nextMazeId();
		buildRandomMaze(cells, walls, nullptr);
		cout << "Maze ID: " << mazeId << endl;

		clock.reset();
		bool wallFinished = wallFollowerSearch(walls);
		logRun(mazeId, 1, "WALL_FOLLOWER", clock.elapsed(), wallFinished, walls, false);

		clock.reset();
		bool bfsFinished = breadthFirstSearch(walls);
		logRun(mazeId, 2, "BFS", clock.elapsed(), bfsFinished, walls, false);

		clock.reset();
		bool dfsFinished = depthFirstSearch(walls);
		logRun(mazeId, 3, "DFS", clock.elapsed(), dfsFinished, walls, false);
	}
}

const int visualWindowSize = 1000;
const int visualMargin = 20;

int scaledX(int cell){
	int mazeWidth = visualWindowSize - 2 * visualMargin;
	return visualMargin + (cell % col) * mazeWidth / col;
}

int scaledY(int cell){
	int mazeHeight = visualWindowSize - 2 * visualMargin;
	return visualMargin + (cell / col) * mazeHeight / row;
}

vector<int> wallFollowerPath(const set<pair<int,int> >& walls){
	vector<int> path;
	int current = 0;
	int finish = finishRow * col + finishCol;
	DIRECTION facing = EAST;
	long long maxSteps = static_cast<long long>(row) * col * 8;

	path.push_back(current);
	for(long long steps = 0; current != finish && steps < maxSteps; steps++){
		vector<DIRECTION> choices = {leftOf(facing), facing, rightOf(facing), backOf(facing)};
		bool moved = false;
		for(DIRECTION direction: choices){
			if(canMove(current, direction, walls)){
				current = moveToCell(current, direction);
				facing = direction;
				path.push_back(current);
				moved = true;
				break;
			}
		}
		if(!moved){
			break;
		}
	}

	if(current != finish){
		path.clear();
	}
	return path;
}

vector<int> breadthFirstPath(const set<pair<int,int> >& walls){
	int finish = finishRow * col + finishCol;
	vector<bool> visited(row * col, false);
	vector<int> parent(row * col, -1);
	queue<int> cells;
	vector<DIRECTION> directions = {EAST, SOUTH, NORTH, WEST};

	visited[0] = true;
	cells.push(0);

	while(!cells.empty()){
		int current = cells.front();
		cells.pop();

		if(current == finish){
			break;
		}

		for(DIRECTION direction: directions){
			if(canMove(current, direction, walls)){
				int next = moveToCell(current, direction);
				if(!visited[next]){
					visited[next] = true;
					parent[next] = current;
					cells.push(next);
				}
			}
		}
	}

	vector<int> path;
	if(!visited[finish]){
		return path;
	}
	for(int current = finish; current != -1; current = parent[current]){
		path.push_back(current);
	}
	reverse(path.begin(), path.end());
	return path;
}

vector<int> depthFirstPath(const set<pair<int,int> >& walls){
	int finish = finishRow * col + finishCol;
	vector<bool> visited(row * col, false);
	vector<int> parent(row * col, -1);
	stack<int> cells;
	vector<DIRECTION> directions = {WEST, NORTH, SOUTH, EAST};

	cells.push(0);

	while(!cells.empty()){
		int current = cells.top();
		cells.pop();

		if(visited[current]){
			continue;
		}
		visited[current] = true;

		if(current == finish){
			break;
		}

		for(DIRECTION direction: directions){
			if(canMove(current, direction, walls)){
				int next = moveToCell(current, direction);
				if(!visited[next]){
					parent[next] = current;
					cells.push(next);
				}
			}
		}
	}

	vector<int> path;
	if(!visited[finish]){
		return path;
	}
	for(int current = finish; current != -1; current = parent[current]){
		path.push_back(current);
	}
	reverse(path.begin(), path.end());
	return path;
}

void drawScaledLine(int x1, int y1, int x2, int y2, Color color, SDL_Plotter& g){
	int dx = abs(x2 - x1);
	int sx = x1 < x2 ? 1 : -1;
	int dy = -abs(y2 - y1);
	int sy = y1 < y2 ? 1 : -1;
	int error = dx + dy;

	while(true){
		g.plotPixel(x1, y1, color.r, color.g, color.b);
		if(x1 == x2 && y1 == y2){
			break;
		}
		int doubleError = 2 * error;
		if(doubleError >= dy){
			error += dy;
			x1 += sx;
		}
		if(doubleError <= dx){
			error += dx;
			y1 += sy;
		}
	}
}

void drawScaledCell(int cell, Color color, SDL_Plotter& g){
	int x = scaledX(cell);
	int y = scaledY(cell);
	for(int dy = -1; dy <= 1; dy++){
		for(int dx = -1; dx <= 1; dx++){
			g.plotPixel(x + dx, y + dy, color.r, color.g, color.b);
		}
	}
}

void drawScaledPath(const vector<int>& path, Color color, SDL_Plotter& g){
	if(path.empty()){
		return;
	}

	for(size_t i = 1; i < path.size(); i++){
		drawScaledLine(scaledX(path[i - 1]), scaledY(path[i - 1]),
					   scaledX(path[i]), scaledY(path[i]), color, g);
	}
}

void drawScaledMaze(const set<pair<int,int> >& walls, SDL_Plotter& g){
	g.clear();
	Color black = {0, 0, 0};
	Color white = {245, 245, 245};
	Color startColor = {0, 120, 255};
	Color finishColor = {0, 180, 0};

	for(int y = visualMargin; y < visualWindowSize - visualMargin; y++){
		for(int x = visualMargin; x < visualWindowSize - visualMargin; x++){
			g.plotPixel(x, y, black.r, black.g, black.b);
		}
	}

	for(auto passage: walls){
		if(passage.first < passage.second){
			drawScaledLine(scaledX(passage.first), scaledY(passage.first),
						   scaledX(passage.second), scaledY(passage.second),
						   white, g);
		}
	}

	drawScaledCell(0, startColor, g);
	drawScaledCell(finishRow * col + finishCol, finishColor, g);
	g.update();
}

void runVisualDataCollection(){
	Timer clock;
	disjointSet cells(maxRow * maxCol);
	set<pair<int,int> > walls;
	SDL_Plotter g(visualWindowSize, visualWindowSize, false);

	cout << "Starting visual data collection with scaled " << minCol << "x" << minRow
		 << " mazes. Close the window or press Escape to stop." << endl;

	while(!g.getQuit()){
		int mazeId = nextMazeId();
		buildRandomMaze(cells, walls, nullptr);
		cout << "Maze ID: " << mazeId << endl;

		drawScaledMaze(walls, g);
		if(g.getQuit()) break;

		clock.reset();
		vector<int> wallPath = wallFollowerPath(walls);
		logRun(mazeId, 1, "WALL_FOLLOWER", clock.elapsed(), !wallPath.empty(), walls, false);
		drawScaledPath(wallPath, Color{255, 50, 50}, g);
		drawScaledCell(finishRow * col + finishCol, Color{0, 180, 0}, g);
		g.update();
		if(g.getQuit()) break;

		clock.reset();
		vector<int> bfsPath = breadthFirstPath(walls);
		logRun(mazeId, 2, "BFS", clock.elapsed(), !bfsPath.empty(), walls, false);
		drawScaledPath(bfsPath, Color{255, 220, 0}, g);
		drawScaledCell(finishRow * col + finishCol, Color{0, 180, 0}, g);
		g.update();
		if(g.getQuit()) break;

		clock.reset();
		vector<int> dfsPath = depthFirstPath(walls);
		logRun(mazeId, 3, "DFS", clock.elapsed(), !dfsPath.empty(), walls, false);
		drawScaledPath(dfsPath, Color{0, 220, 255}, g);
		drawScaledCell(0, Color{0, 120, 255}, g);
		drawScaledCell(finishRow * col + finishCol, Color{0, 180, 0}, g);
		g.update();

		for(int frame = 0; frame < 30 && !g.getQuit(); frame++){
			g.Sleep(100);
		}
	}
}

void* solveMazeThread(void*p){
	set<pair<int,int> > *walls = static_cast<Parameter*>(p)->walls;
	SDL_Plotter* g = static_cast<Parameter*>(p)->g;
	int start = 0;
	int finish = finishRow * col + finishCol;

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

	return nullptr;
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



	if(start == finishRow * col + finishCol){
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

	if(current == finishRow * col + finishCol){
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
