#include "asacomponents/include/UAVControlStation/UAVControlStationAction.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationState.h"

#include "mixr/models/player/air/AirVehicle.hpp"
#include "mixr/models/system/Autopilot.hpp"

#include "mixr/models/navigation/Navigation.hpp"
#include "mixr/models/navigation/Route.hpp"

namespace asa {
namespace ubf {
	IMPLEMENT_SUBCLASS(UAVControlStationAction, "UAVControlStationAction")
	EMPTY_COPYDATA(UAVControlStationAction)
	EMPTY_DELETEDATA(UAVControlStationAction)
	EMPTY_SLOTTABLE(UAVControlStationAction)

	UAVControlStationAction::UAVControlStationAction()
	{
		STANDARD_CONSTRUCTOR()

	}

	//------------------------------------------------------------------------------
	// execute
	//------------------------------------------------------------------------------
	bool UAVControlStationAction::execute(mixr::base::Component* actor)
	{
		//mixr::models::AirVehicle* av{ dynamic_cast<mixr::models::AirVehicle*>(actor) };


		// Let AsaAirVehicleAction take care of autopilot
		return true;
	}

}
}