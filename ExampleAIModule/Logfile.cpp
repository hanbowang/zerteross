#include "Logfile.h"
#include <fstream>

namespace BWAPI{
	Logfile::Logfile(std::string fileName){
		filename = fileName;
		std::ifstream iFile(fileName);
		if(!(iFile >> curgen >> gens >> curorg
             >> orgs >> curround >> rounds
             >> curfitness >> allyMaxHP)){
			valid = false;
		} else {
			valid = true;
		}
		iFile.close();
	};
    
	Logfile::Logfile(int incurgen, int ingens, int incurorg, int inorgs,
                     int incurround, int inrounds, double incurfitness,
                     std::string infilename){
		curgen = incurgen;
        gens = ingens;
		curorg = incurorg;
		orgs = inorgs;
		curround = incurround;
		rounds = inrounds;
		curfitness = incurfitness;
		filename = infilename;
		valid = true;
	};
    
    void Logfile::update(){
		std::ofstream oFile(this->filename);
		oFile << this->curgen << " " << this->gens << " " << this->curorg << " "
        << this->orgs << " " << this->curround << " " << this->rounds << " "
		<< this->curfitness << " " << this->allyMaxHP << std::endl;
		oFile.close();
	};
    
}