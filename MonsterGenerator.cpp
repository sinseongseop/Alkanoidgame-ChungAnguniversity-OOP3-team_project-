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