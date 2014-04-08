#include "OutputHandler.h"
#include <algorithm>
#include <math.h>
#include <queue>

namespace BWAPI{
	OutputHandler::OutputHandler(double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits){
		int allyNum = allyUnits.size();
		int enemyNum = enemyUnits.size();
		// full size including attack scores for each ally unit enemy unit pair and move score/destination for each ally units
		int outputFullSize = allyNum * enemyNum + allyNum * 3;
		// mini size including attack scores for each enemy unit
		int outputMiniSize = enemyNum;
		double maxScore = -10000.0;
		BWAPI::Unit* target = enemyUnits[0];
		int outputIndex;
		double dest_x;
		double dest_y;
		double d;
		double angle;

		if(outputSize == outputMiniSize){
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
		} else if(outputSize == outputFullSize){
			for(int i = 0; i < allyNum; i++){
				if(allyUnits[i]->exists()){
					for(int j = 0; j < enemyNum; j++){
						outputIndex = i * 6 + j;
						if(enemyUnits[j]->exists() && maxScore < outputArray[outputIndex]){
							maxScore = outputArray[outputIndex];
							target = enemyUnits[i];
						} 
					}
					outputIndex = i * 6 + 4;
					if(maxScore < outputArray[outputIndex]){
						d = outputArray[outputIndex + 1];
						angle = outputArray[outputIndex + 2];
						dest_x = allyUnits[i]->getPosition().x() + d * cos(angle);
						dest_y = allyUnits[i]->getPosition().y() + d * sin(angle);
						allyUnits[i]->move(BWAPI::Position((int)(dest_x + 0.5), (int)(dest_y + 0.5)));
					} else {
						allyUnits[i]->attack(target);
					}
				}
			}
		}
	}
}

	