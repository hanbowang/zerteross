#ifndef INPUTHANDLERFACTORY_H
#define INPUTHANDLERFACTORY_H

#include "InputHandler.h"
#include "OutputHandler.h"
#include <BWAPI.h>

namespace BWAPI{
	
  class HandlerFactory
  {
  public:
	  InputHandler* createInHandler(std::string handlerType, std::set<BWAPI::Unit*> allyUnits, std::set<BWAPI::Unit*> enemyUnits);
	  OutputHandler* createOutHandler(std::string handlerType, double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist);
  };
}




#endif