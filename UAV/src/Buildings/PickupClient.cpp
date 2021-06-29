#include "asacomponents/include/Buildings/PickupClient.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(PickupClient, "PickupClient")

BEGIN_SLOTTABLE(PickupClient)
	"numberOfUavs"          //1) Number of Default UAVs automatic generated
END_SLOTTABLE(PickupClient)

BEGIN_SLOT_MAP(PickupClient)
	ON_SLOT(1, setSlotNumberOfUavs, mixr::base::Number)
END_SLOT_MAP()

EMPTY_DELETEDATA(PickupClient)
EMPTY_COPYDATA(PickupClient)

PickupClient::PickupClient()
{
    STANDARD_CONSTRUCTOR()	
	initData();
}

void PickupClient::initData()
{	
	static mixr::base::String generic("PickupClient");
	setType(&generic);
}

////////////// SLOT Funcions /////////////////////////////////////

bool PickupClient::setSlotNumberOfUavs(const mixr::base::Number* const msg)
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