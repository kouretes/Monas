
#ifndef _TrackBall_h_
#define _TrackBall_h_ 1

#include "architecture/IActivity.h"
#include "messages/VisionObservations.pb.h"
#include "architecture/narukom/narukom_common.h"
#include "messages/motion.pb.h"

class TrackBall : public IActivity,  public Publisher {

  public:

    TrackBall();

    int Execute ();

    void UserInit ();

    std::string GetName ();

};

#endif // _TrackBall_h_
