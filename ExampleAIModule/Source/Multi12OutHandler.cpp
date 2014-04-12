#include "Multi12OutHandler.h"
#include <algorithm>
#include <math.h>
#include <queue>

namespace BWAPI{
	Multi12OutHandler::Multi12OutHandler(double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist){
		int allyNum = allyUnits.size();
		int enemyNum = enemyUnits.size();
		// full size including attack scores for each ally unit enemy unit pair and move score/destination for each ally units
		int outputFullSize = allyNum * enemyNum + allyNum * 3;
		// mini size including attack scores for each enemy unit
		int outputMiniSize = enemyNum;
		double maxScore = -10000.0;
		BWAPI::Unit* target = enemyUnits[0];
		int outputIndex;
		int dest_x;
		int dest_y;
		double d;
		double angle;
		double PI = 3.14159;

		for(int i = 0; i < outputSize; i++){
			game->sendText("output %d: %f", i, outputArray[i]);
		}

		for(int i = 0; i < enemyNum; i++){
			if(enemyUnits[i]->exists() && maxScore < outputArray[i]){
				maxScore = outputArray[i];
				target = enemyUnits[i];
			}
		}
		for(int i = 0; i < allyNum; i++){
			if(allyUnits[i]->exists()){
				allyUnits[i]->attack(target);
			}
		}
	}
}

	