#include "asacomponents/include/UAVControlStation/UAVControlStation.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(UAVControlStation, "UAVControlStation")

BEGIN_SLOTTABLE(UAVControlStation)
	"numberOfUavs"          //1) Number of Default UAVs automatic generated
END_SLOTTABLE(UAVControlStation)

BEGIN_SLOT_MAP(UAVControlStation)
	ON_SLOT(1, setSlotNumberOfUavs, mixr::base::Number)
END_SLOT_MAP()

EMPTY_DELETEDATA(UAVControlStation)
EMPTY_COPYDATA(UAVControlStation)

UAVControlStation::UAVControlStation()
{
    STANDARD_CONSTRUCTOR()	
	initData();
}

void UAVControlStation::initData()
{
	
	static mixr::base::String generic("UAVControlStation");
	setType(&generic);

}

////////////// SLOT Funcions /////////////////////////////////////
////////////// SLOT Funcions /////////////////////////////////////

bool UAVControlStation::setSlotNumberOfUavs(const mixr::base::Number* const msg)
{
	bool ok{};
	if (msg != nullptr)
	{
		numberOfUavs = msg->getInt();
	}
	return ok;
}



} // namespace models
} // namespace mixr