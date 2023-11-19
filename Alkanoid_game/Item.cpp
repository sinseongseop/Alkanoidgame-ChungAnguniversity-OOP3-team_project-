#include"Item.h"
#include "d3dUtility.h"
#include "CSphere.h"
#include "CWall.h"

#include <stdlib.h> 
#include <cstdlib>
#include <vector>
#include <random>

#define Max_Whiteball 5// �� ȭ�鿡 �ִ�� ���� ������ ����� �� (1~5 ���� ����) //
#define Max_life 5 // ���� �� �ִ� ������ �ִ�(1~5���� ����)
#define ITEM_TIME1 2000 // ������1 ���� �ð� 
#define ITEM_TIME4 1000 // ������4 ���� �ð�

std::random_device it;
std::mt19937 g(it());

extern IDirect3DDevice9* Device;
extern CSphere whiteball[5]; // �� ��
extern CWall move_board; // �����̴� ��
extern int lifeCnt; //���� ��� ���� counting
extern int ballCnt; //�� ���� counting

int Item::choose_random_item() { //4���� �������� �������� ���� �������� ���ϴ� �Լ�
	//return 4; // ����� �� �ڵ�. ���ϴ� ������ ��ȣ ����
	std::uniform_int_distribution<int> distribution(1, 4); // 4���� �������� ����
	int random_number = distribution(g);
	return random_number; // ������ ���� ����(����) ��ȯ
}

void Item::longer_board() { // �����ð� ���� �����̴� ���� ���̸� ������Ų��.
	set_is_use_item1(true);
	item_time[0] += ITEM_TIME1;
	float position_x = move_board.get_x(); // ���� �����̴� ���� x ��ǥ�� ��´�.
	move_board.destroy();
	move_board.create(Device, -1, -1, 2, 0.4f, 0.15f, d3d::CYAN);
	move_board.setPosition(position_x, 0.12f, -4.7f);
}

void Item::original_board() { //�����ð� �� �����̴� ���� ���̸� ������� ������.
	set_is_use_item1(false);
	float position_x = move_board.get_x(); // ���� �����̴� ���� x ��ǥ�� ��´�.
	move_board.destroy();
	move_board.create(Device, -1, -1, 1, 0.4f, 0.15f, d3d::CYAN);
	move_board.setPosition(position_x, 0.12f, -4.7f);
}

void Item::plus_whiteball() { // �߰� ���� �����Ѵ�.
	if (ballCnt < Max_Whiteball) {
		for (int i = 0; i < Max_Whiteball; i++) {
			if (!whiteball[i].get_exist()) {
				whiteball[i].set_exist(true); //���� ���̰� ��
				whiteball[i].setPower(1.5, 1.5);
				ballCnt += 1;
				return;
			}
		}
	}
}

void Item::plus_life() { // ������ 1 �߰� �Ѵ�.
	if (lifeCnt < Max_life) {
		lifeCnt += 1;
	} //�ִ� ������ ������ ���� �� ���� �߰� 1...
}

void Item::ignore_collision() { //���� �ð����� ���� �������� ���ư��� �Ѵ�.
	set_is_use_item4(true);
	item_time[3] += ITEM_TIME4;
	for (int i = 0; i < Max_Whiteball; i++) {
		whiteball[i].set_ignore_collision(true); // �浹 ���� �Ӽ��� true�� �� �ٲ��ش�
	}
}

void Item::original_collsision() { // ���� �浹 ������ ���� ���·� ������.
	set_is_use_item4(false);
	for (int i = 0; i < Max_Whiteball; i++) {
		whiteball[i].set_ignore_collision(false);// �浹 ���� �Ӽ��� false�� �� �ٲ��ش�
	}
}

void Item::use_item() { // ������ ��� �Լ�
	int item_num = choose_random_item(); // ���� ������ 1���� ��´�.
	switch (item_num)
	{
	case 1:
		longer_board(); // �����̴� ���� ���̸� ������Ų��.
		break;
	case 2:
		plus_whiteball(); // �߰� ���� �����Ѵ�.
		break;
	case 3:
		plus_life(); //��� �ϳ� �߰�
		break;
	case 4:
		ignore_collision(); // �������� �浹�� �����ϰ� �������� ���ư���.
		break;
	default:
		break;
	}
}

void Item::set_item_time(int time, int item_num) { // item_num�� ���� ���ӽð��� time���� �����Ѵ�.
	item_time[item_num] = time;
}

int Item::get_item_time(int item_num) { //item_num�� ���� ���ӽð��� ��´�.
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
