////////////////////////////////////////////////////////////////////////////////
//
// File: virtualLego.cpp
//
// Original Author: 박창현 Chang-hyeon Park, 
// Modified by Bong-Soo Sohn and Dong-Jun Kim
// 
// Originally programmed for Virtual LEGO. 
// Modified later to program for Virtual Billiard.
//        
////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <cmath>
#include <random>

//랜덤 변수 초기 설정
std::random_device rd;
std::mt19937 gen(rd());


IDirect3DDevice9* Device = NULL;

// window size
const int Width = 1500;
const int Height = 768;

// There are four balls
// initialize the position (coordinate) of each ball (ball0 ~ ball3)
const float spherePos[10][2] = { {0.5f,-2.0f} , {+2.4f,0} , {1.2f,1} , {-2.0f,-1.5f},{-1.4f,2.2f} ,{2.4f,3.2f} ,{-1.3f,1.2f} ,{1.6f,1.3f} ,{-1.2f,-2.1f} ,{-1.0f,2.1f} };
// initialize the color of each ball (ball0 ~ ball3)
const D3DXCOLOR sphereColor[10] = { d3d::WHITE, d3d::RED, d3d::RED, d3d::RED,d3d::RED ,d3d::RED ,d3d::RED ,d3d::RED ,d3d::BLACK ,d3d::BLACK };

const float extra_whitePos[4][2] = { {1.0f, -2.0f} , {-1.0f, -2.0f}, {0.7f, -1.5f}, {-0.7f, -1.5f}  };

// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------
D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;

#define M_RADIUS 0.21   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.1
#define DECREASE_RATE 1 //공의 감속 관련
#define ITEM_TIME1 2000 // 아이템1 지속 시간 
#define ITEM_TIME4 1000 // 아이템4 지속 시간
#define Max_Whiteball 5// 한 화면에 최대로 존재 가능한 흰공의 수 (1~5 까지 가능) //

// -----------------------------------------------------------------------------
// CSphere class definition
// -----------------------------------------------------------------------------

class CSphere {
private:
	int						no_change_time = 0; // 이값이 양수인 경우 충돌을 무시한다.
	float					center_x, center_y, center_z;
	float                   m_radius;
	float					m_velocity_x;
	float					m_velocity_z;
	bool					sphere_exist = true; // 공이 실제로 존재하는 지. 충돌시 False로 변경
	bool					white_ball = false; //발사되는 공 or 움직이는 공 인가? (충돌 시에도 계속 존재해야함)
	bool					has_item = false; // 이 공을 깨면 아이템이 나오는 가?
	bool					ignore_collision = false;// 공끼리 충돌 할때 공의 진행 방향이 바뀌는가?


public:
	CSphere(void)
	{
		D3DXMatrixIdentity(&m_mLocal);
		ZeroMemory(&m_mtrl, sizeof(m_mtrl));
		m_radius = 0;
		m_velocity_x = 0;
		m_velocity_z = 0;
		m_pSphereMesh = NULL;
	}
	~CSphere(void) {}

public:
	void set_ignore_collision(bool ignore) {
		ignore_collision = ignore;
	}

	inline bool get_ignore_collision() {
		return ignore_collision;
	}

	void set_item(bool has_item) {
		this->has_item = has_item;
	}

	inline bool is_has_item() {
		return has_item;
	}

	int get_nochangetime() { // no_change_time 얻기
		return no_change_time;
	}

	void set_nochangetime(int time) { // no_change_time 값 설정
		no_change_time = time;
	}

	inline float get_centerx() { //center_x 얻기
		return center_x;
	}

	inline float get_centery() { //center_y 얻기
		return center_y;
	}

	inline float get_centerz() { //center_z 얻기
		return center_z;
	}

	void set_exist(bool sphere_exist) { //공이 화면에 존재하는 지 설정
		this->sphere_exist = sphere_exist;
	}

	inline bool get_exist() { // 공이 존재하는 지 정보를 얻는 함수
		return sphere_exist;
	}

	void set_whiteball(bool white_ball) { // 발사되는 공 or 움직이는 공 인지 설정 (발사되는 공은 충돌시 사라지면 안됨)
		this->white_ball = white_ball;
	}

	inline bool get_whiteball() { // 흰공(발사되는 공)이 맞는 지 확인하는 정보
		return white_ball;
	}

	inline bool is_balloutside() { // 공의 정해진 장소 밖으로 나갔는 가?
		if (center_x < -3.2 || center_x>3.2 || center_z > 4.8 || center_z < -4.8) {
			return true;
		}
		return false;
	}

	void ball_position() { // 공이 올바른 위치가 아니라면 위치를 변경하는 함수.
		if (is_balloutside()) {
			setPower(0, 0);
			center_x = spherePos[0][0];
			center_z = spherePos[0][1];

		}
	}


	bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE, float radius = M_RADIUS) // Default값 반지름을 가지면서 원할시 반지름 변경 가능.
	{
		if (NULL == pDevice)
			return false;

		m_mtrl.Ambient = color;
		m_mtrl.Diffuse = color;
		m_mtrl.Specular = color;
		m_mtrl.Emissive = d3d::BLACK;
		m_mtrl.Power = 5.0f;
		m_radius = radius; // 반지름 다른 공 만들기 가능 

		if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
			return false;
		return true;
	}

	void destroy(void)
	{
		if (m_pSphereMesh != NULL) {
			m_pSphereMesh->Release();
			m_pSphereMesh = NULL;
		}
	}

	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
	{
		if (NULL == pDevice)
			return;
		pDevice->SetTransform(D3DTS_WORLD, &mWorld);
		pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
		pDevice->SetMaterial(&m_mtrl);
		m_pSphereMesh->DrawSubset(0);
	}

	inline bool hasIntersected(CSphere& ball)
	{
		if (ball.get_nochangetime() > 0) { // 충돌한지 너무 작은 시간인 경우
			return false; //충돌 안했다고 판단
		}

		float distance_square = pow((ball.center_x - this->center_x), 2) + pow(ball.center_z - this->center_z, 2); // 두 구의 중점 사이의 거리의 제곱
		float distance = abs(pow(distance_square, 0.5)); // 두 구 중점 사이의 거리
		if (distance <= (ball.getRadius() + this->getRadius()) + 0.001) { // 두 구의 반지름의 합보다 거리가 작으면 충돌상태 
			return true;
		}
		else {
			return false;
		}

	}

	void hitBy(CSphere& ball)
	{

		if (hasIntersected(ball)) { // 충돌이 발생 할때
			ball.set_nochangetime(2); // 2번 돌때 동안은 변화 무시


			// 원하는 분포와 범위로 랜덤 실수 생성
			std::uniform_real_distribution<double> distribution(-1.0, 1.0);
			double random_number1 = distribution(gen);
			double random_number2 = distribution(gen);

			float speedup1 = 0;
			float speedup2 = 0;
			float speedup3 = 0;
			float speedup4 = 0;

			if (abs(-ball.getVelocity_X() + random_number1) < 2) { // 너무 느려지는 거 방지용 속도 보정
				if (-ball.getVelocity_X() + random_number1 >= 0) {
					speedup1 = 2;
				}
				else {
					speedup1 = -2;
				}
			}

			if (abs(-ball.getVelocity_Z() + random_number2) < 2) {
				if (-ball.getVelocity_Z() + random_number2 >= 0) {
					speedup2 = 2;
				}
				else {
					speedup2 = -2;
				}
			}

			if (abs(getVelocity_X() + random_number1) < 2) {
				if (getVelocity_X() + random_number1 >= 0) {
					speedup3 = 2;
				}
				else {
					speedup3 = -2;
				}
			}

			if (abs(getVelocity_Z() + random_number2) < 2) {
				if (getVelocity_Z() + random_number2 >= 0) {
					speedup4 = 2;
				}
				else {
					speedup4 = -2;
				}
			}

			if (!ball.get_whiteball()) {
				ball.set_exist(false); // 공을 화면에서 없앤다.
			}
			else {
				if (!ball.ignore_collision) {
					ball.setPower(-ball.getVelocity_X() + random_number1 + speedup1, -ball.getVelocity_Z() + random_number2 + speedup2); // 안 없어지는 공이고 충돌 영향받는 상태면 속도를 업데이트 한다.

				}

			}
			if (!this->get_whiteball()) {
				this->set_exist(false); // 공을 화면에서 없앤다.
			}
			else {
				if (!ball.ignore_collision) {
					this->setPower(-getVelocity_X() + random_number1 + speedup3, -getVelocity_Z() + random_number2 + speedup4);  // 안 없어지는 공이고 충돌 영향받는 상태면  속도를 업데이트 한다.
				}

			}

		}

		// Insert your code here.
	}

	void ballUpdate(float timeDiff)
	{
		const float TIME_SCALE = 3.3;
		D3DXVECTOR3 cord = this->getCenter();
		double vx = abs(this->getVelocity_X());
		double vz = abs(this->getVelocity_Z());

		if (vx > 0.01 || vz > 0.01)
		{
			float tX = cord.x + TIME_SCALE * timeDiff * m_velocity_x;
			float tZ = cord.z + TIME_SCALE * timeDiff * m_velocity_z;

			//correction of position of ball
			// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
			/*if(tX >= (4.5 - M_RADIUS))
				tX = 4.5 - M_RADIUS;
			else if(tX <=(-4.5 + M_RADIUS))
				tX = -4.5 + M_RADIUS;
			else if(tZ <= (-3 + M_RADIUS))
				tZ = -3 + M_RADIUS;
			else if(tZ >= (3 - M_RADIUS))
				tZ = 3 - M_RADIUS;*/

			this->setCenter(tX, cord.y, tZ);
		}
		else { this->setPower(0, 0); }
		//this->setPower(this->getVelocity_X() * DECREASE_RATE, this->getVelocity_Z() * DECREASE_RATE);
		double rate = 1 - (1 - DECREASE_RATE) * timeDiff * 400;
		if (rate < 0)
			rate = 0;
		this->setPower(getVelocity_X() * rate, getVelocity_Z() * rate);
	}

	inline double getVelocity_X() { return this->m_velocity_x; }
	inline double getVelocity_Z() { return this->m_velocity_z; }

	void setPower(double vx, double vz)
	{
		if (vx > 4) { // 최대 속도 제한
			this->m_velocity_x = 4;
		}
		else if (vx < -4) {
			this->m_velocity_x = -4;
		}
		else {
			this->m_velocity_x = vx;
		}

		if (vz > 4) { // 최대 속도 제한
			this->m_velocity_z = 4;
		}
		else if (vz < -4) {
			this->m_velocity_z = -4;
		}
		else {
			this->m_velocity_z = vz;
		}

	}

	void setCenter(float x, float y, float z)
	{
		D3DXMATRIX m;
		center_x = x;	center_y = y;	center_z = z;
		m_radius = y;
		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}

	float getRadius(void)  const { return (float)(m_radius); } // 반지름 다른거 존재 가능하므로 반지름으로 수정
	const D3DXMATRIX& getLocalTransform(void) const { return m_mLocal; }
	void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
	D3DXVECTOR3 getCenter(void) const
	{
		D3DXVECTOR3 org(center_x, center_y, center_z);
		return org;
	}

private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;

};



// -----------------------------------------------------------------------------
// CWall class definition
// -----------------------------------------------------------------------------

class CWall {

private:

	float					m_x;
	float					m_z;
	float                   m_width;
	float                   m_depth;
	float					m_height;
	int						wall_num; // 각 벽마다 고유의 번호를 부여

public:
	CWall(void)
	{
		D3DXMatrixIdentity(&m_mLocal);
		ZeroMemory(&m_mtrl, sizeof(m_mtrl));
		m_width = 0;
		m_depth = 0;
		m_pBoundMesh = NULL;
	}
	~CWall(void) {}
public:
	bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color = d3d::WHITE)
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

	inline void setwallnum(int wall_num) {
		this->wall_num = wall_num;
	}

	inline float get_witdh() { // 벽의 가로 길이 얻기
		return this->m_width;
	}

	void destroy(void)
	{
		if (m_pBoundMesh != NULL) {
			m_pBoundMesh->Release();
			m_pBoundMesh = NULL;
		}
	}
	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
	{
		if (NULL == pDevice)
			return;
		pDevice->SetTransform(D3DTS_WORLD, &mWorld);
		pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
		pDevice->SetMaterial(&m_mtrl);
		m_pBoundMesh->DrawSubset(0);
	}

	inline bool hasIntersected(CSphere& ball)  // 공이 벽과 충돌하는 경우
	{
		if (ball.get_nochangetime() > 0) { // 충돌한지 너무 작은 시간인 경우
			ball.set_nochangetime(ball.get_nochangetime() - 1);
			return false; // 벽끼임 방지를 위해 충돌 안했다고 판단
		}

		switch (this->wall_num) {

		case 0: //왼쪽 벽
			if (m_x + m_width / 2 + 0.001 >= (ball.get_centerx() - ball.getRadius())) {
				return true;
			}
			break;
		case 1:// 위의 벽
			if (m_z - m_depth / 2 - 0.001 <= ball.get_centerz() + ball.getRadius()) {
				return true;
			}
			break;
		case 2: // 오른쪽 벽
			if (m_x - m_width / 2 - 0.001 <= ball.get_centerx() + ball.getRadius()) {
				return true;
			}
			break;

		case 3: // 움직이는 판
			if ((m_z - m_depth / 2 <= ball.get_centerz() && m_z + m_depth / 2 + 0.001 >= ball.get_centerz() - ball.getRadius()) && (ball.get_centerx() + ball.getRadius() > m_x - m_width / 2 - 0.001 && ball.get_centerx() - ball.getRadius() < m_x + m_width / 2 + 0.001)) {
				return true;
			}
			break;

		default:
			break;



		}
		return false;
	}

	void hitBy(CSphere& ball)
	{
		if (hasIntersected(ball)) {
			ball.set_nochangetime(10); // 5번 돌때 동안은 변화 무시
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
			double random_number1 = distribution(gen);
			double random_number2 = a(gen);


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
		}

		// Insert your code here.
	}

	void setPosition(float x, float y, float z)
	{
		D3DXMATRIX m;
		this->m_x = x;
		this->m_z = z;

		D3DXMatrixTranslation(&m, x, y, z);
		setLocalTransform(m);
	}

	float getHeight(void) const { return M_HEIGHT; }

	D3DXVECTOR3 getCenter(void) const
	{
		D3DXVECTOR3 org(m_x, M_RADIUS, m_z);
		return org;
	}

	float get_x() { // 벽의 x축 좌표를 얻음
		return m_x;
	}

private:
	void setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }

	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pBoundMesh;
};

// -----------------------------------------------------------------------------
// CLight class definition
// -----------------------------------------------------------------------------

class CLight {
public:
	CLight(void)
	{
		static DWORD i = 0;
		m_index = i++;
		D3DXMatrixIdentity(&m_mLocal);
		::ZeroMemory(&m_lit, sizeof(m_lit));
		m_pMesh = NULL;
		m_bound._center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_bound._radius = 0.0f;
	}
	~CLight(void) {}
public:
	bool create(IDirect3DDevice9* pDevice, const D3DLIGHT9& lit, float radius = 0.1f)
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
	void destroy(void)
	{
		if (m_pMesh != NULL) {
			m_pMesh->Release();
			m_pMesh = NULL;
		}
	}
	bool setLight(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
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

	void draw(IDirect3DDevice9* pDevice)
	{
		if (NULL == pDevice)
			return;
		D3DXMATRIX m;
		D3DXMatrixTranslation(&m, m_lit.Position.x, m_lit.Position.y, m_lit.Position.z);
		pDevice->SetTransform(D3DTS_WORLD, &m);
		pDevice->SetMaterial(&d3d::WHITE_MTRL);
		m_pMesh->DrawSubset(0);
	}

	D3DXVECTOR3 getPosition(void) const { return D3DXVECTOR3(m_lit.Position); }

private:
	DWORD               m_index;
	D3DXMATRIX          m_mLocal;
	D3DLIGHT9           m_lit;
	ID3DXMesh* m_pMesh;
	d3d::BoundingSphere m_bound;
};

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
CWall	g_legoPlane;
CWall	g_legowall[4];
CSphere	g_sphere[10];
CSphere	g_target_blueball;
CSphere extra_whiteball[5]; // 추가 흰 공
CLight	g_light;
CWall move_board; // 움직이는 판
int ball_count = 1;

//아이템 클래스
class Item // I는 대문자 i 임(item)
{
public:
	int choose_random_item() { //4가지 아이템중 랜덤으로 얻을 아이템을 정하는 함수
		//return 2; // 디버깅 용 코드. 원하는 아이템 번호 삽입
		std::uniform_int_distribution<int> distribution(1, 4); // 4가지 아이템이 존재
		int random_number = distribution(gen);
		return random_number; // 정해진 랜덤 숫자(정수) 반환
	}

	void longer_board() { // 일정시간 동안 움직이는 판의 길이를 증가시킨다.
		set_is_use_item1(true);
		item_time[0] += ITEM_TIME1;
		float position_x = move_board.get_x(); // 기존 움직이는 판의 x 좌표를 얻는다.
		move_board.destroy();
		move_board.create(Device, -1, -1, 2, 0.4f, 0.15f, d3d::CYAN);
		move_board.setPosition(position_x, 0.12f, -4.2f);
	}

	void original_board() { //일정시간 후 움직이는 판의 길이를 원래대로 돌린다.
		set_is_use_item1(false);
		float position_x = move_board.get_x(); // 기존 움직이는 판의 x 좌표를 얻는다.
		move_board.destroy();
		move_board.create(Device, -1, -1, 1, 0.4f, 0.15f, d3d::CYAN);
		move_board.setPosition(position_x, 0.12f, -4.2f);
	}

	void plus_whiteball() { // 추가 공을 생성한다.
		if (ball_count < Max_Whiteball) {
			if (!g_sphere[0].get_exist()) {
				g_sphere[0].set_exist(true); //눈에 보이게 함
				g_sphere[0].setPower(1.3, -1.3);
				ball_count += 1;
				return;
			}
			for (int i = 0; i < Max_Whiteball - 1; i++) {
				if (!extra_whiteball[i].get_exist()) {
					extra_whiteball[i].set_exist(true); //눈에 보이게 함
					extra_whiteball[i].setPower(1.3, -1.3);
					ball_count += 1;
					return;
				}
			}
		}
	}

	void plus_life() { // 생명을 1 추가 한다.
		//life+=1; //그냥 생명 추가 1...
	}

	void ignore_collision() { //일정 시간동안 공이 직선으로 날아가게 한다.
		set_is_use_item4(true);
		item_time[3] += ITEM_TIME4;
		for (int i = 0; i < 10; i++) {
			g_sphere[i].set_ignore_collision(true); // 충돌 무시 속성을 true로 다 바꿔준다
		}
	}

	void original_collsision() { // 공의 충돌 판정을 원래 상태로 돌린다.
		set_is_use_item4(false);
		for (int i = 0; i < 10; i++) {
			g_sphere[i].set_ignore_collision(false);// 충돌 무시 속성을 false로 다 바꿔준다
		}
	}

	void use_item() { // 아이템 사용 함수
		int item_num = choose_random_item(); // 랜덤 아이템 1개를 얻는다.
		switch (item_num)
		{
		case 1:
			longer_board(); // 움직이는 판의 길이를 증가시킨다.
			break;
		case 2:
			plus_whiteball(); // 추가 공을 생성한다.
			break;
		case 3:
			plus_life(); //목숨 하나 추가
			break;
		case 4:
			ignore_collision(); // 공끼리의 충돌을 무시하고 직선으로 날아간다.
			break;
		default:
			break;
		}
	}

	inline void set_item_time(int time, int item_num) { // item_num의 남은 지속시간을 time으로 설정한다.
		item_time[item_num] = time;
	}

	inline int get_item_time(int item_num) { //item_num의 남은 지속시간을 얻는다.
		return item_time[item_num];
	}

	inline bool get_is_use_item1() {
		return is_use_item1;
	}

	inline bool get_is_use_item4() {
		return is_use_item4;
	}

	inline void set_is_use_item1(bool use) {
		is_use_item1 = use;
	}

	inline void set_is_use_item4(bool use) {
		is_use_item4 = use;
	}

private:
	int item_time[4] = { 0,0,0,0 }; // 아이템 번호별 남은 유지 시간( item1,item4한테만 유의미, 규칙성 때문에 4로 설정)
	bool is_use_item1 = false;
	bool is_use_item4 = false;
};


Item special_item;// 아이템 객체 

double g_camera_pos[3] = { 0.0, 0.0, -0.0 };

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------


void destroyAllLegoBlock(void)
{
}

// initialization
bool Setup()
{
	int i;

	D3DXMatrixIdentity(&g_mWorld);
	D3DXMatrixIdentity(&g_mView);
	D3DXMatrixIdentity(&g_mProj);

	// create plane and set the position
	if (false == g_legoPlane.create(Device, -1, -1, 6, 0.2f, 9, d3d::GREEN)) return false; //Device, ? , ?,  가로, 두께, 높이, 색깔
	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);

	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, 0.3f, 0.3f, 9.0f, d3d::BLACK)) return false; //왼쪽벽
	g_legowall[0].setPosition(-3.1f, 0.12f, 0.0f);

	if (false == g_legowall[1].create(Device, -1, -1, 6.6, 0.3f, 0.3f, d3d::BLACK)) return false; //위쪽 벽
	g_legowall[1].setPosition(0.0f, 0.12f, 4.5f);

	if (false == g_legowall[2].create(Device, -1, -1, 0.3f, 0.3f, 9.0f, d3d::BLACK)) return false; // 오른쪽 벽
	g_legowall[2].setPosition(3.1f, 0.12f, 0.0f);

	if (false == move_board.create(Device, -1, -1, 1, 0.4f, 0.15f, d3d::CYAN)) return false;  //	움직이는 판
	move_board.setPosition(0.0f, 0.12f, -4.2f);

	for (int i = 0; i < 3; i++) {
		g_legowall[i].setwallnum(i);
	}

	move_board.setwallnum(3); // 움직이는 보드 번호(3) 설정

	// create balls and set the position
	for (i = 0; i < 10; i++) {
		if (false == g_sphere[i].create(Device, sphereColor[i])) return false;
		g_sphere[i].setCenter(spherePos[i][0], (float)g_sphere[i].getRadius(), spherePos[i][1]);
		g_sphere[i].setPower(0, 0);
	}

	for (i = 0; i < Max_Whiteball - 1; i++) { //*** 추가 흰공 초기화***
		if (false == extra_whiteball[i].create(Device, sphereColor[0])) return false;
		extra_whiteball[i].setCenter(extra_whitePos[i][0], (float)extra_whiteball[i].getRadius(), extra_whitePos[i][1]);
		extra_whiteball[i].setPower(0, 0);
		extra_whiteball[i].set_whiteball(true); //***추가공***
		extra_whiteball[i].set_exist(false); //***처음에는 추가공 안 보이게 설정***
	}


	/*for (int i = 1; i<10; i++) { // 디버깅 확인용 모든 공에 아이템 속성 부여
		g_sphere[i].set_item(true);
	}*/
	

	g_sphere[8].set_item(true);
	g_sphere[9].set_item(true);

	// create blue ball for set direction
	//if (false == g_target_blueball.create(Device, d3d::BLUE)) return false; // 수정) 이 공이 마우스 좌클릭으로 움직이는 공임. 원하는 색깔 변경 가능.(기존 파란색)
	//g_target_blueball.setCenter(-0.0f, (float)g_target_blueball.getRadius(), -4.2f); // 움직이는 공의 초기 좌표 설정. 첫번째게 x축, 세번째게 z축 위치

	g_sphere[0].set_whiteball(true); // 충돌에도 안 없어지게 하기 위한 설정
	//g_target_blueball.set_whiteball(true); // 충돌에도 안 없어지게 하기 위한 설정


	// light setting 
	D3DLIGHT9 lit;
	::ZeroMemory(&lit, sizeof(lit));
	lit.Type = D3DLIGHT_POINT;
	lit.Diffuse = d3d::WHITE;
	lit.Specular = d3d::WHITE * 0.9f;
	lit.Ambient = d3d::WHITE * 0.9f;
	lit.Position = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	lit.Range = 100.0f;
	lit.Attenuation0 = 0.0f;
	lit.Attenuation1 = 0.9f;
	lit.Attenuation2 = 0.0f;
	if (false == g_light.create(Device, lit))
		return false;

	// Position and aim the camera.
	D3DXVECTOR3 pos(0.0f, 15.0f, -2.0f); // 초기 카메라 거리 조정
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f); // 카메라 중점 설정
	D3DXVECTOR3 up(0.0f, 4.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_mView, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &g_mView);

	// Set the projection matrix.
	D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI / 4,
		(float)Width / (float)Height, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &g_mProj);

	// Set render states.
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	g_light.setLight(Device, g_mWorld);
	return true;
}

void Cleanup(void)
{
	g_legoPlane.destroy();
	for (int i = 0; i < 3; i++) {
		g_legowall[i].destroy();
	}
	destroyAllLegoBlock();
	g_light.destroy();
}


// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	int i = 0;
	int j = 0;

	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();

		// update the position of each ball. during update, check whether each ball hit by walls.
		for (i = 0; i < 10; i++) {
			for (j = 0; j < 3; j++) {
				g_legowall[j].hitBy(g_sphere[i]);
				for (int k = 0; k < Max_Whiteball - 1; k++) {
					if (extra_whiteball[k].get_exist()) {
						g_legowall[j].hitBy(extra_whiteball[k]);
					}
				}
			}
			g_sphere[i].ballUpdate(timeDelta);

			move_board.hitBy(g_sphere[i]);
		}

		for (i = 0; i < Max_Whiteball - 1; i++) {
			extra_whiteball[i].ballUpdate(timeDelta);
			move_board.hitBy(extra_whiteball[i]);
		}

		// check whether any two balls hit together and update the direction of balls
		for (i = 1; i < 10; i++) {
			if (g_sphere[i].get_exist()) { // i 공이  실재로 존재하면 i공과 움직이는 공의 충돌 확인
				if (g_sphere[0].get_exist()) {
					g_sphere[0].hitBy(g_sphere[i]);
				}

				for (j = 0; j < Max_Whiteball - 1; j++) { // 추가 흰공이 존재하면 충돌 확인
					if (extra_whiteball[j].get_exist()) {
						extra_whiteball[j].hitBy(g_sphere[i]);
					}

				}
			}
			else { // 공이 존재하지 않으면 이 공이 아이템을 가지고 있는 지 확인한다.
				if (g_sphere[i].is_has_item()) {
					special_item.use_item();
					g_sphere[i].set_item(false); // 아이템을 먹었으므로 공의 item을 false 처리 해준다.
				}

			}
		}

		if (special_item.get_item_time(0) > 0) { // 아이템 1 지속시간 업데이트
			special_item.set_item_time((special_item.get_item_time(0) - 1), 0);
		}
		else if (special_item.get_is_use_item1()) {
			special_item.original_board(); // 아이템 1의 지속시간이 끝나면 판을 원래 길이로 되돌린다.
		}

		if (special_item.get_item_time(3) > 0) {  // 아이템 4 지속시간 업데이트
			special_item.set_item_time((special_item.get_item_time(3) - 1), 3);
		}
		else if (special_item.get_is_use_item4()) {
			special_item.original_collsision(); // 아이템 4의 지속시간이 끝나면 공을 원래 상태로 되돌린다.
		}

		//g_target_blueball.hitBy(g_sphere[0]); // 파란공과 흰공 충돌 확인

		if (g_sphere[0].is_balloutside()) { //움직이는 공이 규격장 밖으로 나가면 공의 초기화 시킴.
			g_sphere[0].destroy();
			if (false == g_sphere[0].create(Device, sphereColor[0])) return false;
			g_sphere[0].setCenter(spherePos[0][0], (float)g_sphere[0].getRadius(), spherePos[0][1]);
			g_sphere[0].setPower(0, 0);
			g_sphere[0].set_exist(false);
			ball_count -= 1;
		}

		for (int i = 0; i < Max_Whiteball - 1; i++) {
			if (extra_whiteball[i].is_balloutside()) { //움직이는 공이 규격장 밖으로 나가면 공의 초기화 시킴.
				extra_whiteball[i].destroy();
				if (false == extra_whiteball[i].create(Device, sphereColor[0])) return false;
				extra_whiteball[i].setCenter(extra_whitePos[i][0], (float)extra_whiteball[i].getRadius(), extra_whitePos[i][1]);
				extra_whiteball[i].setPower(0, 0);
				extra_whiteball[i].set_exist(false);
				ball_count -= 1;
			}

		}

		if (ball_count == 0) {
			ball_count = 1;
			g_sphere[0].set_exist(true);
			//life-=1;			// 생명을 1 깍는다.
		}


		//g_sphere[0].ball_position(); // 움직이는 공이 규격장 밖으로 나갔는 지 확인 

		// draw plane, walls, and spheres
		g_legoPlane.draw(Device, g_mWorld);
		for (i = 0; i < 3; i++) {
			g_legowall[i].draw(Device, g_mWorld);
		}

		move_board.draw(Device, g_mWorld);

		for (i = 0; i < 10; i++) {
			if (g_sphere[i].get_exist()) {
				g_sphere[i].draw(Device, g_mWorld);
			}
		}

		for (i = 0; i < Max_Whiteball - 1; i++) {
			if (extra_whiteball[i].get_exist()) {
				extra_whiteball[i].draw(Device, g_mWorld);
			}

		}
		//g_target_blueball.draw(Device, g_mWorld);
	   // g_light.draw(Device); // 빛 위치 끄기

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture(0, NULL);
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool wire = false;
	static bool isReset = true;
	static int old_x = 0;
	static int old_y = 0;
	static enum { WORLD_MOVE, LIGHT_MOVE, BLOCK_MOVE } move = WORLD_MOVE;

	switch (msg) {
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		break;
	}
	case WM_KEYDOWN:
	{
		switch (wParam) {
		case VK_ESCAPE:
			::DestroyWindow(hwnd);
			break;
		case VK_RETURN:
			if (NULL != Device) {
				wire = !wire;
				Device->SetRenderState(D3DRS_FILLMODE,
					(wire ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
			}
			break;
		case VK_SPACE:

			D3DXVECTOR3 targetpos = move_board.getCenter(); // 이부분을 수정하면 공의 목적지가 변형 됩니다.
			D3DXVECTOR3	whitepos = g_sphere[0].getCenter(); // 이부분을 수정하면 발사되는 공이 수정 됩니다.
			double theta = acos(sqrt(pow(targetpos.x - whitepos.x, 2)) / sqrt(pow(targetpos.x - whitepos.x, 2) +
				pow(targetpos.z - whitepos.z, 2)));		// 기본 1 사분면
			if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x >= 0) { theta = -theta; }	//4 사분면
			if (targetpos.z - whitepos.z >= 0 && targetpos.x - whitepos.x <= 0) { theta = PI - theta; } //2 사분면
			if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x <= 0) { theta = PI + theta; } // 3 사분면
			double distance = sqrt(pow(targetpos.x - whitepos.x, 2) + pow(targetpos.z - whitepos.z, 2));
			g_sphere[0].setPower(distance * cos(theta), distance * sin(theta)); // 앞의 객체를 발사하는 공으로 변경하면 됩니다.

			break;
		}
		break;
	}

	case WM_MOUSEMOVE:
	{
		int new_x = LOWORD(lParam);
		int new_y = HIWORD(lParam);
		float dx;
		float dy;

		if (LOWORD(wParam) & MK_LBUTTON) {

			if (isReset) {
				isReset = false;
			}
			else {
				D3DXVECTOR3 vDist;
				D3DXVECTOR3 vTrans;
				D3DXMATRIX mTrans;
				D3DXMATRIX mX;
				D3DXMATRIX mY;

				switch (move) {
				case WORLD_MOVE:
					dx = (old_x - new_x) * 0.01f;
					dy = (old_y - new_y) * 0.01f;
					//D3DXMatrixRotationY(&mX, dx); //  수정 플래이트 판은 좌우로는 회전 불가능
					D3DXMatrixRotationX(&mY, dy);
					g_mWorld = g_mWorld * 1 * mY; // 수정 플래이트 판은 좌우로는 회전 불가능 하게

					break;
				}
			}

			old_x = new_x;
			old_y = new_y;

		}
		else {
			isReset = true;

			if (LOWORD(wParam) & MK_RBUTTON) { // 이부분을 수정하여 움직이는 판 이 오직 좌우 방향으로만 움직이게 만들기 가능.
				dx = (old_x - new_x);// * 0.01f;
				dy = 0; // 0.01f;  // y방향으로는 움직게 하는 것 불가능. 

				D3DXVECTOR3 coord3d = move_board.getCenter();
				if (-3.0 + move_board.get_witdh() / 2 + 0.1 <= coord3d.x + dx * (-0.007f) && coord3d.x + dx * (-0.007f) <= 3.0 - move_board.get_witdh() / 2 - 0.1) {
					//g_target_blueball.setCenter(coord3d.x + dx * (-0.007f), coord3d.y, coord3d.z + dy * 0.007f);
					move_board.setPosition(coord3d.x + dx * (-0.007f), coord3d.y, coord3d.z + dy * 0.007f);
				}
				else { //움직이는 판 이 움직일 수 있는 x축 범위를 제한
					if (coord3d.x + dx * (-0.007f) < -3.0 + move_board.get_witdh() / 2 + 0.1) {
						//g_target_blueball.setCenter(-2.8, coord3d.y, coord3d.z + dy * 0.007f);
						move_board.setPosition(-3.0 + move_board.get_witdh() / 2 + 0.1, coord3d.y, coord3d.z + dy * 0.007f);
					}
					else {
						//g_target_blueball.setCenter(2.8, coord3d.y, coord3d.z + dy * 0.007f);
						move_board.setPosition(3.0 - move_board.get_witdh() / 2 - 0.1, coord3d.y, coord3d.z + dy * 0.007f);
					}

				}


			}
			old_x = new_x;
			old_y = new_y;

			move = WORLD_MOVE;
		}
		break;
	}
	}

	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	srand(static_cast<unsigned int>(time(NULL)));

	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}