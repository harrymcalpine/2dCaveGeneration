#include "objectManager.h"
#include "GameData.h"
#include "Terrainobject.h"
#include "DrawData2D.h"

ObjectsManager::ObjectsManager(ID3D11Device* _GD, int _MaxRows, int _MaxCols) : GameObject2D()
{
	for (int i = 0; i < _MaxRows; i++)
	{
		for (int j = 0; j < _MaxCols; j++)
		{
			myObjects.push_back(new TerrainObject("brown", _GD, 1));

			myObjects.push_back(new TerrainObject("darkBrown", _GD, 2));

			myObjects.push_back(new TerrainObject("white10", _GD, 3));
			
			myObjects.push_back(new TerrainObject("blue", _GD, 4));
		}
	}
}

ObjectsManager::~ObjectsManager()
{
	for (list<TerrainObject *>::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		delete (*it);
	}
}
void ObjectsManager::SpawnObject(int _Type, Vector2 _pos)
{
	for (list<TerrainObject *>::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		if (!(*it)->IsAlive())
		{
			if ((*it)->GetType() == _Type)
			{
				(*it)->Spawn(_pos);
				break;
			}
		}
	}
}
void ObjectsManager::AddWall(DrawData2D* _DD)
{
	for (list<TerrainObject*> ::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		if (((*it)->IsAlive()) & ((*it)->GetType() == 1))
		{
			(*it)->Draw(_DD);
		}
	}
}

void ObjectsManager::AddDeepWall(DrawData2D* _DD)
{
	for (list<TerrainObject*> ::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		if (((*it)->IsAlive()) & ((*it)->GetType() == 2))
		{
			(*it)->Draw(_DD);
		}
	}
}

void ObjectsManager::AddFloor(DrawData2D* _DD)
{
	for (list<TerrainObject*> ::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		if (((*it)->IsAlive()) & ((*it)->GetType() == 3))
		{
			(*it)->Draw(_DD);
		}
	}
}
void ObjectsManager::AddWater(DrawData2D* _DD)
{
	for (list<TerrainObject*> ::iterator it = myObjects.begin(); it != myObjects.end(); it++)
	{
		if (((*it)->IsAlive()) & ((*it)->GetType() == 4))
		{
			(*it)->Draw(_DD);
			//(*it)->Kill();
		}
	}
}
void ObjectsManager::Draw(DrawData2D* _DD)
{
}