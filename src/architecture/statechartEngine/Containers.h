#ifndef CONTAINERS_H_
#define CONTAINERS_H_ 1

#include <list>
#include <vector>

namespace statechart_engine
{

	class State;
	class OrState;

	class Containers
	{

	public:

		typedef std::list<State*> StateContainer;
		typedef StateContainer::iterator StateContainerIterator;
		typedef StateContainer::const_iterator StateContainerIteratorConst;

		typedef std::vector<OrState*> OrStateContainer;
		typedef OrStateContainer::iterator OrStateContainerIterator;
		typedef OrStateContainer::const_iterator OrStateContainerIteratorConst;

		typedef std::vector<TransitionSegmentBase*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionContainerIterator;
		typedef TransitionContainer::const_iterator TransitionContainerIteratorConst;


	};

}

#endif /* CONTAINERS_H_ */
