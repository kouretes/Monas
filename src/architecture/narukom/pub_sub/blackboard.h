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
#include "publisher.h"
#include "subscriber.h"

#include <string>
#include <map>
#include <set>

#include <string>




class Blackboard : public Subscriber, public Publisher
{
public:
    typedef struct signalentry_st{
        msgentry d;
        bool cleared;
    } signalentry;


    typedef std::set< msgentry,msgentrytimestampComparator > historyqueue;
    typedef std::map<std::string,boost::posix_time::time_duration> datatimeouts;
    typedef std::map<std::string,historyqueue> datastruct;
    //typedef std::map<std::string,msgentry> simplestruct;
    typedef std::map<std::string,signalentry> signalstruct;

    explicit
    Blackboard(const std::string& );

    virtual ~Blackboard() {}
    void process_messages();
    void publish_all();
		template<class Data>
		boost::shared_ptr<const Data> read_data(const std::string& type,const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 , boost::posix_time::ptime const* const time_req =0);
		template<class Data>
        boost::shared_ptr<const Data> read_signal(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 );
        template<class Data>
        boost::shared_ptr<const Data> read_state(const std::string& type, const std::string&  host = "localhost",boost::posix_time::ptime* const tmp =0 );
        void publish_data(const google::protobuf::Message & msg,std::string const& topic);
        void publish_signal(const google::protobuf::Message & msg,std::string const& topic);
        void publish_state(const google::protobuf::Message &msg, std::string const& topic);
private:
	datatimeouts blkdatatimeouts;
    datastruct blkdata;
    signalstruct sigdata;
    //simplestruct statedata;
    //boost::posix_time::ptime cur_tmsp;
    std::vector<msgentry> topublish;
    int cleanup();
    static bool timestampComparatorFunc( const  boost::posix_time::ptime &a, const  boost::posix_time::ptime& b) { return a< b  ;};
    static bool timestampComparatorFunc( const msgentry &a, const  boost::posix_time::ptime & b) { return a.timestamp< b  ;};
    static bool timestampComparatorFunc(  const  boost::posix_time::ptime &a,const msgentry &b) { return a< b.timestamp  ;};
	static const float TIMEOUTSCALE=1.2;
};




template<class Data>
boost::shared_ptr<const Data> Blackboard::read_data(const std::string& type, const std::string&  host,boost::posix_time::ptime* const tmp  ,boost::posix_time::ptime const * const time_req )
{
    if(blkdata.find(type)==blkdata.end())
        return boost::shared_ptr<const Data>();
    historyqueue q=blkdata[type];
    historyqueue::reverse_iterator it=q.rbegin();
    if(it==q.rend())
        return boost::shared_ptr<const Data>();
    if(time_req==NULL)
    {
        while(it!=q.rend())
        {
            //cout<<(*it).msg->GetTypeName()<<endl;
            //if( process==""||(*it).publisher==process)
                if((*it).host==host||host=="")
                {
                    if(tmp!=NULL)
                        *tmp=(*it).timestamp;
                        /*
                    Data *p=static_cast<Data *>((*it).msg);
                    Data *cp=p->New();
                    cp->CopyFrom(*p);
                    return cp;*/
                    return  boost::static_pointer_cast<const Data>( (*it).msg);
                }
            ++it;
        }
        return boost::shared_ptr<const Data>();//NOT found
    }
    boost::posix_time::time_duration nt=boost::posix_time::microsec_clock::universal_time()-*time_req;
	nt*=TIMEOUTSCALE;
    if(blkdatatimeouts.find(type)==blkdatatimeouts.end())
		blkdatatimeouts[type]=nt;
	else
		blkdatatimeouts[type]= blkdatatimeouts[type]<nt?nt: blkdatatimeouts[type];

    historyqueue::const_iterator fit=q.begin();
    while(fit!=q.end())
    {
        //Skip
       // if( !(process==""||(*it).publisher==process) ||
             if(  ! ((*fit).host==host||host=="") )
               {
                   ++fit;
                    continue;
               }

        if(*time_req>(*fit).timestamp)
            ++fit;
        else
            break;
    }

    if(fit==q.end())
        --fit;
    if(fit==q.end())
    {
        return boost::shared_ptr<const Data>();
    }

    //if( !(process==""||(*it).publisher==process) ||
    if( ! ((*fit).host==host||host=="") )
        return boost::shared_ptr<const Data>();//;


    if(tmp!=NULL)
        *tmp=(*fit).timestamp;
        /*
    Data *p=static_cast<Data *>((*it).msg);
    Data *cp=p->New();
    cp->CopyFrom(*p);
    return cp;*/
    return  boost::static_pointer_cast<const Data>( (*fit).msg);
}
template<class Data>
boost::shared_ptr<const Data> Blackboard::read_signal(const std::string& type, const std::string&  host ,boost::posix_time::ptime* tmp )
{
    if(sigdata.find(type)==sigdata.end())
        return boost::shared_ptr<const Data>();

	if( ! (sigdata[type].d.host==host||host=="") )
        return boost::shared_ptr<const Data>();//;

    if(tmp!=NULL)
        *tmp=sigdata[type].d.timestamp;
    sigdata[type].cleared=true;
    return  boost::static_pointer_cast<const Data>( sigdata[type].d.msg);
}

template<class Data>
boost::shared_ptr<const Data> Blackboard::read_state(const std::string& type, const std::string&  host ,boost::posix_time::ptime* tmp )
{
	return read_data<Data>(type,host,tmp);
/*
    if(statedata.find(type)==statedata.end())
        return boost::shared_ptr<const Data>();
	if( ! (statedata[type].host==host||host=="") )
        return boost::shared_ptr<const Data>();//;

    if(tmp!=NULL)//Return timestamp :P
        *tmp=statedata[type].timestamp;

    return  boost::static_pointer_cast<const Data>(statedata[type].msg);
    */
}



#endif // BLACKBOARD_H
