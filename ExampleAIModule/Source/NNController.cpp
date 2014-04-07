#include "NNController.h"
#include <fstream>
#include <Windows.h>
#include "Input.h"

namespace BWAPI{
	NNController::NNController(std::string filename){
		// std::ifstream iFile(CONFIGFILE);
		// read configuration from nncontroller_config.ini
		configFile = filename;
		char pResult[255];
		GetPrivateProfileString("General",  "popFileName", "", pResult, 255, configFile.c_str());
		popFileName = pResult;
		curgen = GetPrivateProfileInt("General", "curgen", 0, configFile.c_str());
		gens = GetPrivateProfileInt("General", "gens", 0, configFile.c_str());
		curorg = GetPrivateProfileInt("General", "curorg", 0, configFile.c_str());
		organisms = GetPrivateProfileInt("General", "organisms", 0, configFile.c_str());
		curround = GetPrivateProfileInt("General", "curround", 0, configFile.c_str());
		rounds = GetPrivateProfileInt("General", "rounds", 0, configFile.c_str());

		// generate population from population file and retrieve neural network
		pop = new NEAT::Population(popFileName.c_str());
		net = pop->organisms[curorg]->net;

		// give allyUnits and enemyUnits default values
		/*allyUnits = 0;
		enemyUnits = 0;*/
	}

	void NNController::unitsInit(std::set<BWAPI::Unit*> allyUnitsIn, std::set<BWAPI::Unit*> enemyUnitsIn){
		allyUnits = allyUnitsIn;
		enemyUnits = enemyUnitsIn;
	}

	void NNController::takeAction(){

		Input* input = new Input(allyUnits, enemyUnits);
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

			for(std::set<Unit*>::const_iterator i=allyUnits.begin();i!=allyUnits.end();i++){
				/*for(std::set<Unit*>::const_iterator j=Broodwar->enemy()->getUnits().begin();j!=Broodwar->enemy()->getUnits().end();j++) {
				if(targetIndex == k++) {
				(*i)->attack(*j);
				}
				}*/
				if(input->getEnemyUnit(targetIndex)) {
					(*i)->attack(input->getEnemyUnit(targetIndex));
					Broodwar->sendText("the type is: %d", input->getEnemyUnit(targetIndex)->getID());
				}
			}

		}

	}
}