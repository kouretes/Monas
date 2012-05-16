#ifndef KGUIMESSENGER_H_
#define KGUIMESSENGER_H_

#include <QStringList>
#include <QObject>
#include <QTimer>

#include "architecture/narukom/pub_sub/endpoint.h"
#include "architecture/narukom/pub_sub/stringRegistry.h"

#include "messages/Network.pb.h"
#include "messages/WorldInfo.pb.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/VisionObservations.pb.h"
#include "messages/SensorsMessage.pb.h"
#include "messages/motion.pb.h"

class KGUIMessenger : public QObject
{
	Q_OBJECT

public:
	KGUIMessenger();
	~KGUIMessenger();

	MessageBuffer* makeWriteBuffer(std::string const& s);
	MessageBuffer* makeReadBuffer(std::string const& s);

public slots:
	void GWRHSubscriptionHandler(QString);
	void GWRHUnsubscriptionHandler(QString);

	void LWRHSubscriptionHandler(QString);
	void LWRHUnsubscriptionHandler(QString);

signals:
	void knownHostsUpdate(KnownHosts);
	void gameStateMessageUpdate(GameStateMessage, QString);
	void worldInfoUpdate(WorldInfo, QString);
	void localizationDataUpdate(LocalizationDataForGUI, QString);
	void obsmsgUpdate(ObservationMessage, QString);
	void headYawJointUpdate(float, QString);
	void motionCommandUpdate(MotionWalkMessage, QString);

private slots:
	void allocateReceivedMessages();

private:
	void updateSubscription(std::string const& topic , msgentry::msgclass_t where, std::size_t host);

	void printMyGWRequestedHosts();
	void printKnownHosts(KnownHosts hosts);

	EndPoint *multicast;
	stringRegistry pubsubRegistry;

	QTimer *timer;

	QStringList myGWRequestedHosts;
	QString myLWRequestedHost;
};
#endif /* KGUIMESSENGER_H_ */
