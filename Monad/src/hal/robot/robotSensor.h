#ifndef _robot_sensor_h_
#define _robot_sensor_h_ 1

#include <string>

class RobotSensor {

    public:

        RobotSensor(std::string &name) 
            name(name)
        {
            ;
        }

        virtual ~RobotSensor() { ; }

    private:

        std::string name;

};

#endif // _robot_sensor_h_
