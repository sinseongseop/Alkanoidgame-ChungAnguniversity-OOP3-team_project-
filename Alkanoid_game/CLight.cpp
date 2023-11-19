#include "CLight.h"
#include "d3dUtility.h"
#include <stdlib.h> 
#include <cstdlib>
#include <random>

#define M_RADIUS 0.21   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.1
#define DECREASE_RATE 1 //공의 감속 관련
#define Max_Whiteball 5// 한 화면에 최대로 존재 가능한 흰공의 수 (1~5 까지 가능) //
#define Max_life 5 // 가질 수 있는 생명의 최댓값(1~5까지 가능)

// 벽의 가로, 세로, 두께 default 값
const float Wall_height = 10.0f;
const float Wall_width = 7.0f;
const float Wall_thickness = 0.2f;

void exitProgram();
extern int lifeCnt; //남은 목숨 개수 counting
extern int ballCnt; //공 개수 counting

// 전체 포인트
extern int total_point;


CLight::CLight(void)
{
	static DWORD i = 0;
	m_index = i++;
	D3DXMatrixIdentity(&m_mLocal);
	::ZeroMemory(&m_lit, sizeof(m_lit));
	m_pMesh = NULL;
	m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_bound._radius = 0.0f;
}

CLight::~CLight(void) {}

bool CLight::create(IDirect3DDevice9* pDevice, const D3DLIGHT9& lit, float radius)
{
	if (NULL == pDevice)
		return false;
	if (FAILED(D3DXCreateSphere(pDevice, radius, 10, 10, &m_pMesh, NULL)))
		return false;

	m_bound._center = lit.Position;
	m_bound._radius = radius;

	m_lit.Type = lit.Type;
	m_lit.Diffuse = lit.Diffuse;
	m_lit.Specular = lit.Specular;
	m_lit.Ambient = lit.Ambient;
	m_lit.Position = lit.Position;
	m_lit.Direction = lit.Direction;
	m_lit.Range = lit.Range;
	m_lit.Falloff = lit.Falloff;
	m_lit.Attenuation0 = lit.Attenuation0;
	m_lit.Attenuation1 = lit.Attenuation1;
	m_lit.Attenuation2 = lit.Attenuation2;
	m_lit.Theta = lit.Theta;
	m_lit.Phi = lit.Phi;
	return true;
}
void CLight::destroy(void)
{
	if (m_pMesh != NULL) {
		m_pMesh->Release();
		m_pMesh = NULL;
	}
}
bool CLight::setLight(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return false;

	D3DXVECTOR3 pos(m_bound._center);
	D3DXVec3TransformCoord(&pos, &pos, &m_mLocal);
	D3DXVec3TransformCoord(&pos, &pos, &mWorld);
	m_lit.Position = pos;

	pDevice->SetLight(m_index, &m_lit);
	pDevice->LightEnable(m_index, TRUE);
	return true;
}

void CLight::draw(IDirect3DDevice9* pDevice)
{
	if (NULL == pDevice)
		return;
	D3DXMATRIX m;
	D3DXMatrixTranslation(&m, m_lit.Position.x, m_lit.Position.y, m_lit.Position.z);
	pDevice->SetTransform(D3DTS_WORLD, &m);
	pDevice->SetMaterial(&d3d::WHITE_MTRL);
	m_pMesh->DrawSubset(0);
}

D3DXVECTOR3 CLight::getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }
