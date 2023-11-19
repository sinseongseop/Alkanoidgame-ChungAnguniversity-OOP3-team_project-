#pragma once
#ifndef CLife_H
#define CLife_H

#include "d3dUtility.h"
#include <vector>

class CLife {
private:
	float					center_x, center_y, center_z;
	float                   m_radius = 0.15f;

private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;

public:
	CLife(void);
	~CLife(void);

public:
	// inline ¾ø¾Ú
	float get_centerx();
	// inline ¾ø¾Ú
	float get_centery();
	// inline ¾ø¾Ú
	float get_centerz();

	float getRadius(void)  const;
	bool create(IDirect3DDevice9* pDevice);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);
	void setCenter(float x, float y, float z);
	const D3DXMATRIX& getLocalTransform(void) const;
	void setLocalTransform(const D3DXMATRIX& mLocal);
	D3DXVECTOR3 getCenter(void) const;

};

#endif
