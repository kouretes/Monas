#ifndef _robot_camera_h_
#define _robot_camera_h_ 1

#include"robot_sensor.h"

#include<string>

template<class ImageDataType>
class RobotCamera : public RobotSensor {

    public:

        RobotCamera(std::string name, int px, int py, int fps):
            RobotSensor(name),
            px(px),py(py),fps(fps) {
                ;
        }

        ImageDataType GetImage()=0;

        std::pair<int,int> GetImageDimensions() const {
            return std::pair<int,int> (px,py);
        }

        int GetCameraFPS() const {
            return fps;
        }

    private:

        const int px,py,fps;

};

#endif // _robot_camera_h_
