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

#include "time_filter.h"
TimeFilter::TimeFilter(const std::string& type_name,unsigned int period):Filter(type_name)
{
  this->period = boost::posix_time::milliseconds(period);
  last_accepted = boost::posix_time::min_date_time;
}

FilterState TimeFilter::filter(const Tuple& a_tuple)
{
  if(last_accepted == boost::posix_time::min_date_time)
  {
    last_accepted = boost::posix_time::microsec_clock::local_time();
    return Accepted;
  }
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  if(now - last_accepted > period)
  {
    last_accepted = now;
    return Accepted;
  }
  else
    return Rejected;
}
