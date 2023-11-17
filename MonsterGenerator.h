#pragma once
#include "CSphere.h"
class MonsterGenerator
{
private:
	std::vector<CSphere> monsters;
	double timeScale = 0;
	double deltaTime = 0;

public:
	//make a monsters
	void setMonsters(float pos[2], CSphere monster);

	CSphere getMonster(int i);
	void deleteMonster(int i);
	void setMonsterExist(int i, bool exist);
	int getCountMonsters();
	float getDeltaTime();
	void addDeltaTime(float deltaTime);
	void resetDeltaTime();
	void setMonstersDown();
};

