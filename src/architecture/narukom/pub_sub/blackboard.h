/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef BLACKBOARD_H
#define BLACKBOARD_H
#include "msg.h"
#include "endpoint.h"
#include "stringRegistry.h"
#include "topicTree.h"

#include <string>
#include <map>
#include <set>

#include <string>




class Blackboard : virtual public EndPoint
{
public:


	explicit
	Blackboard(const std::string& );

	virtual ~Blackboard() {}
	void process_messages();
	void publish_all();
	enum  DataReq
	{
	    DATA_NEAREST, DATA_NEAREST_NOTOLDER, DATA_NEAREST_NOTNEWER
	} ;
	template<class Data>
	boost::shared_ptr<const Data> readData(const std::string& topic, const std::size_t  host = msgentry::HOST_ID_LOCAL_HOST, boost::posix_time::ptime* const tmp = 0 , boost::posix_time::ptime const* const time_req = 0, DataReq dtype = DATA_NEAREST);
	template<class Data>
	boost::shared_ptr<const Data> readSignal(const std::string& topic, const std::size_t  host = msgentry::HOST_ID_LOCAL_HOST, boost::posix_time::ptime* const tmp = 0 );
	template<class Data>
	boost::shared_ptr<const Data> readState(const std::string& topic, const std::size_t  host = msgentry::HOST_ID_LOCAL_HOST, boost::posix_time::ptime* const tmp = 0 );
	void publishData(const google::protobuf::Message & msg, std::string const& topic);
	void publishSignal(const google::protobuf::Message & msg, std::string const& topic);
	void publishState(const google::protobuf::Message &msg, std::string const& topic);
private:
	stringRegistry typeRegistry;

	//Each blackboard record is:
	typedef struct blackboard_record_s
	{
		boost::shared_ptr<const google::protobuf::Message > msg;
		boost::posix_time::ptime timestamp;
		//boost::posix_time::ptime timeoutstamp;
		bool operator== (const struct blackboard_record_s & b) const
		{
			return msg == b.msg;
		};
		bool operator<(const struct blackboard_record_s &b) const
		{
			return timestamp < b.timestamp;
		};
		bool operator<(const struct  boost::posix_time::ptime  &b) const
		{
			return timestamp < b;
		};
	} brecord;

	//=== Data struct: list of records, alongside a list of timeouts
	typedef std::set< brecord> recordlist;
	typedef std::map<std::size_t, boost::posix_time::time_duration> datatimeouts;
	typedef std::map<std::size_t, recordlist> datastruct;
	//=== State struct : single record per type
	typedef std::map<std::size_t, brecord> statestruct;
	//Signal struct: single record per type, with marking for clearing
	typedef struct signalentry_st
	{
		brecord d;
		bool cleared;
	} signalentry;
	typedef std::map<std::size_t, signalentry> signalstruct;

	//Registry of string types:
	typedef std::size_t type_t;
	typedef std::size_t topic_t;
	typedef std::size_t host_t;
	typedef struct region_index_s
	{
		topic_t tid;
		host_t hid;

		bool operator== (const struct region_index_s & b) const
		{
			return (tid == 0 || b.tid == 0 || tid == b.tid) &&
			       (hid == msgentry::HOST_ID_ANY_HOST || b.hid == msgentry::HOST_ID_ANY_HOST || hid == b.hid);
		};
		bool operator<(const struct region_index_s &b) const
		{
			return hid == b.hid ? tid < b.tid : hid < b.hid;
		};
	} region_index;
	//Distjoint region: one per topic and per host
	typedef struct
	{
		datatimeouts blkdatatimeouts;
		datastruct blkdata;
		signalstruct blksignal;
		statestruct blkstate;
	} disjoint_region;
	typedef std::map<region_index, disjoint_region> regions;
	regions allrecords;

	//boost::posix_time::ptime cur_tmsp;
	//To be published
	std::vector<msgentry> topublish;
	int cleanup();
	static const float TIMEOUTSCALE = 1.2;
};




template<class Data>
boost::shared_ptr<const Data> Blackboard::readData(const std::string& topic, const std::size_t  host,
        boost::posix_time::ptime* const tmp ,
        boost::posix_time::ptime const * const time_req,
        DataReq dtype)
{
	const type_t atypeid = typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;
	i.tid = Topics::Instance().getId(topic);
	i.hid = host;

	if(topic == "") i.tid = 0; //Wildcard

	regions::iterator rit = allrecords.begin();

	for(; rit != allrecords.end(); ++rit)
	{
		if((*rit).first == i && (*rit).second.blkdata.find(atypeid) != (*rit).second.blkdata.end())
			break;
	}

	if(rit == allrecords.end())
		return boost::shared_ptr<const Data>();

	const datastruct &d = (*rit).second.blkdata;
	datastruct::const_iterator dit = d.find(atypeid);

	if(dit == d.end())
		return boost::shared_ptr<const Data>();

	const recordlist &q = (*dit).second;

	//Empty set, avoid iterators beiing beign()==end()
	if(q.size() == 0)
		return boost::shared_ptr<const Data>();

	if(time_req == NULL)
	{
		recordlist::const_reverse_iterator it = q.rbegin();

		if(tmp != NULL)
			*tmp = (*it).timestamp;

		return  boost::static_pointer_cast<const Data>( (*it).msg);
	}

	//Update timeout
	boost::posix_time::time_duration nt = boost::posix_time::microsec_clock::universal_time() - *time_req;
	nt *= TIMEOUTSCALE;

	if((*rit).second.blkdatatimeouts.find(atypeid) == (*rit).second.blkdatatimeouts.end())
		(*rit).second.blkdatatimeouts[atypeid] = nt;
	else
		(*rit).second.blkdatatimeouts[atypeid] = (*rit).second.blkdatatimeouts[atypeid] < nt ? nt : (*rit).second.blkdatatimeouts[atypeid];

	brecord s;
	s.timestamp = *time_req;
	recordlist::const_iterator lit = q.lower_bound(s);

	if(dtype == DATA_NEAREST_NOTOLDER)
	{
		if(lit == q.end())
		{
			return boost::shared_ptr<const Data>();
		}

		if(tmp != NULL)
			*tmp = (*lit).timestamp;

		return  boost::static_pointer_cast<const Data>( (*lit).msg);
	}
	else if(dtype == DATA_NEAREST_NOTNEWER)
	{
		if(lit == q.end())
			lit--;
		else if(s < *lit)
		{
			if(lit == q.begin())
			{
				return boost::shared_ptr<const Data>();
			}

			--lit;//At least one element, and not at begining, see check q.size()==0 above
		}

		if(tmp != NULL)
			*tmp = (*lit).timestamp;

		return  boost::static_pointer_cast<const Data>( (*lit).msg);
	}

	if(lit == q.end()) //No newer message that lit, this is the nearest, and is OLDER than requested
	{
		lit--;

		if(tmp != NULL)
			*tmp = (*lit).timestamp;

		return  boost::static_pointer_cast<const Data>( (*lit).msg);// Nearest is last, meaning request wast too recent
	}

	//Here lit!=end()
	if(lit == q.begin()) //No older message that lit, this is the nearest
	{
		if(tmp != NULL)
			*tmp = (*lit).timestamp;

		return  boost::static_pointer_cast<const Data>( (*lit).msg);// Nearest is the lower_bound==begin(), meaning request was too old
	}

	recordlist::const_iterator uit = lit;
	uit--;

	if(s.timestamp - (*uit).timestamp < (*lit).timestamp - s.timestamp) //Upper_bound is closer to search
		lit = uit;

	if(tmp != NULL)
		*tmp = (*lit).timestamp;

	return  boost::static_pointer_cast<const Data>( (*lit).msg);
}

template<class Data>
boost::shared_ptr<const Data> Blackboard::readSignal(const std::string& topic, const std::size_t  host  , boost::posix_time::ptime* tmp )
{
	const type_t atypeid = typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;
	i.tid = Topics::Instance().getId(topic);
	i.hid = host;

	if(topic == "") i.tid = 0; //Wildcard

	regions::iterator rit = allrecords.begin();

	for(; rit != allrecords.end(); ++rit)
	{
		if((*rit).first == i && (*rit).second.blksignal.find(atypeid) != (*rit).second.blksignal.end())
			break;
	}

	if(rit == allrecords.end())
		return boost::shared_ptr<const Data>();

	signalstruct &s = (*rit).second.blksignal;
	signalstruct::iterator it = s.find(atypeid);

	if(it == s.end())
		return boost::shared_ptr<const Data>();

	if(tmp != NULL)
		*tmp = (*it).second.d.timestamp;

	(*it).second.cleared = true;
	return  boost::static_pointer_cast<const Data>((*it).second.d.msg);
}

template<class Data>
boost::shared_ptr<const Data> Blackboard::readState(const std::string& topic, const std::size_t  host  , boost::posix_time::ptime* tmp )
{
	const type_t atypeid = typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;
	i.tid = Topics::Instance().getId(topic);
	i.hid = host;

	if(topic == "") i.tid = 0; //Wildcard

	regions::const_iterator rit = allrecords.begin();

	for(; rit != allrecords.end(); ++rit)
	{
		if((*rit).first == i && (*rit).second.blkstate.find(atypeid) != (*rit).second.blkstate.end())
			break;
	}

	if(rit == allrecords.end())
		return boost::shared_ptr<const Data>();

	const statestruct &s = (*rit).second.blkstate;
	statestruct::const_iterator it = s.find(atypeid);

	if(it == s.end())
		return boost::shared_ptr<const Data>();

	if(tmp != NULL) //Return timestamp :P
		*tmp = (*it).second.timestamp;

	return  boost::static_pointer_cast<const Data>((*it).second.msg);
}

#endif // BLACKBOARD_H
