#ifndef _XMLConfig_h_
#define _XMLConfig_h_ 1

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <utility>

#include "../external/tinyxml_2-5-3/tinyxml.h"


class XMLConfig
{

public:

	XMLConfig ( const std::string & filename)
	{
		load_successful = doc.LoadFile( filename );

		if ( ! load_successful )
			doc.LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
	}

	bool IsLoadedSuccessfully () const
	{
		return load_successful;
	}

	bool SaveConfiguration ()
	{
		return doc.SaveFile();
	}

	template <class T>
	bool SetElement (const std::string & ElName, const T & Value, bool isIterative = false)
	{
		std::ostringstream strs;
		strs << Value;
		TiXmlElement * element = doc.FirstChildElement ( ElName );

		if ( element == NULL || isIterative )
		{
			element = new TiXmlElement ( ElName );
			doc.LinkEndChild(element);
		}

		if ( element->FirstChild() == NULL )
			element->LinkEndChild( new TiXmlText ( strs.str() ) );
		else
			element->FirstChild()->ToText()->SetValue( strs.str() );

		return true;
	}

	template <class T>
	bool QueryElement (const std::string & ElName, T & Value) const
	{
		std::vector<T> tmpVector;
		bool found = QueryElement( ElName, tmpVector );

		if (!found) return false;

		Value = tmpVector[0];
		return true;
	}

	template <class T>
	bool QueryElement (const std::string & ElName, std::vector<T> & Value) const
	{
		Value.clear();

		for ( const TiXmlNode * currentNode = doc.FirstChild( ElName ); currentNode != NULL ; currentNode = doc.IterateChildren( ElName , currentNode ) )
		{
			const TiXmlElement * element = currentNode->ToElement();

			if ( element == NULL ) continue;

			std::string elText;
			elText.assign( element->GetText() );

			if ( elText.size() == 0 ) continue;

			std::istringstream strmElText(elText);
			T  tTypeVal;
			strmElText >> tTypeVal;
			Value.push_back( tTypeVal );
		}

		return Value.size() == 0 ? false : true;
	}

	template <class T>
	bool SetElement (const std::string & ElName, const std::map<std::string, T > & Values, bool isIterative = false)
	{
		TiXmlElement * element = doc.FirstChildElement ( ElName );

		if ( element == NULL || isIterative )
		{
			element = new TiXmlElement ( ElName );
			doc.LinkEndChild(element);
		}

		for ( typename std::map<std::string, T>::const_iterator it = Values.begin(); it != Values.end(); it++ )
		{
			std::ostringstream strs;
			strs << (*it).second;
			element->SetAttribute( (*it).first , strs.str() );
		}

		return true;
	}

	template < class T >
	bool QueryElement (const std::string & ElName, std::map< std::string, T > & Values)
	{
		std::vector< std::map< std::string, T > > tmpVector;
		bool found = QueryElement( ElName, tmpVector);

		if (!found) return false;

		Values = tmpVector[0];
		return true;
	}

	template < class T >
	bool QueryElement (const std::string & ElName, std::vector < std::map< std::string, T > >& Values)
	{
		Values.clear();

		for ( const TiXmlNode * currentNode = doc.FirstChild( ElName ); currentNode != NULL ; currentNode = doc.IterateChildren( ElName , currentNode ) )
		{
			const TiXmlElement * element = currentNode->ToElement();

			if ( element == NULL ) continue;

			const TiXmlAttribute * pAttrib = element->FirstAttribute();
			std::map< std::string, T >  aMap;

			while ( pAttrib )
			{
				std::istringstream strs( pAttrib->Value() );
				T theValue;
				strs >> theValue;
				aMap.insert( std::pair<std::string, T> ( pAttrib->Name(), theValue ) );
				pAttrib = pAttrib->Next();
			}

			Values.push_back( aMap );
		}

		return Values.size() == 0 ? false : true;
	}


private:

	TiXmlDocument doc;

	bool load_successful;
};

#endif // _XMLConfig_h_
