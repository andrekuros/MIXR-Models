#include "asacomponents/include/UAVControlStation/UAVControlStationState.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationAgent.h"
#include "asacomponents/include/UAVControlStation/UAVControlStation.h"

#include "mixr/models/navigation/Navigation.hpp"
#include "mixr/models/navigation/Route.hpp"

namespace asa {
namespace ubf {

IMPLEMENT_SUBCLASS(UAVControlStationState, "UAVControlStationState")
EMPTY_COPYDATA(UAVControlStationState)
EMPTY_SLOTTABLE(UAVControlStationState)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
UAVControlStationState::UAVControlStationState() {

	STANDARD_CONSTRUCTOR() 

}


//------------------------------------------------------------------------------
// deleteData
//------------------------------------------------------------------------------
void UAVControlStationState::deleteData()
{
	BaseClass::deleteData();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void UAVControlStationState::reset()
{
	BaseClass::reset();
}

//------------------------------------------------------------------------------
// updateState
//------------------------------------------------------------------------------
void UAVControlStationState::updateState(const mixr::base::Component* const actor)
{

	BaseClass::updateState(actor);

	// Get and cast required pointers
	const UAVControlStationAgent* agent{};
	const UAVControlStationState* initState{};

	/*const asa::components::UAVControlStation* uav{ dynamic_cast<const asa::components::UAVControlStation*>(actor) };

	if (uav != nullptr)
	{
		const mixr::base::Pair* pairNav{ uav->findByType(typeid(mixr::models::Navigation)) };
		if (pairNav != nullptr)
		{
			const mixr::models::Navigation* nav = static_cast<const mixr::models::Navigation*>(pairNav->object());
			if (nav != nullptr)
			{
				const mixr::models::Route* route = nav->getPriRoute();

				if (route != nullptr)
				{					
					nextWpt = route->getStptIndex(); 										
				}
			}
		}
	}*/
}


//------------------------------------------------------------------------------
// getAgent - pre-ref
	//------------------------------------------------------------------------------
	UAVControlStationAgent* UAVControlStationState::getAgent()
	{
		UAVControlStationAgent* asaAgent{};

		if (hasActor)
		{
			mixr::base::Pair* pairAgent{ actor->findByType(typeid(UAVControlStationAgent)) };
			if (pairAgent != nullptr)
			{
				UAVControlStationAgent* agent{ static_cast<UAVControlStationAgent*>(pairAgent->object()) };
				if (agent != nullptr)
				{
					asaAgent = agent;

					if (asaAgent != nullptr)
						asaAgent->ref();
				}
			}
		}

		return asaAgent;
	}

	const UAVControlStationAgent* UAVControlStationState::getAgent() const
	{
		const UAVControlStationAgent* asaAgent{};

		if (hasActor)
		{
			const mixr::base::Pair* pairAgent{ actor->findByType(typeid(UAVControlStationAgent)) };
			if (pairAgent != nullptr)
			{
				const UAVControlStationAgent* agent{ static_cast<const UAVControlStationAgent*>(pairAgent->object()) };
				if (agent != nullptr)
				{
					asaAgent = agent;

					if (asaAgent != nullptr)
						asaAgent->ref();
				}
			}
		}

		return asaAgent;
	}

} // namespace ubf
} // namespace asa