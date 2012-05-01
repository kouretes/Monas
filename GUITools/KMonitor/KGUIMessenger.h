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

class KGUIMessenger : public QObject
{
	Q_OBJECT

public:
	KGUIMessenger();
	~KGUIMessenger();

	MessageBuffer* makeWriteBuffer(std::string const& s);
	MessageBuffer* makeReadBuffer(std::string const& s);

public slots:
	void GWRHSubscriptionHandler(QString hostId);
	void GWRHUnsubscriptionHandler(QString hostId);

	void LWRHSubscriptionHandler(QString hostId);
	void LWRHUnsubscriptionHandler(QString hostId);

signals:
	void knownHostsUpdate(KnownHosts newHosts);
	void gameStateMessageUpdate(GameStateMessage gsm, QString hostId);
	void worldInfoUpdate(WorldInfo wim, QString hostId);
	void localizationDataUpdate(LocalizationDataForGUI, QString);

	void obsmsgUpdate(ObservationMessage obm, QString hostId);

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

	int vt;
};
#endif /* KGUIMESSENGER_H_ */
