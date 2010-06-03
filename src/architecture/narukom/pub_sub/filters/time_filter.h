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

#ifndef TIME_FILTER_H
#define TIME_FILTER_H
#include "filter.h"
#include <boost/date_time/posix_time/posix_time.hpp>
class TimeFilter : public Filter
{
  public:
    TimeFilter(const std::string& type_name,unsigned period = 100);
    virtual FilterState filter(const Tuple& a_tuple);
  private:
    boost::posix_time::time_duration period;
    boost::posix_time::ptime last_accepted;
};

#endif // TIME_FILTER_H
