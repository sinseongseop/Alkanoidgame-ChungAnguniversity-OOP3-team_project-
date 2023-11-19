#include"Item.h"
#include "d3dUtility.h"
#include "CSphere.h"
#include "CWall.h"

#include <stdlib.h> 
#include <cstdlib>
#include <vector>
#include <random>

#define Max_Whiteball 5// 한 화면에 최대로 존재 가능한 흰공의 수 (1~5 까지 가능) //
#define Max_life 5 // 가질 수 있는 생명의 최댓값(1~5까지 가능)
#define ITEM_TIME1 2000 // 아이템1 지속 시간 
#define ITEM_TIME4 1000 // 아이템4 지속 시간

std::random_device it;
std::mt19937 g(it());

extern IDirect3DDevice9* Device;
extern CSphere whiteball[5]; // 흰 공
extern CWall move_board; // 움직이는 판
extern int lifeCnt; //남은 목숨 개수 counting
extern int ballCnt; //공 개수 counting

int Item::choose_random_item() { //4가지 아이템중 랜덤으로 얻을 아이템을 정하는 함수
	//return 4; // 디버깅 용 코드. 원하는 아이템 번호 삽입
	std::uniform_int_distribution<int> distribution(1, 4); // 4가지 아이템이 존재
	int random_number = distribution(g);
	return random_number; // 정해진 랜덤 숫자(정수) 반환
}

void Item::longer_board() { // 일정시간 동안 움직이는 판의 길이를 증가시킨다.
	set_is_use_item1(true);
	item_time[0] += ITEM_TIME1;
	float position_x = move_board.get_x(); // 기존 움직이는 판의 x 좌표를 얻는다.
	move_board.destroy();
	move_board.create(Device, -1, -1, 2, 0.4f, 0.15f, d3d::CYAN);
	move_board.setPosition(position_x, 0.12f, -4.7f);
}

void Item::original_board() { //일정시간 후 움직이는 판의 길이를 원래대로 돌린다.
	set_is_use_item1(false);
	float position_x = move_board.get_x(); // 기존 움직이는 판의 x 좌표를 얻는다.
	move_board.destroy();
	move_board.create(Device, -1, -1, 1, 0.4f, 0.15f, d3d::CYAN);
	move_board.setPosition(position_x, 0.12f, -4.7f);
}

void Item::plus_whiteball() { // 추가 공을 생성한다.
	if (ballCnt < Max_Whiteball) {
		for (int i = 0; i < Max_Whiteball; i++) {
			if (!whiteball[i].get_exist()) {
				whiteball[i].set_exist(true); //눈에 보이게 함
				whiteball[i].setPower(1.5, 1.5);
				ballCnt += 1;
				return;
			}
		}
	}
}

void Item::plus_life() { // 생명을 1 추가 한다.
	if (lifeCnt < Max_life) {
		lifeCnt += 1;
	} //최대 생명보다 생명이 작을 때 생명 추가 1...
}

void Item::ignore_collision() { //일정 시간동안 공이 직선으로 날아가게 한다.
	set_is_use_item4(true);
	item_time[3] += ITEM_TIME4;
	for (int i = 0; i < Max_Whiteball; i++) {
		whiteball[i].set_ignore_collision(true); // 충돌 무시 속성을 true로 다 바꿔준다
	}
}

void Item::original_collsision() { // 공의 충돌 판정을 원래 상태로 돌린다.
	set_is_use_item4(false);
	for (int i = 0; i < Max_Whiteball; i++) {
		whiteball[i].set_ignore_collision(false);// 충돌 무시 속성을 false로 다 바꿔준다
	}
}

void Item::use_item() { // 아이템 사용 함수
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

void Item::set_item_time(int time, int item_num) { // item_num의 남은 지속시간을 time으로 설정한다.
	item_time[item_num] = time;
}

int Item::get_item_time(int item_num) { //item_num의 남은 지속시간을 얻는다.
	return item_time[item_num];
}

bool Item::get_is_use_item1() {
	return is_use_item1;
}

bool Item::get_is_use_item4() {
	return is_use_item4;
}

void Item::set_is_use_item1(bool use) {
	is_use_item1 = use;
}

void Item::set_is_use_item4(bool use) {
	is_use_item4 = use;
}
