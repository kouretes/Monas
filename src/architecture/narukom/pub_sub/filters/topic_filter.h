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

#ifndef TOPIC_FILTER_H
#define TOPIC_FILTER_H
#include "filter.h"

class TopicFilter : public Filter
{
public:
	TopicFilter(const std::string& type_name, const std::string& topic_filter, bool value = false);
	virtual FilterState filter(const msgentry& a_tuple);
private:
	std::string topic;
};

#endif // METADATA_FILTER_H
