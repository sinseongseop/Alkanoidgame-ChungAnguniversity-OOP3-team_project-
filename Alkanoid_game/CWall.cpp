#include "CSphere.h"
#include "d3dUtility.h"
#include "CWall.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <cmath>
#include <random>


//랜덤 변수 초기 설정
std::random_device d;
std::mt19937 gen1(d());

#define M_HEIGHT 0.1



CWall::CWall(void)
{
	D3DXMatrixIdentity(&m_mLocal);
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_width = 0;
	m_depth = 0;
	m_pBoundMesh = NULL;
};


bool CWall::create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color)
{
	if (NULL == pDevice)
		return false;

	m_mtrl.Ambient = color;
	m_mtrl.Diffuse = color;
	m_mtrl.Specular = color;
	m_mtrl.Emissive = d3d::BLACK;
	m_mtrl.Power = 5.0f;

	m_width = iwidth;
	m_depth = idepth;
	m_height = iheight;

	if (FAILED(D3DXCreateBox(pDevice, iwidth, iheight, idepth, &m_pBoundMesh, NULL)))
		return false;
	return true;
}

void CWall::setwallnum(int wall_num) {
	this->wall_num = wall_num;
}

float CWall::get_witdh() { // 벽의 가로 길이 얻기
	return m_width;
}

void CWall::destroy(void)
{
	if (m_pBoundMesh != NULL) {
		m_pBoundMesh->Release();
		m_pBoundMesh = NULL;
	}
}
void CWall::draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
	pDevice->SetMaterial(&m_mtrl);
	m_pBoundMesh->DrawSubset(0);
}

// inline 없앰
bool CWall::hasIntersected(CSphere& ball)  // 공이 벽과 충돌하는 경우
{
	if (ball.get_nochangetime() > 0) { // 충돌한지 너무 작은 시간인 경우
		ball.set_nochangetime(ball.get_nochangetime() - 1);
		return false; // 벽끼임 방지를 위해 충돌 안했다고 판단
	}

	switch (this->wall_num) {

	case 0: //왼쪽 벽
		if (m_x + m_width / 2 + 0.1 >= (ball.get_centerx() - ball.getRadius())) {
			return true;
		}
		break;
	case 1:// 위의 벽
		if (m_z - m_depth / 2 - 0.1 <= ball.get_centerz() + ball.getRadius()) {
			return true;
		}
		break;
	case 2: // 오른쪽 벽
		if (m_x - m_width / 2 - 0.1 <= ball.get_centerx() + ball.getRadius()) {
			return true;
		}
		break;

	case 3: // 움직이는 판
		if ((m_z - m_depth / 2 <= ball.get_centerz() && m_z + m_depth / 2 + 0.1 >= ball.get_centerz() - ball.getRadius()) && (ball.get_centerx() + ball.getRadius() > m_x - m_width / 2 - 0.001 && ball.get_centerx() - ball.getRadius() < m_x + m_width / 2 + 0.001)) {
			return true;
		}
		break;

	default:
		break;



	}
	return false;
}

void  CWall::hitBy(CSphere& ball)
{
	if (hasIntersected(ball)) {
		ball.set_nochangetime(10); // 10번 돌때 동안은 변화 무시
		float speedup = 0;
		if (abs(ball.getVelocity_X()) <= 2) { // 공이 너무 느려짐을 방지
			if (ball.getVelocity_X() > 0) {
				float speedup = 2;
			}
			else {
				float speedup = -2;
			}
			ball.setPower(ball.getVelocity_X() + speedup, ball.getVelocity_Z());
		}
		if (abs(ball.getVelocity_Z()) <= 2) {
			if (ball.getVelocity_Z() > 0) {
				float speedup = 2;
			}
			else {
				float speedup = -2;
			}
			ball.setPower(ball.getVelocity_X(), ball.getVelocity_Z() + speedup);

		}
		std::uniform_real_distribution<double> distribution(-1.0, 1.0);
		std::uniform_real_distribution<double> a(-0.5, 0.5);
		double random_number1 = distribution(gen1);
		double random_number2 = a(gen1);


		switch (this->wall_num)
		{
		case 0: //왼쪽 벽
			ball.setPower(-(ball.getVelocity_X() + 0.002), ball.getVelocity_Z());
			break;
		case 1:// 위의 벽
			ball.setPower(ball.getVelocity_X(), -(ball.getVelocity_Z() + 0.002));
			break;
		case 2: // 오른쪽 벽
			ball.setPower(-(ball.getVelocity_X() + 0.002), ball.getVelocity_Z());
			break;
		case 3: // 움직이는 판
			if (abs(ball.getVelocity_X() + random_number2) <= 2) {
				if (ball.getVelocity_X() + random_number2 >= 0) {
					random_number2 += 1;
				}
				else {
					random_number2 -= 1;
				}
			}

			if (abs(ball.getVelocity_Z() + random_number1 <= 2)) {
				if (ball.getVelocity_Z() + random_number1 >= 0) {
					random_number1 += 1;
				}
				else {
					random_number1 -= 1;
				}

			}
			ball.setPower(ball.getVelocity_X() + random_number2, -(ball.getVelocity_Z() + random_number1));
			break;
		default:
			break;
		}

		if (wall_num == 3) {
			ball.setFirstHit(true);
			ball.setPoint(0);
		}
	}

	// Insert your code here.
}

void CWall::setPosition(float x, float y, float z)
{
	D3DXMATRIX m;
	this->m_x = x;
	this->m_z = z;

	D3DXMatrixTranslation(&m, x, y, z);
	setLocalTransform(m);
}

float CWall::getHeight(void) const { return M_HEIGHT; }

D3DXVECTOR3 CWall::getCenter(void) const
{
	D3DXVECTOR3 org(m_x, M_RADIUS, m_z);
	return org;
}

float CWall::get_x() { // 벽의 x축 좌표를 얻음
	return m_x;
}