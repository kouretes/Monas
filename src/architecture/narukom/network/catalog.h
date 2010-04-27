/*
 * Catalog.h
 *
 *  Created on: Mar 29, 2010
 *      Author: vagvaz
 */

#ifndef CATALOG_H_
#define CATALOG_H_

#include "Thread.h"
#include "publisher.h"
#include "subscriber.h"
#include "channel_info.h"
#include <map>


class Catalog: public Thread, public Publisher, public Subscriber {
public:
	Catalog();
	virtual ~Catalog();
	virtual void run();
	virtual void publish(google::protobuf::Message* msg);
	virtual void protcess_messages();
private:
	std::map<ChannelInfo> channels;

};

#endif /* CATALOG_H_ */
