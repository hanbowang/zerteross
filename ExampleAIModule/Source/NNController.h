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
		//NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename);
		NNController(BWAPI::Player* selfPlayer, std::string filename);
		void initEnemy(BWAPI::Player* enemyPlayer);
		void takeAction();
		void log();
		

	private:
		std::string popFileName;
		std::string resultFileName;
		int curgen;
		int gens;
		int curorg;
		int organisms;
		int curround;
		int rounds;
		int maxHP;
		NEAT::Population* pop;
		NEAT::Network* net;
		BWAPI::Player* self;
		BWAPI::Player* enemy;
		//std::set<BWAPI::Unit*> allyUnits;
		//std::set<BWAPI::Unit*> enemyUnits;
		std::string configFile;

		
	};



}

#endif
