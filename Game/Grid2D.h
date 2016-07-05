#pragma once
#include <vector>

#include <list>
#include "GameData.h"
#include "terrain.h"
#include "ObjectList.h"
#include <SFML\System.hpp>
using namespace std;
enum Status
{
	DEAD,
	ALIVE,
	WALL,
	WATER
};
class Grid2D
{
public:
	Grid2D();
	~Grid2D();
	GameData* _GD;
	void run(GameData * _GD);

	void printCellular();
	void neighboursCA();
	void findNeighbours(int x);
	void copy();
	void fillCave(int _neighbours, int j, int i, int x);
	void copyToGD(GameData * _GD);
	void updateArray(GameData * _GD);
	void runWater(GameData * _GD);

	void createWater(GameData * _GD, int _rows, int _cols);

	void createWater(int _rows, int _cols);
	//void createWater(GameData * _GD, int _rows, int _cols);

protected:
	unsigned int genCount = 0;
	int number;
	int *next;
	int neighbours = 0;

	int m_MaxRows;
	int m_MaxCols;
	int m_array[50][50] = { DEAD };
	int m_waterArray[50][50] = { DEAD };
	int seed = 0;
	Vector2 m_pos = Vector2(0.0f, 0.0f);
	int m_GameArray[50][50];
	int m_nextState[50][50];
	int *currentState;
	int *nextState;
	list<Terrain*> wall;

	bool inLoop = false;
};