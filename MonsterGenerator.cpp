#include "MonsterGenerator.h"
#include <vector>


//make a monsters
void MonsterGenerator::setMonsters(float pos[2], CSphere monster) {
	monster.setCenter(pos[0], (float)monster.getRadius(), pos[1]);
	monster.setPower(0, 0);
	monsters.push_back(monster);
}

CSphere MonsterGenerator::getMonster(int i) {
	return monsters[i];
}
void MonsterGenerator::deleteMonster(int i) {
	monsters.erase(monsters.begin() + i);
}
void MonsterGenerator::setMonsterExist(int i, bool exist) {
	monsters[i].set_exist(exist);
	if (!exist) {
		deleteMonster(i);
	}
}
int MonsterGenerator::getCountMonsters() {
	return monsters.size();
}
float MonsterGenerator::getDeltaTime() {

	//QueryPerformanceFrequency((LARGE_INTEGER*)&periodFrequency);	// 1�ʵ��� CPU�� �������� ��ȯ�ϴ� �Լ� (������. �ý��� ���ý� ������)
	//timeScale = 1.0 / (double)periodFrequency;				// �̸� ������ �����

	//QueryPerformanceCounter((LARGE_INTEGER*)&beforeTime);	// ���� ������ ������ ����


	//while (1) {

	//	QueryPerformanceCounter((LARGE_INTEGER*)&afterTime);	// ���� ������ ������ ����

	//	deltaTime = ((double)afterTime - (double)beforeTime) * timeScale;	// ��� (deltaTime ����: ms)

	//	if (deltaTime > 1000) {
	//		break;
	//	}

	//}

	//return true;
	return deltaTime;

}
void MonsterGenerator::addDeltaTime(float deltaTime) {
	this->deltaTime += deltaTime;
}
void MonsterGenerator::resetDeltaTime() {
	this->deltaTime = 0;
}
void MonsterGenerator::setMonstersDown() {
	for (int i = 0; i < monsters.size(); i++) {
		monsters[i].setCenter(monsters[i].get_centerx(), monsters[i].get_centery(), monsters[i].get_centerz() - 1.0f);
	}
}