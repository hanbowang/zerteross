#include <BWAPI.h>

namespace BWAPI{

class Input
{
public:
	Input(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	enum InputType{
		ALLY_HP,
		ALLY_DPF,
		ENEMY_DISTANCE,
		ENEMY_DPF = 5,
		ENEMY_HP = 8
	};
	
	double* getInputArray();
	

private:
	int _allyTotalHealth;
	double _allyDPF;
	double _outputArray[11];
	std::vector<Unit*> _enemyUnits;
	BWAPI::Position _allyCenter;
	void Input::sortUnitsVec(std::vector<BWAPI::Unit*> &v);
	bool distanceCompare(BWAPI::Unit* a, BWAPI::Unit* b);
	Unit* getEnemyUnit(int index);
};
}