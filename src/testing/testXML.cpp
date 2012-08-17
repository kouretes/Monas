#include <iostream>

#include <string>
#include "tools/XML.h"

template<class T>
void printer ( T el )
{
	std::cout << "Element : " << el << std::endl;
}

template<class T>
void printer ( std::pair<const std::string, T> el)
{
	std::cout << "Element : " << el.first << " " << el.second << std::endl;
}

template<class T>
void printer ( const std::vector<T> el)
{
	int i = 0;

	for ( typename std::vector<T>::const_iterator it = el.begin(); it != el.end(); ++it, i++)
	{
		std::cout << "Tuple " << i << " ";
		printer ( (*it) );
	}
}

template<class Y, class T>
void printer ( const std::map<Y, T> el)
{
	int i = 0;

	for ( typename std::map<Y, T>::const_iterator it = el.begin(); it != el.end(); ++it, i++)
	{
		std::cout << "Tuple " << i << " ";
		printer ( (*it) );
	}
}

template<class A, class B, class C>
void printer (const XMLNode<A, B, C> el)
{
	std::cout << "Element : " << el.name << " " << el.value << std::endl;
	printer(el.attrb);
}

int main()
{
	XML XmlFile("test.xml");
	std::vector<XMLNode<std::string, double> > agents;
	agents = XmlFile.QueryElement<std::string, double, std::string> ( "agent" );
	printer( agents );

	for ( unsigned int i = 0; i < agents.size(); i++ )
	{
		std::vector<XMLNode<std::string, double> > modules;
		modules = XmlFile.QueryElement<std::string, double, std::string> ( "module", &(agents[i]) );
		printer( modules );
	}

	return 0;
}

