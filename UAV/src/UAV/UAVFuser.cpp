#include "asacomponents/include/UAV/UAVFuser.h"
#include "asacomponents/include/UAV/UAVMulticopterAction.h"
#include "mixr/base/List.hpp"

namespace asa {
namespace ubf {

IMPLEMENT_SUBCLASS(UAVFuser, "UAVFuser")
EMPTY_SLOTTABLE(UAVFuser)
EMPTY_COPYDATA(UAVFuser)

UAVFuser::UAVFuser()
{
	STANDARD_CONSTRUCTOR()
}

void UAVFuser::deleteData()
{
	BaseClass::deleteData();
}

mixr::base::ubf::AbstractAction * UAVFuser::genComplexAction(mixr::base::List * const actionSet)
{
	UAVMulticopterAction* complexAction = new UAVMulticopterAction();

	// process entire action set
	mixr::base::List::Item* item = actionSet->getFirstItem();

	while (item != nullptr)
	{
		UAVMulticopterAction* action = dynamic_cast<UAVMulticopterAction*>(item->getValue());

		if (action != nullptr)
		{			
			// Autopilot
			if (action->getAutopilotActionFlag())
			{
				complexAction->copyAutopilotActionFrom(action);
			}

			// Navigation
			if (action->getNavigationActionFlag())
			{
				complexAction->copyNavigationActionFrom(action);
			}

			// AutoMode
			if (action->getAutoModeActionFlag())
			{
				complexAction->copyAutoModeActionFrom(action);
			}
			
		}

		// next action
		item = item->getNext();
	}
	return complexAction;
}

}
}


