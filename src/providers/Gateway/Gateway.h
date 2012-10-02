#ifndef GATEWAY_H
#define GATEWAY_H

#include <vector>
#include <map>
#include <string>

#include "architecture/executables/IProvider.h"
#include "architecture/narukom/pub_sub/blackboard.h"
#include "architecture/XmlManager/XmlConfigurator.h"

#include "messages/Network.pb.h"
#include "messages/GUICommunication.pb.h"



PROVIDER_START
class Gateway:   public IProvider
{
public:
	int PROVIDER_VISIBLE IEX_DIRECTIVE_HOT Execute();

	Gateway(KSystem::ThreadConfig &c, Narukom&n):
		EndPoint("Gateway"),
		IProvider("Gateway", c, n), _blk("GatewayBlackboard")
	{
		_blk.attachTo(*n.get_message_queue());
		UserInit();
	};
	void PROVIDER_VISIBLE UserInit();
	const std::string PROVIDER_VISIBLE GetName() const
	{
		return "Gateway";
	}
	PROVIDER_VISIBLE ~Gateway()
	{
	}
	
	int GetCrc32(const std::string& my_string);
private:
	Blackboard _blk;
	XmlNode _xml;
	GenericACK outmsg;
	boost::shared_ptr<const KnownHosts> h;
	uint32_t localHostId;
	uint32_t lockId;
	bool locked;
	std::map<uint32_t,std::string> timeouts;
	
	ResetMessage resetActMsg;
	UpdateMessage updateXMLMsg;

};
PROVIDER_END
#endif //GATEWAY_H

