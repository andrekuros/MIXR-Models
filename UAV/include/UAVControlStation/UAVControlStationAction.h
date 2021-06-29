#ifndef __asa_ubf_UAVControlStationAction_H__
#define __asa_ubf_UAVControlStationAction_H__

#include "mixr/base/ubf/AbstractAction.hpp"

#include "mixr/models/player/air/AirVehicle.hpp"

//------------------------------------------------------------------------------
// Class: UAVControlStationAction
//
// Description:
//------------------------------------------------------------------------------
namespace asa {
namespace ubf {
class UAVControlStationState;


class UAVControlStationAction : public mixr::base::ubf::AbstractAction
{
	DECLARE_SUBCLASS(UAVControlStationAction, mixr::base::ubf::AbstractAction)

public:
	UAVControlStationAction();												

	bool execute(mixr::base::Component* actor) override;

private:
				

};

}
}
#endif
