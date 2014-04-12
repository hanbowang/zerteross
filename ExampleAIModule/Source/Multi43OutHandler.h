#ifndef MULTI43OUTHANDLER_H
#define MULTI43OUTHANDLER_H

#include "OutputHandler.h"
#include <BWAPI.h>

namespace BWAPI{

	class Multi43OutHandler : public OutputHandler
{
public:
	Multi43OutHandler(double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist);	
};
}

#endif