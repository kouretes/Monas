#include "Flooder.h"
#include "messages/TestMessage.pb.h"

#include "messages/Network.pb.h"

using namespace std;

//#define UNNEEDED

namespace {
	ActivityRegistrar<Drain>::Type drainreg("Drain");
}

namespace {
	ActivityRegistrar<Pipe>::Type pipereg("Pipe");
}


int Drain::Execute()
{
	KPROF_SCOPE(p,Drain::Execute());

	boost::shared_ptr<const KnownHosts> h= _blk->readState<KnownHosts>("communication");
	if(!h.get()||(h&&h->name_size()==0))
	{
		boost::shared_ptr<const TestMessage> drop= _blk->readSignal<TestMessage>("communication");
		if(drop==NULL)
			return 0;
		TestMessage newdrop;//.CopyFrom(drop);
		newdrop.set_counter(drop->counter()-1);
		_blk->publishSignal(newdrop,"communication");
		cout<<"Drop reduced to:"<<newdrop.counter()<<endl;


	}
	else
	{
		const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& rf=h->name();
		::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator fit;


		for(fit=rf.begin();fit!=rf.end();++fit)
		{
			boost::shared_ptr<const TestMessage> drop= _blk->readSignal<TestMessage>("communication",*fit);
			if(drop==NULL)
				continue;
			TestMessage newdrop;//.CopyFrom(drop);
			newdrop.set_counter(drop->counter()-1);
			_blk->publishSignal(newdrop,"communication");
			cout<<"Drop reduced to:"<<newdrop.counter()<<endl;

		}


	}





	p.generate_report();
	return 0;
}

void Drain::UserInit()
{
	cout<<"Hey!"<<endl;
	_blk->updateSubscription("communication",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
}



int Pipe::Execute()
{
	KPROF_SCOPE(p,Pipe::Execute());

	boost::shared_ptr<const KnownHosts> h= _blk->readState<KnownHosts>("communication");

	if(!h.get()||(h&&h->name_size()==0))
	{
		std::cout<<"Local"<<std::endl;
		boost::shared_ptr<const TestMessage> drop= _blk->readSignal<TestMessage>("communication");
		TestMessage newdrop;//.CopyFrom(drop);
		if(drop!=NULL)
			newdrop.set_counter(drop->counter()+1);
		else
			newdrop.set_counter(1);
		_blk->publishSignal(newdrop,"communication");
		cout<<"New drop to:"<<newdrop.counter()<<endl;
	}
	else
	{

		const ::google::protobuf::RepeatedField< ::google::protobuf::uint32 >& rf=h->name();
		::google::protobuf::RepeatedField< ::google::protobuf::uint32 >::const_iterator fit;
		for(fit=rf.begin();fit!=rf.end();++fit)
		{
			std::cout<<"Readingfromhost:"<<*fit<<std::endl;


			boost::shared_ptr<const TestMessage> drop= _blk->readSignal<TestMessage>("communication",*fit);
			TestMessage newdrop;//.CopyFrom(drop);
			if(drop!=NULL)
				newdrop.set_counter(drop->counter()+1);
			else
				newdrop.set_counter(1);
			_blk->publishSignal(newdrop,"communication");
			cout<<"New drop to:"<<newdrop.counter()<<endl;

		}



	}


	p.generate_report();
	return 0;
}

void Pipe::UserInit()
{
	cout<<"Hey!"<<endl;
	_blk->updateSubscription("communication",msgentry::SUBSCRIBE_ON_TOPIC,msgentry::HOST_ID_ANY_HOST);
}
