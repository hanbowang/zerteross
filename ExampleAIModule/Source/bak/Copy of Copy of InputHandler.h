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
	double getInitCentralDist();
	

private:
	
	int _allyNum;
	int _enemyNum;
	int _inputSize;
	int _enemyHPIndex;
	int _positionIndex;
	int _allyUnitMaxHP;
	double _ally_center_x;
	double _ally_center_y;
	double _enemy_center_x;
	double _enemy_center_y;
	double _init_central_dist;
	std::vector<Unit*> _enemyUnits;
	std::vector<Unit*> _allyUnits;
	double InputHandler::getAngle(BWAPI::Unit* a, BWAPI::Unit* b);
	
};
}

#endif