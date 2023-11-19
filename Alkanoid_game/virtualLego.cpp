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
// OS: Window
// code write: 2023-11-19
// Remodified by ChungAng-university OOP Class3 Team 8 -  김진호, 김혁진, 박수빈, 박주연, 송민혁, 신성섭 
//        
////////////////////////////////////////////////////////////////////////////////

#include "CSphere.h"
#include "d3dUtility.h"
#include "CLight.h"
#include "CLife.h"
#include "CWall.h"
#include "Item.h"
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <random>

// point 표시하기
#include <D3dx9core.h>
// 화면에 표시할 숫자 담을 배열
char PointString[9];

//랜덤 변수 초기 설정
std::random_device rd;
std::mt19937 gen(rd());


IDirect3DDevice9* Device = NULL;

// window size
const int Width = 1500;
const int Height = 768;

// 벽의 가로, 세로, 두께 default 값
const float Wall_height = 10.0f;
const float Wall_width = 7.0f;
const float Wall_thickness = 0.2f;

//life의 position
const float life_height = 5.5f;
const float life_x = 3.5f;
const float life_interval = 0.5f;

bool exitplag = false; //게임 종료 시 true

// initialize the position (coordinate) of each ball (ball0 ~ ball3)
const float startpos_y = 4.19f;
float spherePos[4][2] = { {+1.2f,startpos_y} , {3.1f,startpos_y}, {2.3f,startpos_y} ,{-0.5f,startpos_y} };
// initialize the color of each ball (ball0 ~ ball3)
const D3DXCOLOR sphereColor[10] = { d3d::RED, d3d::RED, d3d::RED, d3d::RED,d3d::RED ,d3d::RED ,d3d::RED ,d3d::RED ,d3d::RED ,d3d::RED };

// There are four balls
//// initialize the position (coordinate) of each ball (ball0 ~ ball3)
//const float spherePos[10][2] = { {1.5f,-2.0f} , {+2.4f,0} , {1.2f,1} , {-2.0f,-1.5f},{-1.4f,2.2f} ,{2.4f,3.2f} ,{-1.3f,1.2f} ,{1.6f,1.3f} ,{-1.2f,-2.1f} ,{-1.0f,2.1f} };
//// initialize the color of each ball (ball0 ~ ball3)
//const D3DXCOLOR sphereColor[10] = { d3d::RED, d3d::RED, d3d::RED, d3d::RED,d3d::RED ,d3d::RED ,d3d::RED ,d3d::RED ,d3d::BLACK ,d3d::BLACK }; //블록

const float whitePos[5][2] = { {0.0f,-4.43f + M_RADIUS} ,{-1.0f, -1.5f} , {-1.0f, -2.0f}, {0.7f, -1.5f}, {-0.7f, -1.5f} }; // 흰공과 블록 분리

// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------
D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;


#define M_RADIUS 0.3   // ball radius
#define PI 3.14159265
#define M_HEIGHT 0.1
#define DECREASE_RATE 1 //공의 감속 관련
#define ITEM_TIME1 2000 // 아이템1 지속 시간 
#define ITEM_TIME4 1000 // 아이템4 지속 시간
#define Max_Whiteball 5// 한 화면에 최대로 존재 가능한 흰공의 수 (1~5 까지 가능) //
#define Max_life 5 // 가질 수 있는 생명의 최댓값(1~5까지 가능)

void exitProgram();
int lifeCnt = 4; //남은 목숨 개수 counting
int ballCnt = 1; //공 개수 counting

void minus_lifeCnt() {
	lifeCnt--;
	if (lifeCnt == 0) {
		exitProgram();
		return;
	}
}

// 글자 출력
ID3DXFont* Font = 0;               // VC++ 6.0, DirectX SDK 9.0b

// 전체 포인트
int total_point = 0;

// -----------------------------------------------------------------------------
// CSphere class definition
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// CWall class definition
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
// CLight class definition
// -----------------------------------------------------------------------------

// MonsterGenerator 클래스
class MonsterGenerator
{
private:
	std::vector<CSphere> monsters;
	__int64 afterTime = 0;
	__int64 beforeTime = 0;
	__int64 periodFrequency = 0;
	double timeScale = 0;
	double deltaTime = 0;

public:
	//make a monsters
	void setMonsters(float pos[2], CSphere monster) {
		monster.setCenter(pos[0], (float)monster.getRadius(), pos[1]);
		monster.setPower(0, 0);
		monsters.push_back(monster);
	}

	CSphere getMonster(int i) {
		return monsters[i];
	}
	void deleteMonster(int i) {
		monsters.erase(monsters.begin() + i);
	}
	void setMonsterExist(int i, bool exist) {
		monsters[i].set_exist(exist);
		if (!exist) {
			deleteMonster(i);
		}
	}
	int getCountMonsters() {
		return monsters.size();
	}
	float getDeltaTime() {

		//QueryPerformanceFrequency((LARGE_INTEGER*)&periodFrequency);	// 1초동안 CPU의 진동수를 반환하는 함수 (고정값. 시스템 부팅시 고정됨)
		//timeScale = 1.0 / (double)periodFrequency;				// 미리 역수로 만들기

		//QueryPerformanceCounter((LARGE_INTEGER*)&beforeTime);	// 이전 프레임 진동수 측정


		//while (1) {

		//	QueryPerformanceCounter((LARGE_INTEGER*)&afterTime);	// 현재 프레임 진동수 측정

		//	deltaTime = ((double)afterTime - (double)beforeTime) * timeScale;	// 계산 (deltaTime 단위: ms)

		//	if (deltaTime > 1000) {
		//		break;
		//	}

		//}

		//return true;
		return deltaTime;

	}
	void addDeltaTime(float deltaTime) {
		this->deltaTime += deltaTime;
	}
	void resetDeltaTime() {
		this->deltaTime = 0;
	}
	void setMonstersDown() {
		for (int i = 0; i < monsters.size(); i++) {
			monsters[i].setCenter(monsters[i].get_centerx(), monsters[i].get_centery(), monsters[i].get_centerz() - 1.0f);
		}
	}
	void clear() {
		monsters.clear();
	}
};


// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
CWall	g_legoPlane;
CWall	g_legowall[4];
CSphere	g_sphere[10];
CSphere	g_target_blueball;
CSphere whiteball[5]; // 흰 공
CLight	g_light;
CWall move_board; // 움직이는 판
CLife	g_life[5]; //생명
MonsterGenerator monsterGenerator;

//아이템 클래스


Item special_item;// 아이템 객체 

double g_camera_pos[3] = { 0.0, 0.0, -0.0 };

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

void exitProgram() {//종료 함수
	exitplag = true;
	monsterGenerator.clear();

	for (int i = 0; i < Max_Whiteball; i++) {
		whiteball[i].set_exist(false);
	}

	lifeCnt = 0;
}


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
	if (false == g_legoPlane.create(Device, -1, -1, Wall_width, 0.2f, Wall_height, d3d::GREEN)) return false; //Device, ? , ?,  가로, 두께, 높이, 색깔
	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);

	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, Wall_thickness, Wall_thickness, Wall_height, d3d::BLACK)) return false; //왼쪽벽
	g_legowall[0].setPosition(-3.5f, 0.12f, 0.0f);

	if (false == g_legowall[1].create(Device, -1, -1, Wall_width, Wall_thickness, Wall_thickness, d3d::BLACK)) return false; //위쪽 벽
	g_legowall[1].setPosition(0.0f, 0.12f, 4.95f);

	if (false == g_legowall[2].create(Device, -1, -1, Wall_thickness, Wall_thickness, Wall_height, d3d::BLACK)) return false; // 오른쪽 벽
	g_legowall[2].setPosition(3.5f, 0.12f, 0.0f);

	if (false == move_board.create(Device, -1, -1, 1, 0.4f, 0.15f, d3d::CYAN)) return false;  //	움직이는 판
	move_board.setPosition(0.0f, 0.12f, -4.7f);

	for (int i = 0; i < 3; i++) {
		g_legowall[i].setwallnum(i);
	}

	move_board.setwallnum(3); // 움직이는 보드 번호(3) 설정

	//// create balls and set the position
	//for (i = 0; i < 10; i++) {
	//	if (false == g_sphere[i].create(Device, sphereColor[i])) return false;
	//	g_sphere[i].setCenter(spherePos[i][0], (float)g_sphere[i].getRadius(), spherePos[i][1]);
	//	g_sphere[i].setPower(0, 0);
	//}

	// create balls and set the position
	for (i = 0; i < 4; i++) {
		CSphere sphere;
		if (false == sphere.create(Device, sphereColor[i])) return false;
		monsterGenerator.setMonsters(spherePos[i], sphere);
	}

	for (i = 0; i < Max_Whiteball; i++) { //*** 흰공 초기화***
		if (false == whiteball[i].create(Device, d3d::WHITE)) return false;
		whiteball[i].setCenter(whitePos[i][0], (float)whiteball[i].getRadius(), whitePos[i][1]);
		whiteball[i].setPower(0, 0);
		whiteball[i].set_whiteball(true);
		whiteball[i].set_exist(false); //***처음에는 추가공 안 보이게 설정***
	}

	whiteball[0].set_exist(true); // 처음 흰공 1개는 보여야함

	/*for (int i = 1; i<10; i++) { // 디버깅 확인용 모든 공에 아이템 속성 부여
		g_sphere[i].set_item(true);
	}*/


	//g_sphere[8].set_item(true);
	//g_sphere[9].set_item(true);

	//life 생성
	for (i = 0; i < 5; i++) {
		if (false == g_life[i].create(Device)) return false;
		g_life[i].setCenter((life_x - life_interval * i), (float)g_life[i].getRadius(), life_height);
	}


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

	// 글자 출력
	// Font 초기화 / 생성
	D3DXFONT_DESCA lf2;
	ZeroMemory(&lf2, sizeof(D3DXFONT_DESCA));
	lf2.Height = 22;
	lf2.Width = 10;
	lf2.Weight = 100;
	lf2.MipLevels = 0;
	lf2.Italic = false;
	lf2.CharSet = 0;
	lf2.OutputPrecision = 0;
	lf2.Quality = 0;
	lf2.PitchAndFamily = 0;
	strcpy(lf2.FaceName, "Arial"); // font style

	D3DXCreateFontIndirect(Device, &lf2, &Font);

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

	// 글자 출력
	Font->Release();
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

		//종료 조건 확인
		if (!exitplag)
			if (monsterGenerator.getMonster(0).get_centerz() < -5.0f)
				exitProgram();

		// update the position of each ball. during update, check whether each ball hit by walls.
			for (j = 0; j < 3; j++) {
				//g_legowall[j].hitBy(g_sphere[i]);
				for (int k = 0; k < Max_Whiteball; k++) {
					if (whiteball[k].get_exist()) {
						g_legowall[j].hitBy(whiteball[k]);
					}
				}
			}
			//g_sphere[i].ballUpdate(timeDelta);

			//move_board.hitBy(g_sphere[i]);

		for (i = 0; i < Max_Whiteball; i++) {
			whiteball[i].ballUpdate(timeDelta);
			move_board.hitBy(whiteball[i]);
		}

		//// check whether any two balls hit together and update the direction of balls
		//for (i = 0; i < 10; i++) {
		//	if (g_sphere[i].get_exist()) { // i 공이  실재로 존재하면 i공과 움직이는 공의 충돌 확인
		//		for (j = 0; j < Max_Whiteball; j++) { // 추가 흰공이 존재하면 충돌 확인
		//			if (whiteball[j].get_exist()) {
		//				whiteball[j].hitBy(g_sphere[i]);
		//			}

		//		}
		//	}
		//	else { // 공이 존재하지 않으면 이 공이 아이템을 가지고 있는 지 확인한다.
		//		if (g_sphere[i].is_has_item()) {
		//			special_item.use_item();
		//			g_sphere[i].set_item(false); // 아이템을 먹었으므로 공의 item을 false 처리 해준다.
		//		}

		//	}
		//}

		// MonsterGenerator 클래스 함수들 추가
		// gameover 확인
		if (!exitplag) monsterGenerator.addDeltaTime(timeDelta);
		//if duration>1s, set a new balls
		std::vector<float> before_xvalue;
		float random_value;
		if (monsterGenerator.getDeltaTime() > 2) {

			//existing balls down.
			monsterGenerator.setMonstersDown();

			//set a new balls
			for (i = 0; i < 4; i++) {
				CSphere sphere;
				// 원하는 분포와 범위로 랜덤 실수 생성

				std::uniform_real_distribution<float> distribution(M_RADIUS, 6.2 - M_RADIUS);
				random_value = distribution(gen);
				if (i != 0) {
					for (j = 0; j < before_xvalue.size(); j++)
					{
						//if random value is nesting
						if (random_value > before_xvalue[j]) {

							if (random_value < before_xvalue[j] + M_RADIUS * 2 + 0.5) {
								std::uniform_real_distribution<float> distribution(M_RADIUS, 6.2 - M_RADIUS);
								random_value = distribution(gen);
								j = -1;
								//exit(1);
								continue;
							}
						}
						else {
							if (random_value > before_xvalue[j] - M_RADIUS * 2 - 0.5) {
								std::uniform_real_distribution<float> distribution(M_RADIUS, 6.2 - M_RADIUS);
								random_value = distribution(gen);
								j = -1;
								continue;
							}
						}
					}
				}
				before_xvalue.push_back(random_value);
				float new_ball_pos[2] = { random_value - 3.2,startpos_y };

				// item 세팅
				std::uniform_real_distribution<float> distribution1(0.0, 10.0);
				float random = distribution1(gen);
				if (random >= 9.0) {
					if (false == sphere.create(Device, d3d::BLACK)) return false;
					sphere.set_item(true);
				}
				else {
					if (false == sphere.create(Device, d3d::RED)) return false;
				}

				monsterGenerator.setMonsters(new_ball_pos, sphere);
			}
			monsterGenerator.resetDeltaTime();
		}

		// check whether any two balls hit together and update the direction of balls
		for (i = 0; i < monsterGenerator.getCountMonsters(); i++) {
			CSphere monster = monsterGenerator.getMonster(i);
			if (monster.get_exist()) { // i  실재로 존재하면 i공과 움직이는 공의 충돌 확인
				for (int j = 0; j < Max_Whiteball; j++) { // 추가 흰 공이 있으면 충돌 확인
					if (whiteball[j].get_exist()) {
						whiteball[j].hitBy(monster);
						if (whiteball[j].is_has_item()) {
							special_item.use_item();
							whiteball[j].set_item(false);
						}
					}
				}
				if (!monster.get_exist())//block이 존재하지않으면 삭제
					monsterGenerator.deleteMonster(i);
			}
		}

		// monster draw
		for (i = 0; i < monsterGenerator.getCountMonsters(); i++) {
			CSphere monster = monsterGenerator.getMonster(i);
			monster.draw(Device, g_mWorld);
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

		bool incorrect_positionbug = false;

		for (int i = 0; i < Max_Whiteball; i++) {
			if (whiteball[i].is_balloutside(incorrect_positionbug)) { //움직이는 공이 규격장 밖으로 나가면 공의 초기화 시킴.
				whiteball[i].destroy();
				whiteball[i].create(Device, d3d::WHITE);
				if (incorrect_positionbug) { // 버그가 발생한 경우 공을 올바른 위치로 재생성후 진행
					whiteball[i].setCenter(whitePos[i][0], (float)whiteball[i].getRadius(), whitePos[i][1]);
					whiteball[i].setPower(1, -1);
					whiteball[i].set_exist(true);
				}
				else { // 버그가 아닌 경우 공이 안보이게
					whiteball[i].setCenter(whitePos[i][0], (float)whiteball[i].getRadius(), whitePos[i][1]);
					whiteball[i].setPower(0, 0);
					whiteball[i].set_exist(false);
				}
				// 횐공이 밖으로나가 초기화되면 흰공 자체 점수 0으로 초기화하고 흰공이 바에서 처음 튕기는 것으로 세팅
				whiteball[i].setPoint(0);
				whiteball[i].setFirstHit(true);
			}
		}

		if (ballCnt == 0) { // 공이 0 개인 경우 생명을 1깍고 흰공 1개 생성
			ballCnt = 1;
			minus_lifeCnt();			// 생명을 1 깍는다.
			if (lifeCnt > 0) {
				//흰 공 생성 위치 움직이는 판 위로 고정
				D3DXVECTOR3 coord3d = move_board.getCenter();
				whiteball[0].setCenter(coord3d.x, coord3d.y, -4.6f + M_RADIUS);
				whiteball[0].set_exist(true);

				// 움직이는 판에서 우클릭으로 흰공이 움직일 수 있게 설정
				whiteball[0].setActive(true);
			}
		}


		//g_sphere[0].ball_position(); // 움직이는 공이 규격장 밖으로 나갔는 지 확인 

		// draw plane, walls, and spheres
		g_legoPlane.draw(Device, g_mWorld);
		for (i = 0; i < 3; i++) {
			g_legowall[i].draw(Device, g_mWorld);
		}

		move_board.draw(Device, g_mWorld);

		//for (i = 0; i < 10; i++) {
		//	if (g_sphere[i].get_exist()) {
		//		g_sphere[i].draw(Device, g_mWorld);
		//	}
		//}

		for (i = 0; i < Max_Whiteball; i++) {
			if (whiteball[i].get_exist()) {
				whiteball[i].draw(Device, g_mWorld);
			}
		}

		for (i = 0; i < lifeCnt; i++)
			g_life[i].draw(Device, g_mWorld);
		//g_target_blueball.draw(Device, g_mWorld);
	   // g_light.draw(Device); // 빛 위치 끄기


		// 글자 출력
		// Render
		//

		// total_point(점수) 출력
		sprintf(PointString, "%d", total_point);

		RECT rect = { 630, 47, Width, Height };
		Font->DrawText(0,
			PointString,
			-1, // size of string or -1 indicates null terminating string
			&rect,            // rectangle text is to be formatted to in windows coords
			DT_TOP | DT_LEFT, // draw in the top left corner of the viewport
			0xff000000);      // black text


		RECT rect2 = { 535, 47, Width, Height };
		//g_pFont->DrawText(0, "한글 지원??", -1, &rect2, DT_TOP | DT_LEFT, 0xff000000);
		Font->DrawText(0, "Point : ", -1, &rect2, DT_TOP | DT_LEFT, 0xff000000);

		RECT rect3 = { 768, 47, Width, Height };
		//g_pFont->DrawText(0, "한글 지원??", -1, &rect2, DT_TOP | DT_LEFT, 0xff000000);
		Font->DrawText(0, "Life : ", -1, &rect3, DT_TOP | DT_LEFT, 0xff000000);

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
			D3DXVECTOR3	whitepos = whiteball[0].getCenter(); // 이부분을 수정하면 발사되는 공이 수정 됩니다.
			double theta = acos(sqrt(pow(targetpos.x - whitepos.x, 2)) / sqrt(pow(targetpos.x - whitepos.x, 2) +
				pow(targetpos.z - whitepos.z, 2)));		// 기본 1 사분면
			if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x >= 0) { theta = -theta; }	//4 사분면
			if (targetpos.z - whitepos.z >= 0 && targetpos.x - whitepos.x <= 0) { theta = PI - theta; } //2 사분면
			if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x <= 0) { theta = PI + theta; } // 3 사분면
			double distance = sqrt(pow(targetpos.x - whitepos.x, 2) + pow(targetpos.z - whitepos.z, 2));
			whiteball[0].setPower(10 * cos(theta), 10 * sin(theta)); // 앞의 객체를 발사하는 공으로 변경하면 됩니다.

			// 힌공이 움직이는 판에서 튕기기 시작했을 때 우클릭으로 움직일 수 없게 변경
			whiteball[0].setActive(false);

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
		else { // 흰공이 발사 됐을 때 흰공은 우클릭으로 움직일 수 없게 해아함 <- spacebar눌렀을 시에 따라 통제
			isReset = true;

			if (LOWORD(wParam) & MK_RBUTTON) { // 이부분을 수정하여 움직이는 판 이 오직 좌우 방향으로만 움직이게 만들기 가능.
				dx = (old_x - new_x);// * 0.01f;
				dy = 0; // 0.01f;  // y방향으로는 움직이게 하는 것 불가능. 

				D3DXVECTOR3 coord3d = move_board.getCenter();
				D3DXVECTOR3 coord_ball3d = whiteball[0].getCenter();
				if (-3.4 + move_board.get_witdh() / 2 + 0.1 <= coord3d.x + dx * (-0.007f) && coord3d.x + dx * (-0.007f) <= 3.4 - move_board.get_witdh() / 2 - 0.1) {
					//g_target_blueball.setCenter(coord3d.x + dx * (-0.007f), coord3d.y, coord3d.z + dy * 0.007f);
					move_board.setPosition(coord3d.x + dx * (-0.007f), coord3d.y, coord3d.z + dy * 0.007f);
					if(whiteball[0].getActive()) whiteball[0].setCenter(coord_ball3d.x + dx * (-0.007f), coord_ball3d.y, coord_ball3d.z + dy * 0.007f);
				}
				else { //움직이는 판 이 움직일 수 있는 x축 범위를 제한
					if (coord3d.x + dx * (-0.007f) < -3.4 + move_board.get_witdh() / 2 + 0.1) {
						//g_target_blueball.setCenter(-2.8, coord3d.y, coord3d.z + dy * 0.007f);
						move_board.setPosition(-3.4 + move_board.get_witdh() / 2 + 0.1, coord3d.y, coord3d.z + dy * 0.007f);
						if (whiteball[0].getActive()) whiteball[0].setCenter(-3.4 + move_board.get_witdh() / 2 + 0.1, coord_ball3d.y, coord_ball3d.z + dy * 0.007f);
					}
					else {
						//g_target_blueball.setCenter(2.8, coord3d.y, coord3d.z + dy * 0.007f);
						move_board.setPosition(3.4 - move_board.get_witdh() / 2 - 0.1, coord3d.y, coord3d.z + dy * 0.007f);
						if (whiteball[0].getActive()) whiteball[0].setCenter(3.4 - move_board.get_witdh() / 2 - 0.1, coord_ball3d.y, coord_ball3d.z + dy * 0.007f);
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