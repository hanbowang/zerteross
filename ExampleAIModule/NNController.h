#ifndef NNCONTROLLER_H
#define NNCONTROLLER_H

#include <BWAPI.h>
//#include "population.h"
//#include "network.h"


namespace BWAPI{

	class NNController
	{
	public:
		NNController();
		void init(std::set<BWAPI::Unit*> allyUnitsIn, std::set<BWAPI::Unit*> enemyUnitsIn);
		void takeAction();
		void log();

	private:
		std::string configFileName;
		std::string popFileName;
		int curgen;
		int gens;
		int curorg;
		int organisms;
		int curround;
		int rounds;
		NEAT::Population *pop;
		NEAT::Network *net;
		std::set<BWAPI::Unit*> allyUnits;
		std::set<BWAPI::Unit*> enemyUnits;

	};


}

#endif
