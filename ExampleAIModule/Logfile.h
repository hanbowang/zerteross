#include <iostream>

namespace BWAPI {
    
    class Logfile
    {
    public:
        Logfile(std::string fileName);
        Logfile(int incurgen, int ingens, int incurorg, int inorgs,
                int incurround, int inrounds, double incurfitness,
                std::string infilename);
        void update();
        
        int curgen;
        int gens;
        int curorg;
        int orgs;
        int curround;
        int rounds;
        double curfitness;
        bool valid;
        std::string filename;
		int allyMaxHP;
        
    };
}