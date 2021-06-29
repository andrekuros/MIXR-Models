#ifndef __mixr_models_UAVMulticopter_H__
#define __mixr_models_UAVMulticopter_H__

#include "mixr/models/player/air/UnmannedAirVehicle.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: UAVMulticopter
// Factory name: UAVMulticopter
//------------------------------------------------------------------------------
class UAVMulticopter : public mixr::models::UnmannedAirVehicle
{
    DECLARE_SUBCLASS(UAVMulticopter, mixr::models::UnmannedAirVehicle)

public:
    UAVMulticopter();
};

} // namespace models
} // namespace mixr

#endif
