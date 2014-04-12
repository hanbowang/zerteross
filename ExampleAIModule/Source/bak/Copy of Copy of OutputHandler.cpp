#include "OutputHandler.h"
#include <algorithm>
#include <math.h>
#include <queue>

namespace BWAPI{
	OutputHandler::OutputHandler(double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist){
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
						// denormalize the relative distance
						d = outputArray[outputIndex + 1] * initCentralDist;
						// denormalize the angle
						angle = outputArray[outputIndex + 2] * 2 * PI - PI;
						dest_x = allyUnits[i]->getPosition().x() + int(d * cos(angle) + 0.5);
						dest_y = allyUnits[i]->getPosition().y() + int(d * sin(angle) + 0.5);
						allyUnits[i]->move(BWAPI::Position(dest_x, dest_y));
						game->sendText("Ally unit #%d will move to (%d, %d)", i, dest_x, dest_y);
					} else {
						allyUnits[i]->attack(target);
						game->sendText("Ally unit #%d will attack %d", i, target->getType().getID());
					}
				}
			}
		}
	}
}

	