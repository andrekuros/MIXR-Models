#include "asacomponents/include/UAVControlStation/UAVControlStationBeh.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationAction.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationState.h"

#include "mixr/models/player/air/AirVehicle.hpp"
#include "mixr/models/system/Autopilot.hpp"

namespace asa {
namespace ubf {

	IMPLEMENT_SUBCLASS(UAVControlStationBeh, "UAVControlStationBeh")
	EMPTY_SLOTTABLE(UAVControlStationBeh)
	EMPTY_COPYDATA(UAVControlStationBeh)
	EMPTY_DELETEDATA(UAVControlStationBeh)

	UAVControlStationBeh::UAVControlStationBeh()
	{
		STANDARD_CONSTRUCTOR()
	}

	mixr::base::ubf::AbstractAction* UAVControlStationBeh::genAction(const mixr::base::ubf::AbstractState* const state, const double dt)
	{
			
		UAVControlStationAction* action{};

		const UAVControlStationState* uavState{ dynamic_cast<const UAVControlStationState*>(state) };

		if (uavState != nullptr)
		{				
	

		}		

		return action;
	}
}
}