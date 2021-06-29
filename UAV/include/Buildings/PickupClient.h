#ifndef __mixr_models_PickupClient_H__
#define __mixr_models_PickupClient_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: PickupClient
// Factory name: PickupClient
//------------------------------------------------------------------------------
class PickupClient : public mixr::models::Player
{
    DECLARE_SUBCLASS(PickupClient, mixr::models::Player)

public:
    PickupClient();
	void initData();


private:
	bool setSlotNumberOfUavs(const mixr::base::Number* const);



	unsigned int numberOfUavs{ 0 };

};

} // namespace models
} // namespace mixr

#endif
