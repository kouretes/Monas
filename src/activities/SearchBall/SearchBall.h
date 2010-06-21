
#ifndef _SearchBall_h_
#define _SearchBall_h_ 1

#include "architecture/IActivity.h"
#include "messages/motion.pb.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/SensorsMessage.pb.h"

class SearchBall : public IActivity, public Publisher {

  public:

    SearchBall();

    int Execute ();

    void UserInit ();

    std::string GetName ();

  private:

    float yaw,pitch;
    float new_yaw,new_pitch;
    float dyaw,dpitch;
    int yaw_direction;
    int pitch_direction;
    bool yaw_just_changed;
    bool pitch_just_changed;
    const float left_bound;
    const float right_bound;
    const float upper_bound;
    const float lower_bound;

};

#endif // _SearchBall_h_
