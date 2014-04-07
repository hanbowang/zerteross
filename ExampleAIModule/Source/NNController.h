#ifndef NNCONTROLLER_H
#define NNCONTROLLER_H

#include <BWAPI.h>
//#include "neat.h"
#include "population.h"
#include "network.h"
#include "nnode.h"


namespace BWAPI{

	class NNController
	{
	public:
		NNController(std::string filename);
		void unitsInit(std::set<BWAPI::Unit*> allyUnitsIn, std::set<BWAPI::Unit*> enemyUnitsIn);
		void takeAction();
		void log();
		//enum NNControllerType{
		//	CONFIGFILE = "nncontroller_config.ini"
		//};
		

	private:
		std::string popFileName;
		int curgen;
		int gens;
		int curorg;
		int organisms;
		int curround;
		int rounds;
		int allyMaxHP
		NEAT::Population *pop;
		NEAT::Network *net;
		std::set<BWAPI::Unit*> allyUnits;
		std::set<BWAPI::Unit*> enemyUnits;
		std::string configFile;

		
	};



}

#endif
