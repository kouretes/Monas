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

#include <string>
#include <map>
#include <set>

#include <string>




class Blackboard : public EndPoint
{
public:


    explicit
    Blackboard(const std::string& );

    virtual ~Blackboard() {}
    void process_messages();
    void publish_all();
		template<class Data>
		boost::shared_ptr<const Data> readData(const std::string& topic,const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 , boost::posix_time::ptime const* const time_req =0);
		template<class Data>
        boost::shared_ptr<const Data> readSignal(const std::string& topic, const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 );
        template<class Data>
        boost::shared_ptr<const Data> readState(const std::string& topic, const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 );
        void publishData(const google::protobuf::Message & msg,std::string const& topic);
        void publishSignal(const google::protobuf::Message & msg,std::string const& topic);
        void publishState(const google::protobuf::Message &msg, std::string const& topic);
private:
	stringRegistry typeRegistry,topicRegistry,hostRegistry;

	//Each blackboard record is:
	typedef struct blackboard_record_s{
        boost::shared_ptr<const google::protobuf::Message > msg;
        boost::posix_time::ptime timestamp;
        //boost::posix_time::ptime timeoutstamp;
        bool operator== (const struct blackboard_record_s & b) const { return msg==b.msg;};
		bool operator<(const struct blackboard_record_s &b) const {return timestamp < b.timestamp;};
    } brecord;

	//=== Data struct: list of records, alongside a list of timeouts
	typedef std::set< brecord> recordlist;
	typedef std::map<std::size_t,boost::posix_time::time_duration> datatimeouts;
	typedef std::map<std::size_t,recordlist> datastruct;
	//=== State struct : single record per type
    typedef std::map<std::size_t,brecord> statestruct;
    //Signal struct: single record per type, with marking for clearing
    typedef struct signalentry_st{
        brecord d;
        bool cleared;
    } signalentry;
    typedef std::map<std::size_t,signalentry> signalstruct;

    //Registry of string types:
    typedef std::size_t type_t;
    typedef std::size_t topic_t;
    typedef std::size_t host_t;
	typedef struct region_index_s {
		topic_t tid;
		host_t hid;

		bool operator== (const struct region_index_s & b) const {
			return (tid==0||b.tid==0||tid==b.tid)&&
					(hid==0||b.hid==0||hid==b.hid);};
		bool operator<(const struct region_index_s &b) const {return hid==b.hid?tid<b.tid:hid<b.hid;};
	} region_index;
	//Distjoint region: one per topic and per host
	typedef struct {
	datatimeouts blkdatatimeouts;
    datastruct blkdata;
    signalstruct blksignal;
    statestruct blkstate;
	} disjoint_region;
	typedef std::map<region_index,disjoint_region> regions;
	regions allrecords;

    //boost::posix_time::ptime cur_tmsp;
    //To be published
    std::vector<msgentry> topublish;
    int cleanup();
	static const float TIMEOUTSCALE=1.2;
};




template<class Data>
boost::shared_ptr<const Data> Blackboard::readData(const std::string& topic, const std::string&  host,boost::posix_time::ptime* const tmp  ,boost::posix_time::ptime const * const time_req )
{
	const type_t atypeid=typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;

	i.tid=topicRegistry.getId(topic);
	i.hid=hostRegistry.getId(host);
	if(host=="") i.hid=0;//Wildcard *
	if(topic=="") i.tid=0;//Wildcard
	regions::iterator rit=allrecords.find(i);
	if(rit==allrecords.end())
		return boost::shared_ptr<const Data>();
	const datastruct &d=(*rit).second.blkdata;
	datastruct::const_iterator dit=d.find(atypeid);
    if(dit==d.end())
        return boost::shared_ptr<const Data>();
    const recordlist &q=(*dit).second;
    recordlist::const_reverse_iterator it=q.rbegin();
    if(it==q.rend())
        return boost::shared_ptr<const Data>();
    if(time_req==NULL)
    {
		if(tmp!=NULL)
			*tmp=(*it).timestamp;
		return  boost::static_pointer_cast<const Data>( (*it).msg);
    }
    boost::posix_time::time_duration nt=boost::posix_time::microsec_clock::universal_time()-*time_req;
	nt*=TIMEOUTSCALE;
    if((*rit).second.blkdatatimeouts.find(atypeid)==(*rit).second.blkdatatimeouts.end())
		(*rit).second.blkdatatimeouts[atypeid]=nt;
	else
		(*rit).second.blkdatatimeouts[atypeid]= (*rit).second.blkdatatimeouts[atypeid]<nt?nt:(*rit).second.blkdatatimeouts[atypeid];

    recordlist::const_iterator fit=q.begin();
    boost::posix_time::time_duration dist,t;
    t=*time_req-(*fit).timestamp;
    dist=t;
    while(fit!=q.end())
    {
        //Skip
       // if( !(process==""||(*it).publisher==process) ||

		t=*time_req-(*fit).timestamp;
		//std::cout<<(*fit).timestamp<<" "<<t<<std::endl;
		if(t. is_negative())
			t=t.invert_sign();
        if(t<=dist)
			dist=t;
        else
            break;
		++fit; //advance for the next check
    }
    if(fit!=q.begin()) --fit;//Go back one, t

    if(fit==q.end())
        --fit;
    if(fit==q.end())
    {
        return boost::shared_ptr<const Data>();
    }

    if(tmp!=NULL)
        *tmp=(*fit).timestamp;
    return  boost::static_pointer_cast<const Data>( (*fit).msg);
}
template<class Data>
boost::shared_ptr<const Data> Blackboard::readSignal(const std::string& topic, const std::string&  host ,boost::posix_time::ptime* tmp )
{
	const type_t atypeid=typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;

	i.tid=topicRegistry.getId(topic);
	i.hid=hostRegistry.getId(host);
	if(host=="") i.hid=0;//Wildcard *
	if(topic=="") i.tid=0;//Wildcard
	regions::iterator rit=allrecords.find(i);

	if(rit==allrecords.end())
		return boost::shared_ptr<const Data>();


	signalstruct &s=(*rit).second.blksignal;
	signalstruct::iterator it=s.find(atypeid);
    if(it==s.end())
        return boost::shared_ptr<const Data>();

    if(tmp!=NULL)
        *tmp=(*it).second.d.timestamp;
   (*it).second.cleared=true;
    return  boost::static_pointer_cast<const Data>((*it).second.d.msg);
}

template<class Data>
boost::shared_ptr<const Data> Blackboard::readState(const std::string& topic, const std::string&  host ,boost::posix_time::ptime* tmp )
{
	const type_t atypeid=typeRegistry.getId(Data::default_instance().GetTypeName());
	region_index i;

	i.tid=topicRegistry.getId(topic);
	i.hid=hostRegistry.getId(host);
	if(host=="") i.hid=0;//Wildcard *
	if(topic=="") i.tid=0;//Wildcard
	regions::const_iterator rit=allrecords.find(i);
	if(rit==allrecords.end())
		return boost::shared_ptr<const Data>();

	const statestruct &s=(*rit).second.blkstate;
	statestruct::const_iterator it=s.find(atypeid);
    if(it==s.end())
        return boost::shared_ptr<const Data>();
    if(tmp!=NULL)//Return timestamp :P
        *tmp=(*it).second.timestamp;

    return  boost::static_pointer_cast<const Data>((*it).second.msg);

}

#endif // BLACKBOARD_H
