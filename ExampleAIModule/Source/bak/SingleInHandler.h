#ifndef MULTI43INHANDLER_H
#define MULTI43INHANDLER_H

#include <BWAPI.h>
#include "InputHandler.h"

namespace BWAPI{

	class Multi43InHandler : public InputHandler
{
public:
	Multi43InHandler(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	enum InputType{
		BIAS,
		ALLY_HP
	};
	double* getInputArray();
	
//private:
//	
//	int _allyNum;
//	int _enemyNum;
//	int _inputSize;
//	int _enemyHPIndex;
//	int _positionIndex;
//	int _allyUnitMaxHP;
//	double _ally_center_x;
//	double _ally_center_y;
//	double _enemy_center_x;
//	double _enemy_center_y;
//	double _init_central_dist;
//	std::vector<Unit*> _enemyUnits;
//	std::vector<Unit*> _allyUnits;
//	double InputHandler::getAngle(BWAPI::Unit* a, BWAPI::Unit* b);
	
};
}

#endif