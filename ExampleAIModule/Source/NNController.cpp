#include "NNController.h"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include "Input.h"

namespace BWAPI{
	//NNController::NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename){
	NNController::NNController(BWAPI::Player* selfPlayer, std::string filename){

		// initialize the players
		self = selfPlayer;
		//enemy = enemyPlayer;

		// read configuration from nncontroller_config.ini
		configFile = filename;

		// get population and result file name, number of generations, organisms and round
		char pResult[255];
		GetPrivateProfileString("General",  "popFileName", "", pResult, 255, configFile.c_str());
		popFileName = pResult;

		GetPrivateProfileString("General",  "resultFileName", "", pResult, 255, configFile.c_str());
		resultFileName = pResult;

		gens = GetPrivateProfileInt("General", "gens", 0, configFile.c_str());
		organisms = GetPrivateProfileInt("nncontroller", "organisms", 0, configFile.c_str());
		rounds = GetPrivateProfileInt("General", "rounds", 0, configFile.c_str());

		// read the current gen and organism number from the population.txt
		std::ifstream iFile(popFileName.c_str());
		char curword[20];
		int popgen;
		int orgs;
		iFile >> curword;
		if(strcmp(curword, "/*") == 0) {
			iFile >> popgen >> orgs;
		}
		iFile.close();

		// reset the counters / get current state of the counters
		int reset = GetPrivateProfileInt("nncontroller",  "reset", 0, configFile.c_str());
		if(reset == 0){
			curgen = GetPrivateProfileInt("nncontroller", "curgen", 0, configFile.c_str());
			curorg = GetPrivateProfileInt("nncontroller", "curorg", 0, configFile.c_str());
			curround = GetPrivateProfileInt("nncontroller", "curround", 0, configFile.c_str());
			maxHP = GetPrivateProfileInt("nncontroller", "maxHP", 0, configFile.c_str());

			// if this is the begin of next generation, reset the curorg, organisms and curround
			if(curgen < popgen){
				curgen = popgen;
				curorg = 0;
				organisms = orgs;
				curround = 0;
				std::stringstream ss;
				ss << curgen;
				WritePrivateProfileString("nncontroller", "curgen", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << curorg;
				WritePrivateProfileString("nncontroller", "curorg", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << organisms;
				WritePrivateProfileString("nncontroller", "organisms", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << curround;
				WritePrivateProfileString("nncontroller", "curround", ss.str().c_str(), configFile.c_str());
				ss.clear();
			}
		} else {
			curgen = 1;
			WritePrivateProfileString("nncontroller", "curgen", "1", configFile.c_str());
			curorg = 0;
			WritePrivateProfileString("nncontroller", "curorg", "0", configFile.c_str());
			curround = 0;
			WritePrivateProfileString("nncontroller", "curround", "0", configFile.c_str());
			maxHP = 0;
			for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
				maxHP += (*i)->getType().maxHitPoints();
			}
			std::stringstream MHPs;
			MHPs << maxHP;
			WritePrivateProfileString("nncontroller", "maxHP", MHPs.str().c_str(), configFile.c_str());
			MHPs.clear();

			// reset the reset mark
			WritePrivateProfileString("nncontroller", "reset", "0", configFile.c_str());
		}

		// generate population from population file and retrieve neural network
		pop = new NEAT::Population(popFileName.c_str());
		if(curorg < organisms){
			net = pop->organisms[curorg]->net;
		}else{
			net = pop->organisms[organisms-1]->net;
		}
	}

	//void NNController::initEnemy(BWAPI::Player* enemyPlayer){
	//	enemy = enemyPlayer;
	//	//enemyUnits = enemyUnitsIn;
	//}

	void NNController::initUnits(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer){
		self = selfPlayer;
		enemy = enemyPlayer;
		input = new Input(self->getUnits(), enemy->getUnits());
		//enemyUnits = enemyUnitsIn;
	}

	void NNController::takeAction(){

		double* inputArray = input->getInputArray();
		net->load_sensors(inputArray);

		int net_depth = net->max_depth();
		int relax;
		bool success = net->activate();
		double this_out;

		if(success) {
			Broodwar->sendText("network activated");
		}
		for(relax = 0; relax < net_depth; relax++) {
			success=net->activate();
			for(std::vector<NEAT::NNode*>::const_iterator i=net->outputs.begin();i != net->outputs.end();i++){
				this_out = (*i)->activation;
			}


			double outputArray[3];
			int j = 0;
			double maxScore = -10000;
			int targetIndex = 0;
			for(std::vector<NEAT::NNode*>::const_iterator i=net->outputs.begin();i != net->outputs.end();i++){
				if(maxScore < (*i)->activation) {
					maxScore = (*i)->activation;
					targetIndex = j;
				}
				outputArray[j++] = (*i)->activation;
			}

			net->flush();
			Broodwar->sendText("The target is: %d", targetIndex);
			for(int i = 0; i < 3; i++) {
				Broodwar->sendText("%d th output is: %f", i+1, outputArray[i]);
			}

			for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
				
				if(input->getEnemyUnit(targetIndex)) {
					(*i)->attack(input->getEnemyUnit(targetIndex));
					Broodwar->sendText("the type is: %d", input->getEnemyUnit(targetIndex)->getID());
				}
			}

		}

		Broodwar->sendText("=============================");
	}

	void NNController::log(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer){

		// get players
		self = selfPlayer;
		enemy = enemyPlayer;

		// get combat result
		int allyHP = 0;
		int enemyHP = 0;
		for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
			allyHP += (*i)->getHitPoints();
		}
		for(std::set<Unit*>::const_iterator i=enemy->getUnits().begin();i!=enemy->getUnits().end();i++){
			enemyHP += (*i)->getHitPoints();
		}

		// if this is a valid match, log the results and update counters
		if(curorg < organisms){
			// log combat result
			// if this is the first round overwirte the result file, otherwise append to it
			std::ofstream* oFile;
			if(curround == 0){
				oFile = new std::ofstream(resultFileName.c_str());
			} else {
				oFile = new std::ofstream(resultFileName.c_str(), std::ofstream::app);
			}
			*oFile << curgen << " " << curorg << " " << curround << " " << allyHP << " " << enemyHP << " " << maxHP << std::endl;
			(*oFile).close();
			delete oFile;

			// update counters
			if(curround == rounds - 1){
				curorg += 1;
				curround = 0;
				
				std::stringstream ss;
				ss << curorg;
				WritePrivateProfileString("nncontroller", "curorg", ss.str().c_str(), configFile.c_str());
				WritePrivateProfileString("nncontroller", "curround", "0", configFile.c_str());
				ss.clear();
			}  else {
				curround += 1;
				std::stringstream ss;
				ss << curround;
				WritePrivateProfileString("nncontroller", "curround", ss.str().c_str(), configFile.c_str());
				ss.clear();
			}
		}
		
		// delete input object
		delete input;
		input = NULL;

		// delete pop object
		delete pop;
		pop = NULL;

	}
}