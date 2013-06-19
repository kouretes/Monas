#ifndef KGUIMESSENGER_H_
#define KGUIMESSENGER_H_

#include <QStringList>
#include <QObject>
#include <QTimer>

#include "core/architecture/messaging/EndPoint.hpp"
#include "core/elements/StringRegistry.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include "core/messages/Network.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/RoboCupPlayerData.h"
#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"

#include "messages/Motion.pb.h"
#include "messages/ExternalCommunication.pb.h"
#include "messages/PathPlanningMessages.pb.h"

#include "messages/Kimage.pb.h"
#include "messages/Debug.pb.h"

#include <map>

class KGUIMessenger : public QObject {
	Q_OBJECT

public:
	KGUIMessenger();
	~KGUIMessenger();

	MessageBuffer *makeWriteBuffer(std::string const &s);
	MessageBuffer *makeReadBuffer(std::string const &s);

public slots:
	void GWSubscriptionHandler(QString);
	void GWUnsubscriptionHandler(QString);

	void SubscriptionHandler(QString);

	void XMLPublishMessage(ExternalConfig);
	void CommandPublishMessage(ExternalCommand message);
	void KccPublishMessage(CameraCalibration message);
	
	void tabChangeHandler(int);

signals:
	void knownHostsUpdate(KnownHosts);
	void gameStateMessageUpdate(GameStateMessage, QString);
	void worldInfoUpdate(WorldInfo, QString);
	void sharedWorldInfoUpdate(SharedWorldInfo, QString);
	void localizationDataUpdate(LocalizationDataForGUI, QString);

	void EKFMHypothesisUpdate(EKFMHypothesis, QString);
	void OdometryUpdate(OdometryInfoMessage, QString);

	void obsmsgUpdate(ObservationMessage, QString);
	void motionCommandUpdate(MotionWalkMessage, QString);
	void formationDataUpdate(FormationDataForGUI, QString);
	void visionDebugData(VisionDebugMessage, QString);
	void gridInfoUpdate(GridInfo, QString);
	void rawImage(KRawImage, QString, boost::posix_time::ptime);
	void sensorsDataUpdate(AllSensorValuesMessage, QString);
	void GenericAckReceived(GenericACK, QString);
   
	void addHost(QString hostId, QString hostName);
	void removeHost(QString hostId);
	void updateGameState(QString iconPath, QString state, QString hostId);

	void changeAllTabsToHost(QString hostId);

private slots:
	void allocateReceivedMessages();

private:
	void updateSubscription(std::string const &topic , msgentry::msgclass_t where, std::size_t host);
	void updateKnownHosts(KnownHosts myRemoteHosts);
	void updateGameState(GameStateMessage gsm, QString hostId);

	void printKnownHosts(KnownHosts hosts);

	EndPoint *multicast;
	StringRegistry pubsubRegistry;

	QTimer *timer;

	typedef std::vector<uint32_t> vec;
	vec hostIds;

	int currentKMonitorTab;

	QStringList myGWRequestedHosts;
	QString myKMonitorRequestedHost;
};

#endif // KGUIMESSENGER_H_
