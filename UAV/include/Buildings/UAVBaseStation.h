#ifndef __mixr_models_UAVBaseStation_H__
#define __mixr_models_UAVBaseStation_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: UAVBaseStation
// Factory name: UAVBaseStation
//------------------------------------------------------------------------------
class UAVBaseStation : public mixr::models::Player
{
    DECLARE_SUBCLASS(UAVBaseStation, mixr::models::Player)

public:
    UAVBaseStation();
	void initData();
	void UAVBaseStation::addUav(UAVMulticopter* const x);


private:
	bool setSlotNumberOfUavs(const mixr::base::Number* const);

	mixr::base::List* uavs{};
	
	unsigned int numberOfUavs{ 0 };

};

} // namespace models
} // namespace mixr

#endif
