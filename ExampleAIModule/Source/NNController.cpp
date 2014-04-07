#include "NNController.h"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include "Input.h"

namespace BWAPI{
	NNController::NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename){
		
		// initialize the players
		self = selfPlayer;
		enemy = enemyPlayer;

		// read configuration from nncontroller_config.ini
		configFile = filename;

		// get population and result file name, number of generations, organisms and round
		char pResult[255];
		GetPrivateProfileString("General",  "popFileName", "", pResult, 255, configFile.c_str());
		popFileName = pResult;

		GetPrivateProfileString("General",  "resultFileName", "", pResult, 255, configFile.c_str());
		resultFileName = pResult;

		gens = GetPrivateProfileInt("General", "gens", 0, configFile.c_str());
		organisms = GetPrivateProfileInt("General", "organisms", 0, configFile.c_str());
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
		int reset = GetPrivateProfileInt("General",  "reset", 0, configFile.c_str());
		if(reset == 0){
			curgen = GetPrivateProfileInt("General", "curgen", 0, configFile.c_str());
			curorg = GetPrivateProfileInt("General", "curorg", 0, configFile.c_str());
			curround = GetPrivateProfileInt("General", "curround", 0, configFile.c_str());
			maxHP = GetPrivateProfileInt("General", "maxHP", 0, configFile.c_str());

			// if this is the begin of next generation, reset the curorg, organisms and curround
			if(curgen < popgen){
				curgen = popgen;
				curorg = 0;
				organisms = orgs;
				curround = 0;
				std::stringstream ss;
				ss << curgen;
				WritePrivateProfileString("General", "curgen", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << curorg;
				WritePrivateProfileString("General", "curorg", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << organisms;
				WritePrivateProfileString("General", "organisms", ss.str().c_str(), configFile.c_str());
				ss.clear();
				ss << curround;
				WritePrivateProfileString("General", "curround", ss.str().c_str(), configFile.c_str());
				ss.clear();
			}
		} else {
			curgen = 1;
			WritePrivateProfileString("General", "curgen", "1", configFile.c_str());
			curorg = 0;
			WritePrivateProfileString("General", "curorg", "0", configFile.c_str());
			curround = 0;
			WritePrivateProfileString("General", "curround", "0", configFile.c_str());
			maxHP = 0;
			for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
				maxHP += (*i)->getType().maxHitPoints();
			}
			std::stringstream MHPs;
			MHPs << maxHP;
			WritePrivateProfileString("General", "maxHP", MHPs.str().c_str(), configFile.c_str());

			// reset the reset mark
			WritePrivateProfileString("General", "reset", "0", configFile.c_str());
		}

		// generate population from population file and retrieve neural network
		pop = new NEAT::Population(popFileName.c_str());
		if(curorg < organisms){
			net = pop->organisms[curorg]->net;
		}else{
			net = pop->organisms[organisms-1]->net;
		}
	}

	/*void NNController::initEnemyUnits(std::set<BWAPI::Unit*> enemyUnitsIn){
		enemyUnits = enemyUnitsIn;
	}*/

	void NNController::takeAction(){

		Input* input = new Input(self->getUnits(), enemy->getUnits());
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

	void NNController::log(){
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
				oFile = new std::ofstream(resultFileName.c_str());
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
				WritePrivateProfileString("General", "curorg", ss.str().c_str(), configFile.c_str());
				WritePrivateProfileString("General", "curround", "0", configFile.c_str());
			}  else {
				curround += 1;
				std::stringstream ss;
				ss << curround;
				WritePrivateProfileString("General", "curround", ss.str().c_str(), configFile.c_str());
			}
		}

		// delete pop object
		delete pop;
		pop = NULL;

	}
}