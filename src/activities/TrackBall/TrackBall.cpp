
#include "TrackBall.h"

namespace {
    ActivityRegistrar<TrackBall>::Type temp("TrackBall");
}

TrackBall::TrackBall() : Publisher("TrackBall") {
  ;
}


int TrackBall::Execute() {

  BallTrackMessage bmsg = _blk->read<BallTrackMessage>("BallTrackMessage", "Vision");

  float overshootfix = 1- fabs(bmsg.referencepitch())/(3.14/2);

  float cx = bmsg.cx();
  float cy = bmsg.cy();

  int balllastseendirection = bmsg.referenceyaw()-0.9f*cx;

  if (fabs(cx) > 0.015 || fabs(cy) > 0.015) {

    MotionHeadMessage msg;
    msg.set_command("setHead");
    msg.add_parameter(bmsg.referenceyaw()-overshootfix*cx);
    msg.add_parameter(bmsg.referencepitch()-overshootfix*cy);

    publish(&msg,"motion");
  }
  return 0;
}

void TrackBall::UserInit () {
  _com->get_message_queue()->add_publisher(this);
  _com->get_message_queue()->add_subscriber(_blk);
  _com->get_message_queue()->subscribe("vision", _blk, ON_TOPIC);
}

std::string TrackBall::GetName () {
	return "TrackBall";
}
