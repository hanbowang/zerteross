#include "HandlerFactory.h"
#include "Multi43InHandler.h"
#include "Multi12InHandler.h"
#include "Multi43OutHandler.h"
#include "Multi12OutHandler.h"


namespace BWAPI{
	InputHandler* HandlerFactory::createInHandler(std::string handlerType, std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits){
		if(handlerType ==  "multi43"){
			return new Multi43InHandler(allyUnits, enemyUnits);
		} else if(handlerType == "multi12"){
			return new Multi12InHandler(allyUnits, enemyUnits);
		} else {
			return new Multi43InHandler(allyUnits, enemyUnits);
		}
	}

	OutputHandler* HandlerFactory::createOutHandler(std::string handlerType, double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist){
		if(handlerType == "multi43"){
			return new Multi43OutHandler(outputArray, outputSize, allyUnits, enemyUnits, game, initCentralDist);
		} else if(handlerType == "multi12"){
			return new Multi12OutHandler(outputArray, outputSize, allyUnits, enemyUnits, game, initCentralDist);
		} else {
			return new Multi43OutHandler(outputArray, outputSize, allyUnits, enemyUnits, game, initCentralDist);
		}
	}
}