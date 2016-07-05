#ifndef _TERRAIN_OBJECT_
#define _TERRAIN_OBJECT_
#include "ImageGO2D.h"

class TerrainObject : public ImageGO2D
{
public:
	TerrainObject(string _fileName, ID3D11Device* _GD, int _Type);
	~TerrainObject();

	bool IsAlive() { return m_alive; }
	int GetType() { return m_type; }
	void Spawn(Vector2 _pos);
	void Kill() { m_alive = false; }

protected:
	bool m_alive = false;
	int m_type;		//1 = brick, 2 = explosion, 3 = tunnel
};
#endif