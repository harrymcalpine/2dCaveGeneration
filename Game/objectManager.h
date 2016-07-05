#ifndef _OBJECTS_MANAGER_H_
#define _OBJECTS_MANAGER_H_
#include "GameObject2D.h"
#include <list>

class TerrainObject;

class ObjectsManager : public GameObject2D
{
public:
	ObjectsManager(ID3D11Device* _GD, int _MaxRows, int _MaxCols);
	//ObjectsManager(ID3D11Device * _GD);
	~ObjectsManager();
	void AddWall(DrawData2D* _DD);
	void AddDeepWall(DrawData2D* _DD);
	void AddFloor(DrawData2D* _DD);
	void AddWater(DrawData2D* _DD);
	virtual void Draw(DrawData2D* _DD);
	void SpawnObject(int _Type, Vector2 _pos);

protected:

	list<TerrainObject*> myObjects;
};
#endif