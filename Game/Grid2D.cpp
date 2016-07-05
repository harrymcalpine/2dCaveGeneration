#include "Grid2D.h"
#include <iostream>
#include "gameobject.h"
#include <array>
#include <vector>
#include <cstdio>
#include <rpcndr.h>
#include <time.h>
#include <stdlib.h>
#include "GameData.h"
#include <thread>

using namespace std;

Grid2D::Grid2D()
{
	seed = rand() % 254;
}

Grid2D::~Grid2D()
{
	delete m_array;
}

void Grid2D::run(GameData* _GD)
{
	int size = _GD->_size;

	m_MaxCols = size;
	m_MaxRows = size;

	genCount = 0;
	std::thread m_drawThread(&Grid2D::updateArray, this, _GD);
	m_drawThread.detach();

	for (int i = 0; i < m_MaxRows; i++)
	{
		for (int j = 0; j < m_MaxCols; j++)
		{
			unsigned int randNum = rand() % 2;
			if (randNum == 0)
			{
				m_array[j][i] = DEAD;
			}
			else
			{
				m_array[j][i] = ALIVE;
			}
		}
	}
	while (inLoop != true)
	{
		inLoop = false;
		neighboursCA();
		genCount++;
		if (genCount == 5)
		{
			inLoop = true;
		}
	};
	cout << endl;
	copy();
	genCount = 0;
	while (inLoop == true)
	{
		findNeighbours(1);
		updateArray(_GD);
		genCount++;
		if (genCount == 5)
		{
			inLoop = false;
		}
	};
	findNeighbours(2);
	updateArray(_GD);
	copyToGD(_GD);
	std::thread m_newThread(&Grid2D::runWater, this, _GD);
	m_newThread.detach();
}

void Grid2D::printCellular()
{
	for (int i = 0; i < m_MaxRows; i++)
	{
		for (int j = 0; j < m_MaxCols; j++)
		{
			if (m_array[j][i] == WALL)
			{
				cout << "2";
			}
			if (m_array[j][i] == DEAD)
			{
				cout << ".";
			}
			if (m_array[j][i] == ALIVE)
			{
				cout << "#";
			}
		}
		cout << endl;
	}
}

void Grid2D::neighboursCA()
{
	for (int i = 1; i < m_MaxRows; i++)
	{
		for (int j = 1; j < m_MaxCols; j++)
		{
			neighbours = 0;

			if (m_array[j - 1][i - 1] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j][i - 1] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j + 1][i - 1] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j - 1][i] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j + 1][i] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j - 1][i + 1] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j][i + 1] == ALIVE)
			{
				neighbours++;
			}
			if (m_array[j + 1][i + 1] == ALIVE)
			{
				neighbours++;
			}
		}
	}
}

void Grid2D::findNeighbours(int x)
{
	for (int i = 0; i < m_MaxRows; i++)
	{
		for (int j = 0; j < m_MaxCols; j++)
		{
			neighbours = 0;
			if (x == 1)
			{
				//Moore neighbourhood
				if (m_GameArray[j - 1][i - 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j][i - 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j + 1][i - 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j - 1][i] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j + 1][i] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j - 1][i + 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j][i + 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j + 1][i + 1] == ALIVE)
				{
					neighbours++;
				}

				fillCave(neighbours, j, i, x);
			}
			else if (x == 2)
			{
				//Von Neumann Neighbourhood
				if (m_GameArray[j][i - 1] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j - 1][i] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j + 1][i] == ALIVE)
				{
					neighbours++;
				}
				if (m_GameArray[j][i + 1] == ALIVE)
				{
					neighbours++;
				}
				fillCave(neighbours, j, i, x);
			}
		}
	}
}
void Grid2D::copy()
{
	for (int i = 0; i < m_MaxRows; i++)
	{
		for (int j = 0; j < m_MaxCols; j++)
		{
			m_GameArray[j][i] = m_array[j][i];
		}
	}
}

void Grid2D::fillCave(int _neighbours, int j, int i, int x)
{
	if (x == 1)
	{
		//1 Cell is alive and less than 3 cells in neighborhood = floor
		if ((m_GameArray[j][i] == ALIVE) && (_neighbours < 3))
		{
			m_nextState[j][i] = DEAD;
		}
		//2 cell is dead and more than 4 cells in neighborhood = wall
		else if ((m_GameArray[j][i] == DEAD) && (_neighbours > 4))
		{
			m_nextState[j][i] = ALIVE;
		}
		else if ((m_GameArray[j][i] == DEAD) && (_neighbours = 4))
		{
			m_nextState[j][i] = DEAD;
		}
		else if ((m_GameArray[j][i] == ALIVE) && (_neighbours = 4))
		{
			m_nextState[j][i] = ALIVE;
		}
	}
	else if (x == 2)
	{
		if ((m_GameArray[j][i] == ALIVE) && (_neighbours < 4))
		{
			m_nextState[j][i] = WALL;
		}
	}
}

void Grid2D::copyToGD(GameData* _GD)
{
	m_pos = Vector2(10.0f, 10.0f);

	for (int i = 0; i < m_MaxRows; i++)
	{
		m_pos.x = 10.0f;
		for (int j = 0; j < m_MaxCols; j++)
		{
			if (m_GameArray[j][i] == ALIVE)
			{
				_GD->m_OM->SpawnObject(2, m_pos);
			}
			else if (m_GameArray[j][i] == DEAD)
			{
				_GD->m_OM->SpawnObject(3, m_pos);
			}
			else if (m_GameArray[j][i] == WALL)
			{
				_GD->m_OM->SpawnObject(1, m_pos);
			}
			m_pos.x = m_pos.x + 10.0f;
		}
		m_pos.y = m_pos.y + 10.0f;
	}
}

void Grid2D::updateArray(GameData* _GD)
{
	for (int i = 0; i < m_MaxRows; i++)
	{
		for (int j = 0; j < m_MaxCols; j++)
		{
			m_GameArray[j][i] = m_nextState[j][i];
			_GD->m_GameArray[j][i] = m_nextState[j][i];
		}
	}
}

void Grid2D::runWater(GameData* _GD)
{
	sf::sleep(sf::milliseconds(1000));

	createWater(_GD, m_MaxRows, m_MaxCols);
}

void Grid2D::createWater(GameData* _GD, int _rows, int _cols)
{
	for (int i = 0; i < _rows; i++)
	{
		for (int j = 0; j < _cols; j++)
		{
			if (i == 0)
			{
				unsigned int randNum = rand() % 3;
				if (_GD->m_GameArray[j][i] == DEAD)
				{
					if (randNum == 0)
					{
						_GD->m_GameArray[j][i] = WATER;
					}
				}
			}
			if (i == _rows)
			{
			}
		}
	}
	m_pos = Vector2(10.0f, 10.0f);

	for (int i = 0; i <= m_MaxRows; i++)
	{
		m_pos.x = 10.0f;
		sf::sleep(sf::milliseconds(500));
		for (int j = 0; j <= m_MaxCols; j++)
		{
			if (i >= 1)
			{
				if (((_GD->m_GameArray[j][i] != ALIVE) && (_GD->m_GameArray[j][i] != WALL)) && (_GD->m_GameArray[j][i - 1] == WATER))
				{
					if (((_GD->m_GameArray[j + 1][i] == ALIVE) && (_GD->m_GameArray[j + 1][i] == WALL)) && (_GD->m_GameArray[j + 1][i - 1] == WATER))
					{
						_GD->m_GameArray[j][i] = WATER;
					}
					if (((_GD->m_GameArray[j - 1][i] == ALIVE) && (_GD->m_GameArray[j - 1][i] == WALL)) && (_GD->m_GameArray[j - 1][i - 1] == WATER))
					{
						_GD->m_GameArray[j][i] = WATER;
					}

					_GD->m_GameArray[j][i] = WATER;
				}
			}
			if (_GD->m_GameArray[j][i] == WATER)
			{
				_GD->m_OM->SpawnObject(4, m_pos);
			}
			m_pos.x = m_pos.x + 10.0f;
		}
		m_pos.y = m_pos.y + 10.0f;
	}
}