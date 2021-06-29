#ifndef __mixr_models_DeliveryClient_H__
#define __mixr_models_DeliveryClient_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: DeliveryClient
// Factory name: DeliveryClient
//------------------------------------------------------------------------------
class DeliveryClient : public mixr::models::Player
{
    DECLARE_SUBCLASS(DeliveryClient, mixr::models::Player)

public:
    DeliveryClient();
	void initData();	


private:
	bool setSlotNumberOfUavs(const mixr::base::Number* const);
	unsigned int numberOfUavs;
};

} // namespace models
} // namespace mixr

#endif
