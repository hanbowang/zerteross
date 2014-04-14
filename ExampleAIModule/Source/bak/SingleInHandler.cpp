#include "SingleInHandler.h"
#include <algorithm>
#include <math.h>

namespace BWAPI{
	SingleInHandler::SingleInHandler(const std::set<BWAPI::Unit*> allyUnits, const std::set<BWAPI::Unit*> enemyUnits)//:
//_allyNum(allyUnits.size()),
//_enemyNum(enemyUnits.size())
	{
		// calculate the input demension and relative index
		_allyNum = allyUnits.size();
		_enemyNum = enemyUnits.size();
		_inputSize = 1 + _allyNum + _enemyNum + _allyNum * _enemyNum * 2;
		_enemyHPIndex = 1 + _allyNum;
		_positionIndex = 1 + _allyNum + _enemyNum;
		
		// convert the units set into vector, and other information
		std::vector<BWAPI::Unit*> enemyUnitsVec(enemyUnits.begin(), enemyUnits.end());
		std::vector<BWAPI::Unit*> allyUnitsVec(allyUnits.begin(), allyUnits.end());
		// get the hp of the ally unit which has biggest hp
		_allyUnitMaxHP = 0;
		// get the initial center distance between ally units and enemy units
		_ally_center_x = 0;
		_ally_center_y = 0;
		_enemy_center_x = 0;
		_enemy_center_y = 0;
		
		for(int i = 0; i < _allyNum; i++){
			_allyUnits.push_back(allyUnitsVec[i]);
			if(_allyUnitMaxHP < allyUnitsVec[i]->getHitPoints()){
				_allyUnitMaxHP = allyUnitsVec[i]->getHitPoints();
			}
			_ally_center_x += allyUnitsVec[i]->getPosition().x();
			_ally_center_y += allyUnitsVec[i]->getPosition().y();
		}
		for(int i = 0; i < _enemyNum; i++) {
			_enemyUnits.push_back(enemyUnitsVec[i]);
			_enemy_center_x += enemyUnitsVec[i]->getPosition().x();
			_enemy_center_y += enemyUnitsVec[i]->getPosition().y();
		}
		_init_central_dist = sqrt(pow((_ally_center_x / allyUnitsVec.size() - _enemy_center_x / enemyUnitsVec.size()), 2.0) + 
			pow((_ally_center_y / allyUnitsVec.size() - _enemy_center_y / enemyUnitsVec.size()), 2.0));
	}

	double* SingleInHandler::getInputArray(){
		// create inputArray and assign bias element
		double* inputArray;
		inputArray = new double[_inputSize];
		inputArray[BIAS] = 1.0;

		// get the normalized HP of ally units and enemy units
		for(int i = 0; i < _allyNum; i++){
			inputArray[ALLY_HP + i] = (double)_allyUnits[i]->getHitPoints() / _allyUnitMaxHP;
		}
		for(int i = 0; i < _enemyNum; i ++){
			inputArray[_enemyHPIndex + i] = (double)_enemyUnits[i]->getHitPoints() / _allyUnitMaxHP;
		}

		// get normalized relative position of ally unit - enemy unit pair
		for(int i = 0; i < _allyNum; i++){
			for(int j = 0; j < _enemyNum; j++){
				if(_allyUnits[i]->exists() && _enemyUnits[j]->exists()){
					// get distance
					inputArray[_positionIndex + i * _enemyNum * 2 + j * 2] = (double)_allyUnits[i]->getDistance(_enemyUnits[j]) / _init_central_dist;
					// get angle
					inputArray[_positionIndex + i * _enemyNum * 2 + j * 2 + 1] = getAngle(_allyUnits[i], _enemyUnits[j]);
				} else {
					// assign 0 to nonexisted pair
					inputArray[_positionIndex + i * _enemyNum * 2 + j * 2] = 0;
					inputArray[_positionIndex + i * _enemyNum * 2 + j * 2 + 1] = 0;
				}
			}
		}
		return inputArray;
	}

	//double InputHandler::getAngle(BWAPI::Unit* a, BWAPI::Unit* b){
	//	double PI = 3.14159;
	//	int delta_x = b->getPosition().x() - a->getPosition().x();
	//	int delta_y = b->getPosition().y() - a->getPosition().y();
	//	double hypotenuse = sqrt(pow(delta_x, 2.0) + pow(delta_y, 2.0));
	//	// normalize to (0, 1)
	//	return (asin(delta_y / hypotenuse) + PI) / 2 / PI;
	//}

	//Unit* InputHandler::getEnemyUnit(int index){
	//	if (index < (int)_enemyUnits.size()){
	//		return _enemyUnits[index];
	//	}
	//	else{
	//		return 0;
	//	}
	//}

	//std::vector<Unit*> InputHandler::getAllyUnits(){
	//	return _allyUnits;
	//}

	//std::vector<Unit*> InputHandler::getEnemyUnits(){
	//	return _enemyUnits;
	//}

	//double InputHandler::getInitCentralDist(){
	//	return _init_central_dist;
	//}
}