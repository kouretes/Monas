#ifndef IACTION_H_
#define IACTION_H_ 1

#include "architecture/narukom/narukom.h"
#include "architecture/narukom/pub_sub/blackboard.h"

#include "architecture/IExecutable.h"

namespace statechart_engine
{

	class Statechart;

	class IAction: public IExecutable
	{

	public:

		void Initialize ( Narukom*, Blackboard*, Statechart* );

		virtual void UserInit () = 0;

	protected:

		Narukom* _com;

		Blackboard* _blk;

		Statechart* _s;

	};

}

#endif /* IACTION_H_ */
