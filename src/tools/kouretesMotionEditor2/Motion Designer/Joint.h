#ifndef JOINT_H
#define JOINT_H

#include<string>
#include<vector>

class Joint{

	public:

                std::string jointName;
                std::vector<float> bounds;
		float step;
                std::string jointColor;
                std::string coupledWith;
                std::string couplingType;

};

#endif // JOINT_H

