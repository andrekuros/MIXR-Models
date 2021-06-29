#include "asacomponents/include/UAV/UAVMulticopter.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(UAVMulticopter, "UAVMulticopter")
EMPTY_SLOTTABLE(UAVMulticopter)
EMPTY_DELETEDATA(UAVMulticopter)
EMPTY_COPYDATA(UAVMulticopter)

UAVMulticopter::UAVMulticopter()
{
    STANDARD_CONSTRUCTOR()
    static mixr::base::String generic("UAVMulticopter");
    setType(&generic);
}

} // namespace models
} // namespace mixr