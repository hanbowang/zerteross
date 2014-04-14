#include "SNNController.h"
#include <fstream>
#include <sstream>
#include <Windows.h>
//#include "Input.h"

namespace BWAPI{
	//NNController::NNController(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer, std::string filename){
	SNNController::SNNController(BWAPI::Player* selfPlayer, std::string filename){

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

		// GetPrivateProfileString("nncontroller",  "NNType", "", pResult, 255, configFile.c_str());
		// NNType = pResult;

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
				std::stringstream ss1;
				ss1 << curgen;
				WritePrivateProfileString("nncontroller", "curgen", ss1.str().c_str(), configFile.c_str());
				ss1.clear();
				std::stringstream ss2;
				ss2 << curorg;
				WritePrivateProfileString("nncontroller", "curorg", ss2.str().c_str(), configFile.c_str());
				ss2.clear();
				std::stringstream ss3;
				ss3 << organisms;
				WritePrivateProfileString("nncontroller", "organisms", ss3.str().c_str(), configFile.c_str());
				ss3.clear();
				std::stringstream ss4;
				ss4 << curround;
				WritePrivateProfileString("nncontroller", "curround", ss4.str().c_str(), configFile.c_str());
				ss4.clear();
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
			std::stringstream ss1;
			ss1 << maxHP;
			WritePrivateProfileString("nncontroller", "maxHP", ss1.str().c_str(), configFile.c_str());
			ss1.clear();

			std::stringstream ss2;
			organisms = orgs;
			//std::cout << "orgs is: " << orgs << " and the organisms is: " << organisms << std::endl;
			ss2 << organisms;
			WritePrivateProfileString("nncontroller", "organisms", ss2.str().c_str(), configFile.c_str());
			ss2.clear();
			//std::cout << "organisms in ini is: " << GetPrivateProfileInt("nncontroller", "organisms", 0, configFile.c_str());

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

	void SNNController::initUnits(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer){
		self = selfPlayer;
		enemy = enemyPlayer;
		//input = new Input(self->getUnits(), enemy->getUnits());
		// BWAPI::HandlerFactory* factory = new  BWAPI::HandlerFactory();
		// inputHandler = factory->createInHandler(NNType, self->getUnits(), enemy->getUnits());
		// delete factory;
		// factory = NULL;
		//inputHandler = new InputHandler(self->getUnits(), enemy->getUnits());
		//enemyUnits = enemyUnitsIn;
	}

	void SNNController::takeAction(){
		
		for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
			if ((*i)->exists())
			{
				controlUnit((*i));
			}
		}

	}

	//double* SNNController::getInputArray(BWAPI::Unit* unit, std::vector<Unit*>* binVecs[]){
	//	// construct inputarray, assign bias and self hp
	//	double* inputArray = new double[38];
	//	inputArray[BIAS] = 1.0;
	//	inputArray[SELF_HP] = (double)unit->getHitPoints() / unit->getType().maxHitPoints();

	//	

	//	// for each bin, calculate attributes and assign them to the inputarray
	//	for (int i = 0; i < 18; ++i)
	//	{
	//		double binDPF = 0;	// current DPF sum of the bin
	//		double binDPFvsUnit = 0;	// the DPF sum if all the enemy units in the bin focus on this unit
	//		int binHP = 0;
	//		int sHP = 0;	// HP sum of small unit in the bin
	//		int mHP = 0;	// HP sum of medium unit in the bin
	//		int lHP = 0;	// HP sum of large unit in the bin
	//		for (int j = 0; j < binVecs[i]->size(); ++j)
	//		{
	//			if(binVecs[i][j]->exists()){
	//				binDPF += getDPF(binVecs[i][j], binVecs[i][j]->getTarget());
	//				binDPFvsUnit += getDPF(binVecs[i][j], unit);
	//				std::string size = binVecs[i][j]->getType().size().getName();
	//				int unitHP = binVecs[i][j]->getHitPoints();
	//				binHP += unitHP;
	//				if(size == "Small"){
	//					sHP += unitHP;
	//				} else if(size == "Medium"){
	//					mHP += unitHP;
	//				} else {
	//					lHP += unitHP;
	//				}
	//			}
	//		}
	//		inputArray[BIN_ATTR + i * 2] = DPFdRate(unit, binDPF, binHP, sHP, mHP, lHP);
	//		inputArray[BIN_ATTR + i * 2 + 1] = binDPF / unit->getType().maxHitPoints();
	//	}
	//	return inputArray;
	//}

	double SNNController::DPFdRate(BWAPI::Unit* unit, double binDPF, int binHP, int sHP, int mHP, int lHP){
		/* calculate the DPF decreasing rate of the specified bin */
		double attFactor = 1;
		double sFactor = (double)sHP / binHP;
		double mFactor = (double)mHP / binHP;
		double lFactor = (double)lHP / binHP;
		double unitDPF = (double)unit->getType().groundWeapon().damageAmount() / unit->getType().groundWeapon().damageCooldown();
		if(unit->getType().groundWeapon().damageType().getName() == "Explosive"){
			attFactor = sFactor * 0.5 + mFactor * 0.75 + lFactor * 1;
		} else if(unit->getType().groundWeapon().damageType().getName() == "Concussive"){
			attFactor = sFactor * 1 + mFactor * 0.5 + lFactor * 0.25;
		}
		return binDPF * unitDPF * attFactor / binHP;
	}

	double SNNController::getDPF(BWAPI::Unit* unit, BWAPI::Unit* target){
		if(target != NULL && target->exists()){
			double attFactor = 1;
			if(unit->getType().groundWeapon().damageType().getName() == "Explosive"){
				if(target->getType().size().getName() == "Medium"){
					attFactor = 0.75;
				} else if(target->getType().size().getName() == "Small"){
					attFactor = 0.5;
				}
			} else if(unit->getType().groundWeapon().damageType().getName() == "Concussive"){
				if(target->getType().size().getName() == "Large"){
					attFactor = 0.25;
				} else if(target->getType().size().getName() == "Medium"){
					attFactor = 0.5;
				}
			}
			return attFactor * unit->getType().groundWeapon().damageAmount() / unit->getType().groundWeapon().damageCooldown();
		} else {
			return 0;
		}
		
	}

	double SNNController::getAngle(BWAPI::Unit* a, BWAPI::Unit* b){
		double PI = 3.14159;
		int delta_x = b->getPosition().x() - a->getPosition().x();
		int delta_y = b->getPosition().y() - a->getPosition().y();
		double hypotenuse = sqrt(pow(delta_x, 2.0) + pow(delta_y, 2.0));
		// normalize to (0, 1)
		return (asin(delta_y / hypotenuse) + PI) / 2 / PI;
	}

	double SNNController::moveRadius(BWAPI::Unit* unit, int sec){
		return 18.75 * unit->getType().topSpeed() * sec;
	}

	void SNNController::controlUnit(BWAPI::Unit* unit){

		// generate bin vectors
		std::vector<BWAPI::Unit*> *binVecs[18];
		for (int i = 0; i < 18; ++i)
		{
			std::vector<BWAPI::Unit*> unitVec;
			binVecs[i] = &unitVec;
		}
		double moveR1 = moveRadius(unit, 1);
		double moveR3 = moveRadius(unit, 3);
		double moveR5 = moveRadius(unit, 5);

		// find the bin each enemy unit belongs to
		for(std::set<Unit*>::const_iterator i=enemy->getUnits().begin();i!=enemy->getUnits().end();i++){
			if ((*i)->exists() && (*i)->getDistance(unit) <= moveR5)
			{
				int dist = (*i)->getDistance(unit);
				double angle = getAngle(unit, (*i));
				// classify the distance and angle to calculate bin index
				int dIndex = 0;
				int angleIndex = 0;
				if(dist <= moveR1){
					dIndex = 0;
				} else if(dist <= moveR3){
					dIndex = 1;
				} else {
					dIndex = 2;
				}
				if(angle <= 0.167){
					angleIndex = 0;
				} else if(angle <= 0.333){
					angleIndex = 1;
				} else if(angle <= 0.5){
					angleIndex = 2;
				} else if(angle <= 0.667){
					angleIndex = 3;
				} else if(angle <= 0.833){
					angleIndex = 4;
				} else {
					angleIndex = 5;
				}
				int binIndex = dIndex * 6 + angleIndex;
				binVecs[binIndex]->push_back(*i);
			}
		}

		//double* inputArray = input->getInputArray();
		// double* inputArray = inputHandler->getInputArray();
		// double* inputArray = getInputArray(unit, binVecs);
		// get inputArray
		// construct inputarray, assign bias and self hp
		double* inputArray = new double[38];
		inputArray[BIAS] = 1.0;
		inputArray[SELF_HP] = (double)unit->getHitPoints() / unit->getType().maxHitPoints();

		

		// for each bin, calculate attributes and assign them to the inputarray
		for (int i = 0; i < 18; ++i)
		{
			double binDPF = 0;	// current DPF sum of the bin
			double binDPFvsUnit = 0;	// the DPF sum if all the enemy units in the bin focus on this unit
			int binHP = 0;
			int sHP = 0;	// HP sum of small unit in the bin
			int mHP = 0;	// HP sum of medium unit in the bin
			int lHP = 0;	// HP sum of large unit in the bin
			for (int j = 0; j < binVecs[i]->size(); ++j)
			{
				if((*binVecs[i])[j]->exists()){
					binDPF += getDPF((*binVecs[i])[j], (*binVecs[i])[j]->getTarget());
					binDPFvsUnit += getDPF((*binVecs[i])[j], unit);
					std::string size = (*binVecs[i])[j]->getType().size().getName();
					int unitHP = (*binVecs[i])[j]->getHitPoints();
					binHP += unitHP;
					if(size == "Small"){
						sHP += unitHP;
					} else if(size == "Medium"){
						mHP += unitHP;
					} else {
						lHP += unitHP;
					}
				}
			}
			inputArray[BIN_ATTR + i * 2] = DPFdRate(unit, binDPF, binHP, sHP, mHP, lHP);
			inputArray[BIN_ATTR + i * 2 + 1] = binDPF / unit->getType().maxHitPoints();
		}

		// load the input to nn
		net->load_sensors(inputArray);

		//if(Broodwar->getFrameCount() == 10){
		//	//std::ofstream ofile("C:\\Program Files\\StarCraft\\bwapi-data\\AI\\input.txt");
		//	//std::ofstream ofile("F:\\Games\\StarCraft00\\bwapi-data\\AI\\input.txt");
		//	for(int i = 0; i < 12; i++){
		//		ofile << inputArray[i] << std::endl;
		//	}
		//	ofile.close();
		//}


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
		}

		int outputSize = net->outputs.size();
		double* outputArray = new double[outputSize];
		//double outputArray[3];
		int j = 0;
		//double maxScore = -10000;
		//int targetIndex = 0;
		for(std::vector<NEAT::NNode*>::const_iterator i=net->outputs.begin();i != net->outputs.end();i++){
			/*if(maxScore < (*i)->activation) {
				maxScore = (*i)->activation;
				targetIndex = j;
			}*/
			outputArray[j++] = (*i)->activation;
		}

		net->flush();

		// translateOutput(*outputArray);
		// find the target bin
		double maxScore = -10000.0;
		int targetIndex = 0;
		for (int i = 2; i < 20; ++i)
		{
			if(maxScore < outputArray[i]){
				maxScore = outputArray[i];
				targetIndex = i - 2;
			}
		}

		// compare scores of attack and move
		double attScore = outputArray[0];
		double moveScore = outputArray[1];
		if(attScore >= moveScore){
			// find a target enemy unit with lowest HP
			std::string dmType = unit->getType().groundWeapon().damageType().getName();
			BWAPI::Unit* target = (*binVecs[targetIndex])[0];
			int minHP = 1000;
			for (std::vector<BWAPI::Unit*>::const_iterator i = binVecs[targetIndex]->begin(); i != binVecs[targetIndex]->end(); i++)
			{
				if((*i)->exists() && minHP > (*i)->getHitPoints()){
					minHP = (*i)->getHitPoints();
					target = (*i);
				}
			}
			unit->attack(target);
		} else {
			// find the central position of the bin
			double targetAngle = ((double)(targetIndex % 6) * 0.167 + 0.084) * 2 * 3.14159;
			double targetD = 0;
			if(targetIndex / 6 == 0){
				targetD = moveR1 / 2;
			} else if(targetIndex / 6 == 1){
				targetD = (moveR1 + moveR3) / 2;
			} else {
				targetD = (moveR3 + moveR5) / 2;
			}

			int dest_x = unit->getPosition().x() + int(targetD * cos(targetAngle) + 0.5);
			int dest_y = unit->getPosition().y() + int(targetD * sin(targetAngle) + 0.5);
			unit->move(BWAPI::Position(dest_x, dest_y));
		}

		// BWAPI::HandlerFactory* factory = new  BWAPI::HandlerFactory();
		// BWAPI::OutputHandler* outputHandler = factory->createOutHandler(NNType, outputArray, outputSize, inputHandler->getAllyUnits(), inputHandler->getEnemyUnits(), Broodwar, inputHandler->getInitCentralDist());
		// delete factory;
		// factory = NULL;
		delete inputArray;
		inputArray = NULL;
		/*delete outputHandler;
		outputHandler = NULL;*/
		delete outputArray;
		outputArray = NULL;
		//Broodwar->sendText("The target is: %d", targetIndex);
		//for(int i = 0; i < 3; i++) {
		//	Broodwar->sendText("%d th output is: %f", i+1, outputArray[i]);
		//}

		/*for(std::set<Unit*>::const_iterator i=self->getUnits().begin();i!=self->getUnits().end();i++){
			
			if(input->getEnemyUnit(targetIndex)) {
				(*i)->attack(input->getEnemyUnit(targetIndex));
				Broodwar->sendText("the type is: %d", input->getEnemyUnit(targetIndex)->getID());
			}
		}*/

		Broodwar->sendText("=============================");
	}

	// void SNNController::translateOutput(double ouputArray[], BWAPI::Unit* unit){
	// 	// // find the target bin
	// 	// double maxScore = -10000.0;
	// 	// int targetIndex = 0;
	// 	// for (int i = 2; i < 20; ++i)
	// 	// {
	// 	// 	if(maxScore < outputArray[i]){
	// 	// 		maxScore = outputArray[i];
	// 	// 		targetIndex = i - 2;
	// 	// 	}
	// 	// }

	// 	// // compare scores of attack and move
	// 	// double attScore = outputArray[0];
	// 	// double moveScore = outputArray[1];
	// 	// if(attScore > moveScore){
	// 	// 	unit->attack()
	// 	// }

	// }

	void SNNController::log(BWAPI::Player* selfPlayer, BWAPI::Player* enemyPlayer){

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
		} else {
			std::ofstream oFile("C:\\Program Files\\StarCraft\\bwapi-data\\AI\\combatvsrs.txt", std::ofstream::app);
			oFile << allyHP << ", " << enemyHP << std::endl;
			oFile.close();
		}


		
		// delete input object
		//delete input;
		//input = NULL;
		/*delete inputHandler;
		inputHandler = NULL;*/
		

		// delete pop object
		delete pop;
		pop = NULL;

	}
}