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
		// get the maximum hp of ally units 
		_allyMaxHP = 0;
		// get the initial center distance between ally units and enemy units
		_ally_center_x = 0;
		_ally_center_y = 0;
		_enemy_center_x = 0;
		_enemy_center_y = 0;
		// get normalized base
		_allyMaxDPF = 0;
		_enemyUnitMaxDPF = 0;
		_enemyUnitMaxHP = 0;
		
		for(int i = 0; i < _allyNum; i++){
			_allyUnits.push_back(allyUnitsVec[i]);
			_allyMaxHP += allyUnitsVec[i]->getHitPoints();
			BWAPI::WeaponType weapon = allyUnitsVec[i]->getType().groundWeapon();
			_allyMaxDPF += weapon.damageAmount() / (double)weapon.damageCooldown();
			_ally_center_x += allyUnitsVec[i]->getPosition().x();
			_ally_center_y += allyUnitsVec[i]->getPosition().y();
		}
		for(int i = 0; i < _enemyNum; i++) {
			_enemyUnits.push_back(enemyUnitsVec[i]);
			if(_enemyUnitMaxHP < enemyUnitsVec[i]->getHitPoints()){
				_enemyUnitMaxHP = enemyUnitsVec[i]->getHitPoints();
			}
			BWAPI::WeaponType weapon = enemyUnitsVec[i]->getType().groundWeapon();
			if(_enemyUnitMaxDPF < weapon.damageAmount() / (double)weapon.damageCooldown()){
				_enemyUnitMaxDPF = weapon.damageAmount() / (double)weapon.damageCooldown();
			}
			_enemy_center_x += enemyUnitsVec[i]->getPosition().x();
			_enemy_center_y += enemyUnitsVec[i]->getPosition().y();
		}
		_init_central_dist = sqrt(pow((_ally_center_x / allyUnitsVec.size() - _enemy_center_x / enemyUnitsVec.size()), 2.0) + 
			pow((_ally_center_y / allyUnitsVec.size() - _enemy_center_y / enemyUnitsVec.size()), 2.0));
		
		
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
		_outputArray[ALLY_HP] = _allyTotalHealth / _allyMaxHP;
		_outputArray[ALLY_DPF] = _allyDPF / _allyMaxDPF;

		for (int i = 0; i < _enemyUnits.size(); i++){
			if (_enemyUnits[i]->exists()){
				_outputArray[ENEMY_DISTANCE+i] = _enemyUnits[i]->getDistance(_allyCenter) / _init_central_dist;
				BWAPI::WeaponType weapon = _enemyUnits[i]->getType().groundWeapon();
				_outputArray[ENEMY_DPF+i] = weapon.damageAmount() / (double)weapon.damageCooldown()  / _enemyUnitMaxDPF;
				_outputArray[ENEMY_HP+i] = _enemyUnits[i]->getHitPoints() / _enemyUnitMaxHP;
			}else{
				_outputArray[ENEMY_DISTANCE+i] = 0;
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