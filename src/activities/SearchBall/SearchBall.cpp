
#include "SearchBall.h"

#include <iostream> //TODO

using namespace std;

namespace {
    ActivityRegistrar<SearchBall>::Type temp("SearchBall");
}

SearchBall::SearchBall()
: Publisher("Behavior"),
  yaw(0),pitch(0),
  dyaw(0.2),dpitch(0.25),
  yaw_direction(1),
  pitch_direction(1),
  yaw_just_changed(false),
  pitch_just_changed(false),
  left_bound(0.40),
  right_bound(-0.40),
  upper_bound(-0.20),
  lower_bound(0.40)
{
  ;
}


int SearchBall::Execute() {

  _blk->process_messages();

  HeadJointSensorsMessage headJointMsg = _blk->read<HeadJointSensorsMessage>("HeadJointSensorsMessage", "Sensors");

  yaw = headJointMsg.sensordata(0).sensorvalue();
  pitch = headJointMsg.sensordata(1).sensorvalue();
  
  new_yaw =0.0;
  new_pitch=0.0;


  if ( yaw_just_changed && abs(yaw) < 0.15 )
      yaw_just_changed = false;

  if ( pitch_just_changed && abs(pitch) < 0.15 )
      pitch_just_changed  = false;


  if ( ( pitch < upper_bound || pitch > lower_bound ) && !pitch_just_changed  ) {
    pitch_direction *= -1;
    pitch_just_changed = true;
  }


  if ( ( yaw < right_bound || yaw > left_bound ) && !yaw_just_changed ) {
    yaw_direction *= -1;
    yaw_just_changed = true;
    new_pitch = pitch_direction * dpitch;
  } 


  new_yaw = yaw_direction * dyaw;

  MotionHeadMessage msg;
  msg.set_command("changeHead");
  msg.add_parameter(new_yaw);
  msg.add_parameter(new_pitch);

  publish(&msg,"motion");

}

void SearchBall::UserInit () {
  _com->get_message_queue()->add_publisher(this);
  _com->get_message_queue()->add_subscriber(_blk);
  _com->get_message_queue()->subscribe("sensors", _blk, ON_TOPIC);
}

std::string SearchBall::GetName () {
	return "SearchBall";
}
