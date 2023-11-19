#include "CLife.h"
#include "d3dUtility.h"
#include <stdlib.h> 
#include <cstdlib>


// 벽의 가로, 세로, 두께 default 값
const float Wall_height = 10.0f;
const float Wall_width = 7.0f;
const float Wall_thickness = 0.2f;

void exitProgram();
extern int lifeCnt; //남은 목숨 개수 counting
extern int ballCnt; //공 개수 counting

// 전체 포인트
extern int total_point;


CLife::CLife(void)
{
	D3DXMatrixIdentity(&m_mLocal);
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_pSphereMesh = NULL;
}
CLife::~CLife(void) {}


inline float CLife::get_centerx() { //center_x 얻기
	return center_x;
}

inline float CLife::get_centery() { //center_y 얻기
	return center_y;
}

inline float CLife::get_centerz() { //center_z 얻기
	return center_z;
}


float CLife::getRadius(void)  const { return (float)(m_radius); }

bool CLife::create(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice)
		return false;

	D3DXCOLOR color = d3d::YELLOW; //색

	m_mtrl.Ambient = color;
	m_mtrl.Diffuse = color;
	m_mtrl.Specular = color;
	m_mtrl.Emissive = d3d::RED;
	m_mtrl.Power = 5.0f;

	if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
		return false;
	return true;
}

void CLife::destroy(void)
{
	if (m_pSphereMesh != NULL) {
		m_pSphereMesh->Release();
		m_pSphereMesh = NULL;
	}
}

void CLife::draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
	pDevice->SetMaterial(&m_mtrl);
	m_pSphereMesh->DrawSubset(0);
}


void CLife::setCenter(float x, float y, float z)
{
	D3DXMATRIX m;
	center_x = x;	center_y = y;	center_z = z;
	m_radius = y;
	D3DXMatrixTranslation(&m, x, y, z);
	setLocalTransform(m);
}

const D3DXMATRIX& CLife::getLocalTransform(void) const { return m_mLocal; }
void CLife::setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
D3DXVECTOR3 CLife::getCenter(void) const
{
	D3DXVECTOR3 org(center_x, center_y, center_z);
	return org;
}

