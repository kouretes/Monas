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

#ifndef TOPICTREE_H
#define TOPICTREE_H
#include "msg.h"
#include "stringRegistry.h"
#include <map>
#include <set>
#include <string>

class TopicTree: public stringRegistry
{

    public:
	TopicTree(std::string const& root);
    std::set<std::size_t> iterateTopics(std::string const& topic , int where) const;
    void addTopic(std::string const& what,std::string const& under);
    private:
    typedef struct topicdata_s{
  		std::size_t parentid;
  		std::set<std::size_t> children;

  	} topicdata;
	//Unlocked, stable and untouched while running
	//stringRegistry topicRegistry;
  	std::map<std::size_t,topicdata > topictree;
	void iterateTopicsBelow(std::set<std::size_t>  &res,std::size_t const topicid) const;

};


#endif /* TOPICTREE_H */
