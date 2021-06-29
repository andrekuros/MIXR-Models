#include "asacomponents/include/Buildings/UAVBaseStation.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(UAVBaseStation, "UAVBaseStation")

BEGIN_SLOTTABLE(UAVBaseStation)
	"numberOfUavs"          //1) Number of Default UAVs automatic generated
END_SLOTTABLE(UAVBaseStation)

BEGIN_SLOT_MAP(UAVBaseStation)
	ON_SLOT(1, setSlotNumberOfUavs, mixr::base::Number)
END_SLOT_MAP()

EMPTY_DELETEDATA(UAVBaseStation)
EMPTY_COPYDATA(UAVBaseStation)

UAVBaseStation::UAVBaseStation()
{
    STANDARD_CONSTRUCTOR()	
	initData();
}

void UAVBaseStation::initData()
{
	
	static mixr::base::String generic("UAVBaseStation");
	setType(&generic);

	//Initialize list of components
	uavs = new mixr::base::List();
	
	//Generate default UAVs
	for (int n = 0; n < numberOfUavs; n++)
	{		
		UAVMulticopter* auxUav = new UAVMulticopter();
		addUav(auxUav);
	}

}

//------------------------------------------------------------------------------
// addUAvs() - add a new uav
//------------------------------------------------------------------------------
void UAVBaseStation::addUav(UAVMulticopter* x)
{
	uavs->addTail(x);
	x->container(this);
}


////////////// SLOT Funcions /////////////////////////////////////

bool UAVBaseStation::setSlotNumberOfUavs(const mixr::base::Number* const msg)
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