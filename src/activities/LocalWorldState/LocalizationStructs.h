#ifndef LOCALIZATIONSTRUCTS_H_
#define LOCALIZATIONSTRUCTS_H_

#include <string>
#include <map>
using namespace std;

namespace Localization{
    /**
     * @struct ftr
     * @brief A structure used to store statistical data about a recognized feature
     */
    typedef struct ftr
    {
        float x;
        float y;
        string id;
        float weight;
        void set(float x_, float y_, string id_, float weight_)
        {
	        id = id_;
	        x = x_;
	        y = y_;
	        weight = weight_;
        }
    }feature;

    /**
     * @struct pvar
     * @brief Special structure to keep the data of a particle
     */
    typedef struct pvar
    {
        float x;
        float y;
        float phi;
        float Weight;
        bool valid;

        pvar()
        {
	        x = 0;
	        y = 0;
	        phi = 0;
	        Weight = 0;
            valid=false;
        }
        bool operator<(const struct pvar &other) const
        {
	        return Weight < other.Weight;
        }

    } partcl;

     /**
     * @struct var
     * @brief Usualy particles are processed in arrays.So in this structure a number of arrays store individually a property
     */
    typedef struct var
    {
        float *x;
        float *y;
        float *phi;
        float *Weight;
        float WeightSum;
        int size;
        ~var()
        {
	        if (x != NULL)
		        delete[] x;

	        if (y != NULL)
		        delete[] y;

	        if (phi != NULL)
		        delete[] phi;

	        if (Weight != NULL)
		        delete[] Weight;
        }
    } parts;

    /**
     * @struct rvar
     * @brief Random variable structure
     */
    typedef struct rvar
    {
        float val;
        float Emean;
        float Edev;
        float ratiomean;
        float ratiodev;
    } randvar;

    /**
     * @struct MM
     * @brief Structure to keep motion model info
     */
    typedef struct MM
    {
        bool freshData;
        string type;
        randvar Distance;
        randvar Direction;
        randvar Rotation;
    } KMotionModel;


    /**
     * @struct OM
     * @brief Structure to keep observation model info
     */
    typedef struct OM
    {
        feature Feature;
        randvar Distance;
        randvar Bearing;
        boost::posix_time::ptime observationTime;
    } KObservationModel;

    

    /**
     * @struct blf
     * @brief  Structure to store the belief of the robot about its position
     */
    typedef struct blf
    {
        float x;
        float y;
        float phi;
        float confidence;
        float weightconfidence;
    } belief;

    typedef struct Config{

        int ekfEnable;

        float partclsNum;
        float spreadParticlesDeviation;
        float rotationDeviation;
        float percentParticlesSpread;
        float spreadParticlesDeviationAfterFall;
        float rotationDeviationAfterFallInDeg;
        float numberOfParticlesSpreadAfterFall;
        KMotionModel robotmovement;
  
        // Field parameters
        float carpetMaxX;
	    float carpetMinX;
	    float carpetMaxY;
	    float carpetMinY;
	    float fieldMaxX;
	    float fieldMinX;
	    float fieldMaxY;
	    float fieldMinY;

        // Player positions
	    float initX[2], initY[2], initPhi[2];
        float readyX,readyY,readyPhi;
	    int playerNumber;

	    //map with all the features we read from an xml
	    map<string, Localization::feature> KFeaturesmap;

    }LocConfig;
}

#endif
