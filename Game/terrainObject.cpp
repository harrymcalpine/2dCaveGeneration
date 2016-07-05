#include "terrainObject.h"

TerrainObject::TerrainObject(string _fileName, ID3D11Device* _GD, int _Type) : ImageGO2D(_fileName, _GD)
{
	m_type = _Type;
}

TerrainObject::~TerrainObject()
{
}

void TerrainObject::Spawn(Vector2 _pos)
{
	m_pos = _pos;
	m_alive = true;
}