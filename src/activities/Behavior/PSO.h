#ifndef PSO_H
#define PSO_H

using namespace std;
using namespace FormationParameters;
using namespace Utility;

namespace PSO {
	
	typedef vector<float> particle;	// particle type
	
	struct PSOData{
		particle gBest;
		float value;
		unsigned int cycle;
	};
	
	inline bool comparePSOData(const PSOData &data1, const PSOData &data2) {
    	return (data1.value > data2.value);
	}
	
	inline void sortPSOData(vector<PSOData> &mappings) {
		sort(mappings.begin(), mappings.end(), comparePSOData);
	}
	
	const unsigned int DIM = 2; // dimensions of each robot (x, y)
	const float omega = 0.5f;
	
	unsigned int unfIndex;
  	float gBestCost;
  	
  	/**
  	 * @fn vector<float> uniformVector(unsigned int size)
  	 * @brief Uniform distribution function on [0,1) that returns a vector of random
  	 * number defined by the size value.
  	 */
  	vector<float> uniformVector(unsigned int size) {
  		
  		vector<float> result(size);
  		  		
  		for(unsigned int i = 0 ; i < size ; i++)
  			result[i] = (rand() % 100) * 0.01;
  			
  		return result;
  	}
  	
  	/**
  	 * @fn particle runPSO(unsigned int iterations, unsigned int swarmSize, FormationGenerator &fGen, vector<Robot> &robots, float ballY)
  	 * @brief Use to run the PSO algorithm for a given number of particles and iterations.
  	 * @return the global best particle found on the local search.
  	 */
	particle runPSO(unsigned int iterations, unsigned int swarmSize, FormationGenerator &fGen, vector<Robot> &robots, float ballY) {
		
		float Ux = 0.0f, Uxbest = 0.0f, UgStar = 0.0f;
		
		gBestCost = 0.0f;
		
		vector<particle> swarmX(swarmSize);
		vector<particle> swarmV(swarmSize);
		vector<particle> swarmXbest(swarmSize);
		particle gStar(DIM*robots.size());

		vector<posInfo> temp;
		
		for(unsigned int p = 0 ; p < swarmSize ; p++) {
			
			temp.resize(fGen.getFormation()->size() - 1);
			
			for(unsigned int i = 1 ; i < fGen.getFormation()->size() ; i++) {
					temp.at(i-1).X = fGen.getFormation()->at(i).X;
					temp.at(i-1).Y = fGen.getFormation()->at(i).Y;
			}
			
			swarmX[p].resize(DIM*robots.size());
			swarmV[p].resize(DIM*robots.size());
			
			for(unsigned int d = 0 ; d < DIM*robots.size() ; d= d+2) {
				unfIndex = rand() % temp.size();
				swarmX[p].at(d) = temp.at(unfIndex).X;
				swarmX[p].at(d + 1) = temp.at(unfIndex).Y;
				swarmV[p].at(d) = 0.0f;
				swarmV[p].at(d + 1) = 0.0f;
				temp.erase(temp.begin() + unfIndex);
			}
			
			swarmXbest[p] = swarmX[p];
		
			Uxbest = U(swarmXbest[p], ballY, fGen, robots);
			UgStar = U(gStar, ballY, fGen, robots);

            if( Uxbest > UgStar ) {
            	gStar = swarmXbest[p];
				gBestCost = Uxbest;
			}
            	
            temp.clear();
		}
		
		vector<float> e1(DIM*robots.size());
		vector<float> e2(DIM*robots.size());
		
		for(unsigned int it = 0 ; it < iterations ; it++) {
			for(unsigned int p = 0 ; p < swarmSize ; p++) {
				
				e1 = uniformVector(DIM*robots.size());
                e2 = uniformVector(DIM*robots.size());
                
                for(unsigned int d = 0 ; d < DIM*robots.size() ; d++) {     	
                	swarmV[p].at(d) = omega*swarmV[p].at(d) + 2*e1[d]*( gStar[d] - swarmX[p].at(d) ) + 2*e2[d]*( swarmXbest[p].at(d) - swarmX[p].at(d) );
                	swarmX[p].at(d) = swarmX[p].at(d) + swarmV[p].at(d);
                }
                
                Ux = U(swarmX[p], ballY, fGen, robots);
                Uxbest = U(swarmXbest[p], ballY, fGen, robots);
                UgStar = U(gStar, ballY, fGen, robots);
                 
                if(Ux > Uxbest) {
                    swarmXbest[p] = swarmX[p];
                    if(Uxbest > UgStar) {
                        gStar = swarmXbest[p];
    					gBestCost = Uxbest;
                    }
             	}
			}
		}
		return gStar;
	}

};

#endif
