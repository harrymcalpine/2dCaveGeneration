#include "VBPlane.h"

void VBPlane::init(int _size, ID3D11Device* _pd3dDevice, ID3D11ShaderResourceView* _textureSRV)
{
	m_size = _size;

	//calculate number of vertices and primatives
	int numVerts = 6 * (m_size - 1) * (m_size - 1);
	m_numPrims = numVerts / 3;
	m_vertices = new myVertex[numVerts];
	WORD* indices = new WORD[numVerts];

	//as using the standard VB shader set the tex-coords somewhere safe
	for (int i = 0; i < numVerts; i++)
	{
		indices[i] = i;
		m_vertices[i].texCoord = Vector2::One;
	}

	//Set the texture as the SRV - usually the planes render target
	m_pTextureRV = _textureSRV;

	//in each loop create the two traingles for the matching sub-square on each of the six faces
	int vert = 0;

	//Set the width and height (based on the ratio of the map size 51:16)
	m_height = 40;
	m_width = 50;

	//Half of size, width and height
	int m_sizeH = (m_size - 1) / 2;
	int m_widthH = m_width / 2;
	int m_heightH = m_height / 2;

	//First Triangle
	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(0.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(-m_width, -m_height, m_sizeH);

	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(1.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(m_width, -m_height, m_sizeH);

	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(0.0f, 0.0f);
	m_vertices[vert++].Pos = Vector3(-m_width, m_height, m_sizeH);

	//Second Triangle
	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(1.0f, 1.0f);
	m_vertices[vert++].Pos = Vector3(m_width, -m_height, m_sizeH);

	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(1.0f, 0.0f);
	m_vertices[vert++].Pos = Vector3(m_width, m_height, m_sizeH);

	m_vertices[vert].Color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_vertices[vert].texCoord = Vector2(0.0f, 0.0f);
	m_vertices[vert++].Pos = Vector3(-m_width, m_height, m_sizeH);

	//carry out some kind of transform on these vertices to make this object more interesting
	Transform();

	//calculate the normals for the basic lighting in the base shader
	for (int i = 0; i < m_numPrims; i++)
	{
		WORD V1 = 3 * i;
		WORD V2 = 3 * i + 1;
		WORD V3 = 3 * i + 2;

		//build normals
		Vector3 norm;
		Vector3 vec1 = m_vertices[V1].Pos - m_vertices[V2].Pos;
		Vector3 vec2 = m_vertices[V3].Pos - m_vertices[V2].Pos;
		norm = vec1.Cross(vec2);
		norm.Normalize();

		m_vertices[V1].Norm = norm;
		m_vertices[V2].Norm = norm;
		m_vertices[V3].Norm = norm;
	}

	BuildIB(_pd3dDevice, indices);
	BuildVB(_pd3dDevice, numVerts, m_vertices);

	delete[] m_vertices; //this is no longer needed as this is now in the Vertex Buffer
}