#ifndef _VB_plane_H_
#define _VB_plane_H_
#include "VBGO.h"
#include "vertex.h"

class VBPlane : public VBGO
{
public:
	VBPlane() {};
	virtual ~VBPlane() {};

	//initialise the Veretx and Index buffers for the cube
	void init(int _size, ID3D11Device* _GD, ID3D11ShaderResourceView* _texture);

protected:
	//this is to allow custom versions of this which create the basic cube and then distort it
	//see VBSpiral, VBSpiked and VBPillow
	virtual void Transform() {};

	int m_size;
	int m_height, m_width;
	myVertex* m_vertices;
};

#endif