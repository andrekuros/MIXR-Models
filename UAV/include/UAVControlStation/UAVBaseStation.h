#ifndef __mixr_models_UAVControlStation_H__
#define __mixr_models_UAVControlStation_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: UAVControlStation
// Factory name: UAVControlStation
//------------------------------------------------------------------------------
class UAVControlStation : public mixr::models::Player
{
    DECLARE_SUBCLASS(UAVControlStation, mixr::models::Player)

public:
    UAVControlStation();
	void initData();
	void UAVControlStation::addUav(UAVMulticopter* const x);


private:
	bool setSlotNumberOfUavs(const mixr::base::Number* const);

	mixr::base::List* uavs{};
	mixr::base::List* pickups{};
	mixr::base::List* clients{};
	mixr::base::List* rechargeStations{};

	unsigned int numberOfUavs{ 0 };

};

} // namespace models
} // namespace mixr

#endif
