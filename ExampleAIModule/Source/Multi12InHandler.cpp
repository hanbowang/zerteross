#include "Multi12InHandler.h"
#include <algorithm>
#include <math.h>

namespace BWAPI{
	Multi12InHandler::Multi12InHandler(const std::set<BWAPI::Unit*> allyUnits, const std::set<BWAPI::Unit*> enemyUnits)
	{
		_allyNum = allyUnits.size();
		_enemyNum = enemyUnits.size();
		
		std::vector<BWAPI::Unit*> enemyUnitsVec(enemyUnits.begin(), enemyUnits.end());
		std::vector<BWAPI::Unit*> allyUnitsVec(allyUnits.begin(), allyUnits.end());
		// get the hp of the ally unit which has biggest hp
		_allyUnitMaxHP = 0;
		
		for(int i = 0; i < _allyNum; i++){
			_allyUnits.push_back(allyUnitsVec[i]);
			if(_allyUnitMaxHP < allyUnitsVec[i]->getHitPoints()){
				_allyUnitMaxHP = allyUnitsVec[i]->getHitPoints();
			}
		}
		for(int i = 0; i < _enemyNum; i++) {
			_enemyUnits.push_back(enemyUnitsVec[i]);
		}
		
		
	}

	double* Multi12InHandler::getInputArray(){
		_allyTotalHealth = 0;
		_allyDPF = 0;
		int centerX = 0;
		int centerY = 0;
		for (int i = 0; i < _allyUnits.size(); i++){
			if(_allyUnits[i]->exists()){
				_allyTotalHealth += _allyUnits[i]->getHitPoints();
				BWAPI::WeaponType weapon = _allyUnits[i]->getType().groundWeapon();
				_allyDPF += weapon.damageAmount() / (double)weapon.damageCooldown();
				centerX += _allyUnits[i]->getPosition().x();
				centerY += _allyUnits[i]->getPosition().y();
			}
		}
		if(!_allyUnits.empty()) {
		centerX /= _allyUnits.size();
		centerY /= _allyUnits.size();
		}
		BWAPI::Position allyCenter(centerX, centerY);
		_allyCenter = allyCenter;

		_outputArray[BIAS] = 1.0;
		_outputArray[ALLY_HP] = _allyTotalHealth;
		_outputArray[ALLY_DPF] = _allyDPF;

		for (int i = 0; i < _enemyUnits.size(); i++){
			if (_enemyUnits[i]->exists()){
				_outputArray[ENEMY_DISTANCE+i] = _enemyUnits[i]->getDistance(_allyCenter);
				BWAPI::WeaponType weapon = _enemyUnits[i]->getType().groundWeapon();
				_outputArray[ENEMY_DPF+i] = weapon.damageAmount() / (double)weapon.damageCooldown();
				_outputArray[ENEMY_HP+i] = _enemyUnits[i]->getHitPoints();
			}else{
				_outputArray[ENEMY_DISTANCE+i] = 10000;
				_outputArray[ENEMY_DPF+i] = 0;
				_outputArray[ENEMY_HP] = 0;
			}
		}
		return _outputArray;
	}

	/*Unit* Input::getEnemyUnit(int index){
		if (index < (int)_enemyUnits.size()){
			return _enemyUnits[index];
		}
		else{
			return 0;
		}
	}*/
}