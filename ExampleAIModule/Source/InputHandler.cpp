#include "InputHandler.h"
#include <algorithm>
#include <math.h>

namespace BWAPI{
	InputHandler::InputHandler(const std::set<BWAPI::Unit*> allyUnits, const std::set<BWAPI::Unit*> enemyUnits):
	_allyNum(allyUnits.size()),
	_enemyNum(enemyUnits.size())
	{
		// calculate the input demension and relative index
		_inputSize = 1 + _allyNum + _enemyNum + _allyNum * _enemyNum * 2;
		_enemyHPIndex = 1 + _allyNum;
		_positionIndex = 1 + _allyNum + _enemyNum;
		
		// convert the units set into vector
		std::vector<BWAPI::Unit*> enemyUnitsVec(enemyUnits.begin(), enemyUnits.end());
		std::vector<BWAPI::Unit*> allyUnitsVec(allyUnits.begin(), allyUnits.end());
		for(int i = 0; i < _allyNum; i++){
			_allyUnits.push_back(allyUnitsVec[i]);
		}
		for(int i = 0; i < _enemyNum; i++) {
			_enemyUnits.push_back(enemyUnitsVec[i]);
		}
	}

	double* InputHandler::getInputArray(){
		// create inputArray and assign bias element
		double* inputArray;
		inputArray = new double[_inputSize];
		inputArray[BIAS] = 1.0;

		// get the HP of ally units and enemy units
		for(int i = 0; i < _allyNum; i++){
			inputArray[ALLY_HP + i] = _allyUnits[i]->getHitPoints();
		}
		for(int i = 0; i < _enemyNum; i ++){
			inputArray[_enemyHPIndex + i] = _enemyUnits[i]->getHitPoints();
		}

		// get relative position of ally unit - enemy unit pair
		for(int i = 0; i < _allyNum; i++){
			for(int j = 0; j < _enemyNum; j++){
				if(_allyUnits[i]->exists() && _enemyUnits[j]->exists()){
					// get distance
					inputArray[_positionIndex + i * _enemyNum * 2 + j * 2] = _allyUnits[i]->getDistance(_enemyUnits[j]);
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

	double InputHandler::getAngle(BWAPI::Unit* a, BWAPI::Unit* b){
		int delta_x = b->getPosition().x() - a->getPosition().x();
		int delta_y = b->getPosition().y() - a->getPosition().y();
		double hypotenuse = sqrt(pow(delta_x, 2.0) + pow(delta_y, 2.0));
		return asin(delta_y / hypotenuse);
	}

	Unit* InputHandler::getEnemyUnit(int index){
		if (index < (int)_enemyUnits.size()){
			return _enemyUnits[index];
		}
		else{
			return 0;
		}
	}

	std::vector<Unit*> InputHandler::getAllyUnits(){
		return _allyUnits;
	}

	std::vector<Unit*> InputHandler::getEnemyUnits(){
		return _enemyUnits;
	}
}