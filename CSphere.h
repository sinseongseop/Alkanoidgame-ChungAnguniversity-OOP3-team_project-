#pragma once
#ifndef _CSphere_
#define _CSphere_
#define M_RADIUS 0.21   // ball radius


#include "d3dUtility.h"
#include <vector>

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
	CSphere(void);
	~CSphere(void) {};

public:
	void set_ignore_collision(bool ignore);

	inline bool get_ignore_collision();

	void set_item(bool has_item);

	bool is_has_item();

	int get_nochangetime(); // no_change_time 얻기	

	void set_nochangetime(int ); // no_change_time 값 설정


	float get_centerx(); //center_x 얻기
	
	float get_centery(); //center_y 얻기

	float get_centerz(); //center_z 얻기

	void set_exist(bool ); //공이 화면에 존재하는 지 설정

	bool get_exist(); // 공이 존재하는 지 정보를 얻는 함수


	void set_whiteball(bool ); // 발사되는 공 or 움직이는 공 인지 설정 (발사되는 공은 충돌시 사라지면 안됨)


	inline bool get_whiteball(); // 흰공(발사되는 공)이 맞는 지 확인하는 정보


	bool is_balloutside(bool& );// 공의 정해진 장소 밖으로 나갔는 가?

	//void ball_position(); // 공이 올바른 위치가 아니라면 위치를 변경하는 함수.



	bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE, float radius = M_RADIUS) ;// Default값 반지름을 가지면서 원할시 반지름 변경 가능.


	void destroy(void);

	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);

	bool hasIntersected(CSphere& ball);

	void hitBy(CSphere& ball);

	void ballUpdate(float timeDiff);
	

	inline double getVelocity_X();
	inline double getVelocity_Z();

	void setPower(double vx, double vz);


	void setCenter(float x, float y, float z);
	
	float getRadius(void)  const; // 반지름 다른거 존재 가능하므로 반지름으로 수정
	const D3DXMATRIX& getLocalTransform(void) const;
	void setLocalTransform(const D3DXMATRIX& mLocal);
	D3DXVECTOR3 getCenter(void) const;

private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;
};




#endif