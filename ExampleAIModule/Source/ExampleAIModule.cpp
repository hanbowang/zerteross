#include "ExampleAIModule.h"
#include <fstream>
#include <math.h>
#include <cstring>
#include <stdlib.h>
#include "Input.h"
//#include "Logfile.h"
#include "NNController.h"
#include "SNNController.h"
#include "MNNController.h"
#include <Windows.h>

using namespace BWAPI;
using namespace std;
using namespace NEAT;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
BWAPI::NNController* controller;
//NEAT::Network *net;
//std::string AI_PATH = "F:\\Games\\StarCraft00\\bwapi-data\\AI\\";


void ExampleAIModule::onStart()
{
	Broodwar->sendText("Hello CS5100");
	cout<<"hello CS5100"<<endl;
	//Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	//Broodwar->enableFlag(Flag::UserInput);
	// Uncomment to enable complete map information
	Broodwar->enableFlag(Flag::CompleteMapInformation);
	Broodwar->setLocalSpeed(0);
	//Broodwar->setFrameSkip(5);
	//Broodwar->setGUI(false);

	//read map information into BWTA so terrain analysis can be done in another thread
	BWTA::readMap();
	analyzed=false;
	analysis_just_finished=false;

	show_bullets=true;
	show_visibility_data=false;

	// Initialize the neural network controller
	std::string configFilePath = "E:\\Games\\StarCraft00\\bwapi-data\\AI\\nncontroller_config.ini";
	// std::string configFilePath = "C:\\Program Files\\StarCraft\\bwapi-data\\AI\\nncontroller_config.ini";
	//controller = new BWAPI::NNController(Broodwar->self(), Broodwar->enemy(), configFilePath.c_str());
	char pResult[255];
	GetPrivateProfileString("nncontroller",  "NNType", "", pResult, 255, configFilePath.c_str());
	std::string NNType = pResult;
	if(NNType == "multi12" || NNType == "multi43"){
		controller = new BWAPI::MNNController(Broodwar->self(), configFilePath.c_str());
	} else {
		controller = new BWAPI::SNNController(Broodwar->self(), configFilePath.c_str());
	}
	
}

void ExampleAIModule::onEnd(bool isWinner)
{
	int allyHP = 0;
	int enemyHP = 0;
	//std::ofstream logFile("C:\\log.txt", std::ofstream::app);
	//logFile << "====================" << std::endl;
	if(isWinner){
		//logFile << "WIN ^.^" << std::endl;
		//logFile << "====" << std::endl;
		//logFile << "Remaining Units:" << std::endl;
		//logFile << "-------------" << std::endl;
		for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
			unsigned int hp = (*i)->getHitPoints();
			unsigned int maxHp = (*i)->getType().maxHitPoints();
			unsigned int percentage = hp * 100 / maxHp;
			//std::string name = (*i)->getType().getName();
			/*logFile << name << "-" << percentage << "%" << std::endl;
			logFile << "-------------" << std::endl;*/
			allyHP += hp;
		}
	}
	else{
		/*logFile << "LOSE T.T" << std::endl;
		logFile << "====" << std::endl;
		logFile << "Remaining Units:" << std::endl;
		logFile << "-------------" << std::endl;*/
		for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
			unsigned int hp = (*i)->getHitPoints();
			unsigned int maxHp = (*i)->getType().maxHitPoints();
			unsigned int percentage = hp * 100 / maxHp;
			/*std::string name = (*i)->getType().getName();
			logFile << name << "-" << percentage << "%" << std::endl;
			logFile << "-------------" << std::endl;*/
			enemyHP += hp;
		}
	}

	// log the combat result
	controller->log(Broodwar->self(), Broodwar->enemy());

	// delete the controller
	delete controller;
	controller = NULL;
	
}

void ExampleAIModule::onFrame()
{

	if (show_visibility_data)
		drawVisibilityData();

	if (show_bullets)
		drawBullets();

	drawStats();
	if(Broodwar->getFrameCount() == 1){
		//controller->initEnemy(Broodwar->enemy());
		controller->initUnits(Broodwar->self(), Broodwar->enemy());

		//// test// get the initial center distance between ally units and enemy units
		//double _ally_center_x = 0;
		//double _ally_center_y = 0;
		//double _enemy_center_x = 0;
		//double _enemy_center_y = 0;
		//
		//for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
		//		_ally_center_x = (*i)->getPosition().x();
		//	_ally_center_y = (*i)->getPosition().y();
		//	}

		//for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
		//		_enemy_center_x = (*i)->getPosition().x();
		//	_enemy_center_y = (*i)->getPosition().y();
		//	}
		///*double _init_central_dist = sqrt(pow((_ally_center_x / Broodwar->self()->getUnits().size() - _enemy_center_x / Broodwar->enemy()->getUnits().size()), 2.0) + 
		//	pow((_ally_center_y / Broodwar->self()->getUnits().size() - _enemy_center_y / Broodwar->enemy()->getUnits().size()), 2.0));*/
		//_ally_center_x -= _enemy_center_x;
		//_ally_center_y -= _enemy_center_y;
		//std::ofstream oFile("C:\\Program Files\\StarCraft\\bwapi-data\\AI\\initD.txt");
		//oFile << _ally_center_x << " " << _ally_center_y << std::endl;
		//oFile.close();


		////////
	}
	// give order to units every specified interval
	if(Broodwar->getFrameCount() % 10 == 0 && Broodwar->getFrameCount() > 0){
		
		// use controller to give order to units
		controller->takeAction();
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
	//if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
	//	//Broodwar->sendText("A %s [%x] has been spotted at (%d,%d)",unit->getType().getName().c_str(),unit,unit->getPosition().x(),unit->getPosition().y());
	//if (unit->getPlayer()->isEnemy(Broodwar->self())){
	//	BWAPI::Unit* nearestUnit;
	//	int nearestDistance = 10000;
	//	for(std::set<Unit*>::const_iterator i=Broodwar->enemy()->getUnits().begin();i!=Broodwar->enemy()->getUnits().end();i++){
	//		int avgDistance = 0;
	//		for(std::set<Unit*>::const_iterator j=Broodwar->self()->getUnits().begin();j!=Broodwar->self()->getUnits().end();j++){
	//			avgDistance += (*i)->getDistance((*j));
	//		}
	//		avgDistance /= Broodwar->enemy()->getUnits().size();
	//		if (avgDistance < nearestDistance){
	//			nearestDistance = avgDistance;
	//			nearestUnit = (*i);
	//		}
	//	}
	//	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++){
	//		(*i)->attack(nearestUnit);
	//	}
	//}
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
	/*if (!Broodwar->isReplay() && Broodwar->getFrameCount()>1)
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
	}*/
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
