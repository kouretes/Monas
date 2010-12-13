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

#ifndef STRINGREGISTRY_H
#define STRINGREGISTRY_H

/*
#include <string>
#include <boost/functional/hash.hpp>
#include <map>
#include <set>

class stringRegistry {

	public:

	stringRegistry(){};
	std::size_t registerNew(std::string const& s)
	{
		std::size_t hashs=hasher(s);
		fttype::iterator fit= ft.find(hashs);
		if(fit!=

		std::vector<stringidpair>::iterator fit;
		for(fit= ft[hashs].begin();fit!=ft:end();

		if


	}


	private:
	typedef struct {
		std::string s;
		std::size_t id;
		bool operator <(stringidpair const & a) { this.id < a.id;};
	}stringidpair;
	typename std::map<std::size_t,std::set<stringidpair>> fttype;
	typename std::map<std::size_t, std::string> rttype;
	boost:hash<std::string> hasher;
	fttype ft;//from hash to id;
	rttype rt;//from id to string :)

}


*/

#endif /* STRINGREGISTRY_H */
