#ifndef __mixr_models_UAVControlStation_H__
#define __mixr_models_UAVControlStation_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

#define MAX_UAVS 255
#define MAX_PICKUP_CLIENTS 255
#define MAX_DELIVERY_CLIENTS 255
#define MAX_BASE_STATIONS 255


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
	void subscribeUav(UAVMulticopter * x);


private:
	bool setSlotNumberOfUavs(const mixr::base::Number* const);

	UAVMulticopter* uavs[MAX_UAVS];	
	UAVMulticopter* pickupClients[MAX_PICKUP_CLIENTS];	
	UAVMulticopter* deliveryClients[MAX_DELIVERY_CLIENTS];	
	UAVMulticopter* baseStations[MAX_BASE_STATIONS];

	unsigned int numberOfUAvs{ 0 };
	unsigned int numberOfpickupClients{ 0 };
	unsigned int numberOfdeliveryClients{ 0 };
	unsigned int numberOfbaseStations{ 0 };

	unsigned int numberOfUavs{ 0 };

};

} // namespace models
} // namespace mixr

#endif
