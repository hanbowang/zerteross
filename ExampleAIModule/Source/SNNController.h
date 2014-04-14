#ifndef SNNCONTROLLER_H
#define SNNCONTROLLER_H

#include <BWAPI.h>
//#include "neat.h"
#include "population.h"
#include "network.h"
#include "nnode.h"
#include "NNController.h"



namespace BWAPI{
	//class Input;
	class SNNController : public NNController
	{
	public:
		//NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename);
		SNNController(BWAPI::Player* selfPlayer, std::string filename);
		//void initEnemy(BWAPI::Player* enemyPlayer);
		void initUnits(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer);
		void takeAction();
		void log(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer);
		enum InputType{
			BIAS,
			SELF_HP,
			BIN_ATTR
		};
		

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
		// static const int PIXELBASE = 19;
		NEAT::Population* pop;
		NEAT::Network* net;
		BWAPI::Player* self;
		BWAPI::Player* enemy;
		//BWAPI::Input* input;
		// std::vector<BWAPI::Unit*> allyUnits;
		// std::vector<BWAPI::Unit*> enemyUnits;
		std::string configFile;
		// std::string NNType;
		
		void controlUnit(BWAPI::Unit* unit);
		double* getInputArray(BWAPI::Unit* unit, std::vector<Unit*> *binVecs[]);
		double getDPF(BWAPI::Unit* unit, BWAPI::Unit* target);
		double DPFdRate(BWAPI::Unit* unit, double binDPF, int binHP, int sHP, int mHP, int lHP);
		double SNNController::getAngle(BWAPI::Unit* a, BWAPI::Unit* b);
		double SNNController::moveRadius(BWAPI::Unit* unit, int sec);
	};
}

#endif
