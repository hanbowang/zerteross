#include <BWAPI.h>

namespace BWAPI{

class InputFull
{
public:
	InputFull(std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	Unit* getEnemyUnit(int index);
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
	double InputFull::getAngle(BWAPI::Unit* a, BWAPI::Unit* b);
	
};
}