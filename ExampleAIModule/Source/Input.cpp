#include "Input.h"
#include <algorithm>

namespace BWAPI{
	Input::Input(const std::set<BWAPI::Unit*> allyUnits, const std::set<BWAPI::Unit*> enemyUnits):
	_allyTotalHealth(0),
	_allyDPF(0)
	{
		
		int centerX = 0;
		int centerY = 0;
		for (std::set<Unit*>::const_iterator i=allyUnits.begin();i!=allyUnits.end();i++){
			_allyTotalHealth += (*i)->getHitPoints();
			BWAPI::WeaponType weapon = (*i)->getType().groundWeapon();
			_allyDPF += weapon.damageAmount() / (double)weapon.damageCooldown();
			centerX += (*i)->getPosition().x();
			centerY += (*i)->getPosition().y();
		}
		if(!allyUnits.empty()) {
		centerX /= allyUnits.size();
		centerY /= allyUnits.size();
		}
		BWAPI::Position allyCenter(centerX, centerY);
		_allyCenter = allyCenter;
		std::vector<BWAPI::Unit*> enemyUnitsVec(enemyUnits.begin(), enemyUnits.end());
		sortUnitsVec(enemyUnitsVec);
		
		_outputArray[BIAS] = 1.0;
		_outputArray[ALLY_HP] = _allyTotalHealth;
		_outputArray[ALLY_DPF] = _allyDPF;
	
		for (int i = 0; i < 3; i++){
			if (i < (int)enemyUnitsVec.size()){
				_enemyUnits.push_back(enemyUnitsVec[i]);
				_outputArray[ENEMY_DISTANCE+i] = enemyUnitsVec[i]->getDistance(_allyCenter);
				BWAPI::WeaponType weapon = enemyUnitsVec[i]->getType().groundWeapon();
				_outputArray[ENEMY_DPF+i] = weapon.damageAmount() / (double)weapon.damageCooldown();
				_outputArray[ENEMY_HP+i] = enemyUnitsVec[i]->getHitPoints();
			}
			else{
				_outputArray[ENEMY_DISTANCE+i] = 10000;
				_outputArray[ENEMY_DPF+i] = 0;
				_outputArray[ENEMY_HP] = 0;
			}
		}

	}

	//Input::Input(const int temp, const std::set<BWAPI::Unit*> allyUnits, const std::set<BWAPI::Unit*> enemyUnits)
	//{
	//	
	//	asdf;
	//}

	void Input::sortUnitsVec(std::vector<BWAPI::Unit*> &v){
		bool finished = false;
		
		while (!finished){
			finished = true;
			for (int i = 0; i < (int)v.size()-1; i++){
				if(v[i]->getDistance(_allyCenter) > v[i+1]->getDistance(_allyCenter)){
					std::swap(v[i],v[i+1]);
					finished = false;
				}
			}
		}
	}

	double* Input::getInputArray(){
		return _outputArray;
	}

	Unit* Input::getEnemyUnit(int index){
		if (index < (int)_enemyUnits.size()){
			return _enemyUnits[index];
		}
		else{
			return 0;
		}
	}
}