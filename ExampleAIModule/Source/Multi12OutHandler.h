#ifndef MULTI12OUTHANDLER_H
#define MULTI12OUTHANDLER_H

#include <BWAPI.h>
#include "OutputHandler.h"

namespace BWAPI{

	class Multi12OutHandler : public OutputHandler
{
public:
	Multi12OutHandler(double outputArray[], int outputSize, std::vector<Unit*> allyUnits, std::vector<Unit*> enemyUnits, BWAPI::Game* game, double initCentralDist);	
};
}

#endif