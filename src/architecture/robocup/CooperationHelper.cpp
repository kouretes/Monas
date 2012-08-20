#include "CooperationHelper.h"
#include "messages/Gamecontroller.pb.h"
#include "messages/Network.pb.h"

std::size_t CooperationHelper::getHostByPlayerNum(std::size_t p)
{
	boost::shared_ptr<const KnownHosts>  h = _blk.readState<KnownHosts>("communication");

	if(!h.get() || (h && h->entrylist_size() == 0))
		return msgentry::HOST_ID_ANY_HOST;

	const ::google::protobuf::RepeatedPtrField< HostEntry >& rf = h->entrylist();
	::google::protobuf::RepeatedPtrField< HostEntry >::const_iterator fit;
	boost::shared_ptr<const GameStateMessage> gs;
	//_blk->readState<GameStateMessage> ("behavior");

	for(fit = rf.begin(); fit != rf.end(); ++fit)
	{
		gs = _blk.readState<GameStateMessage> ("worldstate", (*fit).hostid());

		if(gs->player_number() == p && gs->team_number() == team)
			return (*fit).hostid();
	}

	return msgentry::HOST_ID_ANY_HOST;
};
