#ifndef MULTI12INHANDLER_H
#define MULTI12INHANDLER_H

#include <BWAPI.h>
#include "InputHandler.h"

namespace BWAPI{

	class Multi12InHandler : public InputHandler
{
public:
	Multi12InHandler(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	//Unit* getEnemyUnit(int index);
	//std::vector<Unit*> getEnemyUnits();
	//std::vector<Unit*> getAllyUnits();
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
	//double getInitCentralDist();
	
private:
	int _allyTotalHealth;
	double _allyDPF;
	//double _outputArray[11];
	double _outputArray[12]; // add one input for biasing! It's the first element
	BWAPI::Position _allyCenter;
	void sortUnitsVec(std::vector<BWAPI::Unit*> &v);
	bool distanceCompare(BWAPI::Unit* a, BWAPI::Unit* b);
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