#ifndef INPUT_H
#define INPUT_H

#include <BWAPI.h>

namespace BWAPI{

class Input
{
public:
	Input(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	Unit* getEnemyUnit(int index);
	enum InputType{
		BIAS,
		ALLY_HP,
		ALLY_DPF,
		ENEMY_DISTANCE,
		//ENEMY_DPF = 5,
		//ENEMY_HP = 8
		// change index accordingly for addition of bias
		ENEMY_DPF = 6,
		ENEMY_HP = 9
	};
	
	double* getInputArray();
	

private:
	int _allyTotalHealth;
	double _allyDPF;
	//double _outputArray[11];
	double _outputArray[12]; // add one input for biasing! It's the first element
	std::vector<Unit*> _enemyUnits;
	BWAPI::Position _allyCenter;
	void Input::sortUnitsVec(std::vector<BWAPI::Unit*> &v);
	bool distanceCompare(BWAPI::Unit* a, BWAPI::Unit* b);
	
};
}

#endif