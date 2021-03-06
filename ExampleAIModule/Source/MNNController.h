#ifndef MNNCONTROLLER_H
#define MNNCONTROLLER_H

#include <BWAPI.h>
//#include "neat.h"
#include "population.h"
#include "network.h"
#include "nnode.h"
//#include "Input.h"
#include "InputHandler.h"
#include "NNController.h"



namespace BWAPI{
	//class Input;
	class MNNController : public NNController
	{
	public:
		//NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename);
		MNNController(BWAPI::Player* selfPlayer, std::string filename);
		//void initEnemy(BWAPI::Player* enemyPlayer);
		void initUnits(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer);
		void takeAction();
		void log(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer);
		

	// private:
	// 	std::string popFileName;
	// 	std::string resultFileName;
	// 	int curgen;
	// 	int gens;
	// 	int curorg;
	// 	int organisms;
	// 	int curround;
	// 	int rounds;
	// 	int maxHP;
	// 	NEAT::Population* pop;
	// 	NEAT::Network* net;
	// 	BWAPI::Player* self;
	// 	BWAPI::Player* enemy;
	// 	//BWAPI::Input* input;
	// 	BWAPI::InputHandler* inputHandler;
	// 	//std::set<BWAPI::Unit*> allyUnits;
	// 	//std::set<BWAPI::Unit*> enemyUnits;
	// 	std::string configFile;
	// 	std::string NNType;

		
	};



}

#endif
