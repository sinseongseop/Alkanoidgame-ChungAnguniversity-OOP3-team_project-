#pragma once
#pragma once

#ifndef _CWall_
#define _CWall_

#include "CSphere.h"
#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <cmath>
#include <random>


class CWall {

private:

	float					m_x;
	float					m_z;
	float                   m_width;
	float                   m_depth;
	float					m_height;
	int						wall_num; // 각 벽마다 고유의 번호를 부여

public:
	CWall(void);
	~CWall(void) {};
public:
	bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color = d3d::WHITE);

	void setwallnum(int);

	float get_witdh(); // 벽의 가로 길이 얻기


	void destroy(void);

	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);

	// inline없앰
	bool hasIntersected(CSphere& ball);  // 공이 벽과 충돌하는 경우


	void hitBy(CSphere& ball);

	void setPosition(float x, float y, float z);

	float getHeight(void) const;

	D3DXVECTOR3 getCenter(void) const;


	float get_x(); //벽의 x축 좌표를 얻음

private:
	void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }

	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pBoundMesh;
};


#endif