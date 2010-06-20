/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef TYPE_FILTER_H
#define TYPE_FILTER_H
#include "filter.h"
class TypeFilter : public Filter
{
	public:
		TypeFilter(const std::string& filter_name);
		FilterState filter( const Tuple& t);
		void add_type(const std::string& new_type);
	private:
		std::vector<std::string> type_vector;
};

#endif // TYPE_FILTER_H
