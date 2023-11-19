#pragma once
#ifndef Item_H
#define Item_H

#include "d3dUtility.h"

class Item {
private:
	int item_time[4] = { 0,0,0,0 }; // ������ ��ȣ�� ���� ���� �ð�( item1,item4���׸� ���ǹ�, ��Ģ�� ������ 4�� ����)
	bool is_use_item1 = false;
	bool is_use_item4 = false;

public:
	int choose_random_item();
	void longer_board();
	void original_board();
	void plus_whiteball();
	void plus_life();
	void ignore_collision();
	void original_collsision();
	void use_item();
	void set_item_time(int time, int item_num);
	int get_item_time(int item_num);
	bool get_is_use_item1();
	bool get_is_use_item4();
	void set_is_use_item1(bool use);
	void set_is_use_item4(bool use);
};

#endif
