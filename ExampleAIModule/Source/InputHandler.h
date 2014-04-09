#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <BWAPI.h>

namespace BWAPI{

class InputHandler
{
public:
	InputHandler(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	Unit* getEnemyUnit(int index);
	std::vector<Unit*> getEnemyUnits();
	std::vector<Unit*> getAllyUnits();
	enum InputType{
		BIAS,
		ALLY_HP
	};
	
	double* getInputArray();
	

private:
	
	int _allyNum;
	int _enemyNum;
	int _inputSize;
	int _enemyHPIndex;
	int _positionIndex;
	std::vector<Unit*> _enemyUnits;
	std::vector<Unit*> _allyUnits;
	double InputHandler::getAngle(BWAPI::Unit* a, BWAPI::Unit* b);
	
};
}

#endif