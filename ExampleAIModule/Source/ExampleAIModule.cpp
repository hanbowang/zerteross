#include "ExampleAIModule.h"
#include <fstream>
#include <math.h>
#include <cstring>
#include <stdlib.h>
#include "Input.h"

using namespace BWAPI;
using namespace std;
using namespace NEAT;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
NEAT::Network *net;

void ExampleAIModule::onStart()
{
	Broodwar->sendText("Hello CS5100");
	//Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	//Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	Broodwar->enableFlag(Flag::CompleteMapInformation);

	//read map information into BWTA so terrain analysis can be done in another thread
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;

	show_bullets=true;
	show_visibility_data=false;

	// Initialize the neural network
	NEAT::Population *pop = 0;
	NEAT::Genome *start_genome;
	char curword[20];
	int id;
	
	NEAT::load_neat_params("F:\\Games\\StarCraft00\\bwapi-data\\AI\\multiunit.ne",true);
	std::ifstream iFile("F:\\Games\\StarCraft00\\bwapi-data\\AI\\multiunitstartgenes", std::ios::in);

	//cout << "Start multiunit evolving" << endl;
	Broodwar->sendText("Start multiunit evolving");

	// Read in start Genome
	iFile >> curword;
	iFile >> id;
	Broodwar->sendText("Reading in Genome id: %d", id);
	start_genome = new Genome(id, iFile);
	iFile.close();
	
	// Spawning Population
	Broodwar->sendText("Spawning Population");
	pop = new Population(start_genome, NEAT::pop_size);

	pop->verify();
	Organism* org = pop->organisms[0];
	// get the nerual network
	net = org->net;
}

void ExampleAIModule::onEnd(bool isWinner)
{
	std::ofstream logFile("C:\\log.txt", std::ofstream::app);
	logFile << "====================" << std::endl;
	if(isWinner){
		logFile << "WIN ^.^" << std::endl;
		logFile << "====" << std::endl;
		logFile << "Remaining Units:" << std::endl;
		logFile << "-------------" << std::endl;
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
			unsigned int hp = (*i)->getHitPoints();
			unsigned int maxHp = (*i)->getType().maxHitPoints();
			unsigned int percentage = hp * 100 / maxHp;
			std::string name = (*i)->getType().getName();
			logFile << name << "-" << percentage << "%" << std::endl;
			logFile << "-------------" << std::endl;
		}
	}
	else{
		logFile << "LOSE T.T" << std::endl;
		logFile << "====" << std::endl;
		logFile << "Remaining Units:" << std::endl;
		logFile << "-------------" << std::endl;
		for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
			unsigned int hp = (*i)->getHitPoints();
			unsigned int maxHp = (*i)->getType().maxHitPoints();
			unsigned int percentage = hp * 100 / maxHp;
			std::string name = (*i)->getType().getName();
			logFile << name << "-" << percentage << "%" << std::endl;
			logFile << "-------------" << std::endl;
		}
	}
}

void ExampleAIModule::onFrame()
{

	if (show_visibility_data)
		drawVisibilityData();

	if (show_bullets)
		drawBullets();

	drawStats();
	
	double *inputArray;
		if(Broodwar->getFrameCount() == 1){
			Input *input = new Input(Broodwar->self()->getUnits(),Broodwar->enemy()->getUnits());
			for (int i = 0; i<11; i++){
				Broodwar->printf("%f",input->getInputArray()[i]);
			}
			inputArray = input->getInputArray();
		}

	if(Broodwar->getFrameCount() % 10 == 0 && Broodwar->getFrameCount() > 0){
		

		// load network input
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
			for(std::vector<NNode*>::const_iterator i=net->outputs.begin();i != net->outputs.end();i++){
				this_out = (*i)->activation;
			}
		}

		double outputArray[3];
		int j = 0;
		double maxScore = -10000;
		int targetIndex = 0;
		for(std::vector<NNode*>::const_iterator i=net->outputs.begin();i != net->outputs.end();i++){
			if(maxScore < (*i)->activation) {
				maxScore = (*i)->activation;
				targetIndex = j;
			}
			outputArray[j++] = (*i)->activation;
		}

		net->flush();

		for(int i = 0; i < 3; i++) {
			Broodwar->sendText("%d th output is: %f", i+1, outputArray[i]);
		}
		int k;
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
			k = 0;
			for(std::set<Unit*>::const_iterator j=Broodwar->enemy()->getUnits().begin();j!=Broodwar->enemy()->getUnits().end();j++) {
				if(targetIndex == k++) {
					(*i)->attack(*j);
				}
			}
		}




		/*std::ofstream logFile("C:\\log.txt", std::ofstream::app);
		logFile << "====================" << std::endl;*/
		int leftmost = 10000;
		//BWAPI::Unit* leftmostUnit;
			//std::string type;
		//std::hex id;
		//unsigned int hp;


		// translate outputs to actions 
		//for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
		//	for(std::set<Unit*>::const_iterator j=Broodwar->enemy()->getUnits().begin();j!=Broodwar->enemy()->getUnits().end();j++) {
		//		currentScore = outputs[allyIndex.find(std::to_string(*i)) * 6 + enemyIndex.find(std::to_string(*j))]
		//		if(currentScore > maxScore) {
		//			maxScore = currentScore;
		//			target = (*j);
		//		}
		//	}
		//	if(outputs[allyIndex.find(std::to_string(*i)) * 6 + 3] > maxScore) {
		//		angleOutput = outputs[allyIndex.find(std::to_string(*i)) * 6 + 5];
		//			dOutput = outputs[allyIndex.find(std::to_string(*i)) * 6 + 4];
		//			BWAPI::Position p(round(dOutput * cos(angleOutput)), round(dOutput * sin(angleOutput)))
		//			(*i)->move(p);
		//	} else {
		//		(*i)->attack(target);
		//	}
		//}



		/*for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){

			if ((*i)->getOrder() != Orders::AttackUnit){
				if ((*i)->getPosition().x()-leftmostUnit->getPosition().x() > 30){
					if ((*i)->getOrder() != Orders::AttackUnit){
						(*i)->stop();
					}

				}
				else{
					BWAPI::Position p((*i)->getPosition().x()+500,(*i)->getPosition().y());
					if ((*i)->getOrder() != Orders::AttackUnit){
						(*i)->move(p);
					}
				}
			}
		}*/
		//logFile << "=============================" << std::endl;
		Broodwar->sendText("=============================");
		/*logFile.flush();*/
		
	}

	if (analyzed && Broodwar->getFrameCount()%30==0)
	{
		//order one of our workers to guard our chokepoint.
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
		{
			if ((*i)->getType().isWorker())
			{
				//get the chokepoints linked to our home region
				std::set<BWTA::Chokepoint*> chokepoints= home->getChokepoints();
				double min_length=10000;
				BWTA::Chokepoint* choke=NULL;

				//iterate through all chokepoints and look for the one with the smallest gap (least width)
				for(std::set<BWTA::Chokepoint*>::iterator c=chokepoints.begin();c!=chokepoints.end();c++)
				{
					double length=(*c)->getWidth();
					if (length<min_length || choke==NULL)
					{
						min_length=length;
						choke=*c;
					}
				}

				//order the worker to move to the center of the gap
				(*i)->rightClick(choke->getCenter());
				break;
			}
		}
	}
	if (analyzed)
		drawTerrainData();

	if (analysis_just_finished)
	{
		Broodwar->printf("Finished analyzing map.");
		analysis_just_finished=false;
	}
}

void ExampleAIModule::onSendText(std::string text)
{
	if (text=="/show bullets")
	{
		show_bullets = !show_bullets;
	} else if (text=="/show players")
	{
		showPlayers();
	} else if (text=="/show forces")
	{
		showForces();
	} else if (text=="/show visibility")
	{
		show_visibility_data=!show_visibility_data;
	} else if (text=="/analyze")
	{
		if (analyzed == false)
		{
			Broodwar->printf("Analyzing map... this may take a minute");
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AnalyzeThread, NULL, 0, NULL);
		}
	} else
	{
		Broodwar->printf("You typed '%s'!",text.c_str());
		Broodwar->sendText("%s",text.c_str());
	}
}

void ExampleAIModule::onReceiveText(BWAPI::Player* player, std::string text)
{
	Broodwar->printf("%s said '%s'", player->getName().c_str(), text.c_str());
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player* player)
{
	Broodwar->sendText("%s left the game.",player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
	if (target!=Positions::Unknown)
		Broodwar->printf("Nuclear Launch Detected at (%d,%d)",target.x(),target.y());
	else
		Broodwar->printf("Nuclear Launch Detected");
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] has been discovered at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] was last accessible at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		//Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	if (unit->getPlayer()->isEnemy(Broodwar->self())){
		BWAPI::Unit* nearestUnit;
		int nearestDistance = 10000;
		for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
			int avgDistance = 0;
			for(std::set<Unit*>::const_iterator j=Broodwar->self()->getUnits().begin();j!=Broodwar->self()->getUnits().end();j++){
				avgDistance += (*i)->getDistance((*j));
			}
			avgDistance /= Broodwar->enemy()->getUnits().size();
			if (avgDistance < nearestDistance){
				nearestDistance = avgDistance;
				nearestUnit = (*i);
			}
		}
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
			(*i)->attack(nearestUnit);
		}
	}
}

void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] was last seen at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	if (Broodwar->getFrameCount()>1)
	{
		if (!Broodwar->isReplay())
			Broodwar->sendText("A %s [%x] has been created at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
		else
		{
			/*if we are in a replay, then we will print out the build order
			(just of the buildings, not the units).*/
			if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
			{
				int seconds=Broodwar->getFrameCount()/24;
				int minutes=seconds/60;
				seconds%=60;
				Broodwar->sendText("%.2d:%.2d: %s creates a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
			}
		}
	}
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] has been destroyed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	if (unit->getPlayer()->isEnemy(Broodwar->self())){
		if (!unit->getPlayer()->getUnits().empty()){
			BWAPI::Unit* nearestUnit;
			int nearestDistance = 10000;
			for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
				int avgDistance = 0;
				for(std::set<Unit*>::const_iterator j=Broodwar->self()->getUnits().begin();j!=Broodwar->self()->getUnits().end();j++){
					avgDistance += (*i)->getDistance((*j));
				}
				avgDistance /= Broodwar->enemy()->getUnits().size();
				if (avgDistance < nearestDistance){
					nearestDistance = avgDistance;
					nearestUnit = (*i);
				}
			}
			for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
				if(nearestUnit){	
					(*i)->attack(nearestUnit);
				}
			}

		}
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->sendText("A %s [%x] has been morphed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	else
	{
		/*if we are in a replay, then we will print out the build order
		(just of the buildings, not the units).*/
		if (unit->getType().isBuilding() && unit->getPlayer()->isNeutral()==false)
		{
			int seconds=Broodwar->getFrameCount()/24;
			int minutes=seconds/60;
			seconds%=60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s",minutes,seconds,unit->getPlayer()->getName().c_str(),unit->getType().getName().c_str());
		}
	}
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
	if (!Broodwar->isReplay())
		Broodwar->sendText("A %s [%x] is now owned by %s",unit->getType().getName().c_str(),unit,unit->getPlayer()->getName().c_str());
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
	Broodwar->printf("The game was saved to \"%s\".", gameName.c_str());
}

DWORD WINAPI AnalyzeThread()
{
	BWTA::analyze();

	//self start location only available if the map has base locations
	if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
	{
		home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
	}
	//enemy start location only available if Complete Map Information is enabled.
	if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
	{
		enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
	}
	analyzed   = true;
	analysis_just_finished = true;
	return 0;
}

void ExampleAIModule::drawStats()
{
	std::set<Unit*> myUnits = Broodwar->self()->getUnits();
	Broodwar->drawTextScreen(5,0,"Total Units: %d",myUnits.size());
	std::map<UnitType, int> unitTypeCounts;
	for(std::set<Unit*>::iterator i=myUnits.begin();i!=myUnits.end();i++)
	{
		if (unitTypeCounts.find((*i)->getType())==unitTypeCounts.end())
		{
			unitTypeCounts.insert(std::make_pair((*i)->getType(),0));
		}
		unitTypeCounts.find((*i)->getType())->second++;
	}
	int line=1;
	for(std::map<UnitType,int>::iterator i=unitTypeCounts.begin();i!=unitTypeCounts.end();i++)
	{
		Broodwar->drawTextScreen(5,16*line,"- %d %ss",(*i).second, (*i).first.getName().c_str());
		line++;
	}
}

void ExampleAIModule::drawBullets()
{
	std::set<Bullet*> bullets = Broodwar->getBullets();
	for(std::set<Bullet*>::iterator i=bullets.begin();i!=bullets.end();i++)
	{
		Position p=(*i)->getPosition();
		double velocityX = (*i)->getVelocityX();
		double velocityY = (*i)->getVelocityY();
		if ((*i)->getPlayer()==Broodwar->self())
		{
			Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Green);
			Broodwar->drawTextMap(p.x(),p.y(),"\x07%s",(*i)->getType().getName().c_str());
		}
		else
		{
			Broodwar->drawLineMap(p.x(),p.y(),p.x()+(int)velocityX,p.y()+(int)velocityY,Colors::Red);
			Broodwar->drawTextMap(p.x(),p.y(),"\x06%s",(*i)->getType().getName().c_str());
		}
	}
}

void ExampleAIModule::drawVisibilityData()
{
	for(int x=0;x<Broodwar->mapWidth();x++)
	{
		for(int y=0;y<Broodwar->mapHeight();y++)
		{
			if (Broodwar->isExplored(x,y))
			{
				if (Broodwar->isVisible(x,y))
					Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Green);
				else
					Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Blue);
			}
			else
				Broodwar->drawDotMap(x*32+16,y*32+16,Colors::Red);
		}
	}
}

void ExampleAIModule::drawTerrainData()
{
	//we will iterate through all the base locations, and draw their outlines.
	for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
	{
		TilePosition p=(*i)->getTilePosition();
		Position c=(*i)->getPosition();

		//draw outline of center location
		Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

		//draw a circle at each mineral patch
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
		{
			Position q=(*j)->getInitialPosition();
			Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
		}

		//draw the outlines of vespene geysers
		for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
		{
			TilePosition q=(*j)->getInitialTilePosition();
			Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
		}

		//if this is an island expansion, draw a yellow circle around the base location
		if ((*i)->isIsland())
			Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
	}

	//we will iterate through all the regions and draw the polygon outline of it in green.
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		BWTA::Polygon p=(*r)->getPolygon();
		for(int j=0;j<(int)p.size();j++)
		{
			Position point1=p[j];
			Position point2=p[(j+1) % p.size()];
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
		}
	}

	//we will visualize the chokepoints with red lines
	for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
	{
		for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
		{
			Position point1=(*c)->getSides().first;
			Position point2=(*c)->getSides().second;
			Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
		}
	}
}

void ExampleAIModule::showPlayers()
{
	std::set<Player*> players=Broodwar->getPlayers();
	for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++)
	{
		Broodwar->printf("Player [%d]: %s is in force: %s",(*i)->getID(),(*i)->getName().c_str(), (*i)->getForce()->getName().c_str());
	}
}

void ExampleAIModule::showForces()
{
	std::set<Force*> forces=Broodwar->getForces();
	for(std::set<Force*>::iterator i=forces.begin();i!=forces.end();i++)
	{
		std::set<Player*> players=(*i)->getPlayers();
		Broodwar->printf("Force %s has the following players:",(*i)->getName().c_str());
		for(std::set<Player*>::iterator j=players.begin();j!=players.end();j++)
		{
			Broodwar->printf("  - Player [%d]: %s",(*j)->getID(),(*j)->getName().c_str());
		}
	}
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit *unit)
{
	if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
		Broodwar->sendText("A %s [%x] has been completed at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
}
