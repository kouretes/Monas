#ifndef GATEWAY_H
#define GATEWAY_H

#include <vector>
#include <map>
#include <string>
#include <fstream>

#include "core/architecture/executable/provider/IProvider.hpp"
#include "core/architecture/messaging/Blackboard.hpp"
#include "core/include/Configurator.hpp"
#include "core/messages/Network.pb.h"
#include "core/messages/Architecture.pb.h"

#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupGameControlData.h"
#include "messages/Motion.pb.h"
#include "messages/ExternalCommunication.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/BehaviorMessages.pb.h"
#include "messages/Kimage.pb.h"

#include "hal/syscall.h"


/**
 * The gateway that takes messages from the outer world (GUI: KMonitor) and forwards them inside Monas.
 * The basic functionality is to take a message, do the proper actions and then send back to the sender an acknowledgment.
 * @author <a href="mailto:nikofinas@gmail.com">Nikos Kofinas</a> aka eldr4d, 2012 kouretes team
 * \file Gateway.h
*/

PROVIDER_START
class Gateway:   public IProvider {
public:
	int PROVIDER_VISIBLE IEX_DIRECTIVE_HOT Execute();

	Gateway (KSystem::ThreadConfig &c, Messaging::MessageHub &n) :
		EndPoint ("Gateway"),
		IProvider ("Gateway", c, n), _blk ("GatewayBlackboard") {
		_blk.attachTo (n );
		UserInit();
	};
	void PROVIDER_VISIBLE UserInit();
	const std::string PROVIDER_VISIBLE GetName() const {
		return "Gateway";
	}
	PROVIDER_VISIBLE ~Gateway() {
	}

	int GetCrc32 (const std::string &my_string);
private:
	Messaging::Blackboard _blk;

	boost::shared_ptr<const KnownHosts> h;
	boost::shared_ptr<const GameStateMessage>  gsm;

	uint32_t localHostId;
	uint32_t lockId;
	bool locked;
	std::map<uint32_t, std::string> ectimeouts, comtimeouts, ecameratimeouts;

	ResetMessage resetActMsg;
	GenericACK outmsg;
	GameStateMessage new_gsm;
	MotionActionMessage amot;
	LocalizationResetMessage lrm;
	MotionStiffnessMessage msm;
	HeadControlMessage hcm;
	CalibrateCamMessage ccm;
	/**
	 * Process all the configurations message's (reset, config update and file upload).
	 * This functions forwards to blk a reset or an configuration update message and write's to files if necessary
	 * @param incomingHostId: the id of the host that send us this message.
	 * */
	void processExternalConfig(uint32_t incomingHostId);

	/**
	 * Send a message to the image provider to re-calibrate the camera
	 * @param incomingHostId: the id of the host that send us this message.
	 * */
	void processCameraCalibration(uint32_t incomingHostId);

	/**
	 * Process all command message. This functions only calls the appropriate function to proccess every command separately.
	 * @param incomingHostId: the id of the host that send us this message.
	 * */
	void processExternalCommands(uint32_t incomingHostId);

	/**
	 * Update the current game control message with the external state and send it back to blackboard.
	 * @param commandID: the game command that has been send from the host
	 * */
	void processGameControllerCommand(int commandID);

	/**
	 * Execute some simple commands like stiffness off, sit or shutdown
	 * @param commandID: the corresponding simple action command id
	 * */
	void processSimpleActionsCommand(int commandID);

	/**
	 * Varius motion commands like stiffness change's or action's executions
	 * @param commandID: the corresponding simple action command id
	 * @param svalue: the string value from the message (action name or kinematic chain)
	 * @param value: stiffness value
	 * */
	void processMotionCommand(int commandID, std::string svalue, float value);

	/**
	 * Initialize localization particles to a given new point
	 * */
	void processLocalizationCommand(float x, float y, float phi);

	/**
	 * Send a head command to head controller like scan or localize
	 * @param commandID: the corresponding head command id
	 * @param value: head speed
	 * */
	void processHeadCommand(int commandID, float value);
};
PROVIDER_END
#endif //GATEWAY_H

