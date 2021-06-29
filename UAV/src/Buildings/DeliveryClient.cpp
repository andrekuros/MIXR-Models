#include "asacomponents/include/Buildings/DeliveryClient.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(DeliveryClient, "DeliveryClient")

BEGIN_SLOTTABLE(DeliveryClient)
	"numberOfUavs"          //1) Number of Default UAVs automatic generated
END_SLOTTABLE(DeliveryClient)

BEGIN_SLOT_MAP(DeliveryClient)
	ON_SLOT(1, setSlotNumberOfUavs, mixr::base::Number)
END_SLOT_MAP()

EMPTY_DELETEDATA(DeliveryClient)
EMPTY_COPYDATA(DeliveryClient)

DeliveryClient::DeliveryClient()
{
    STANDARD_CONSTRUCTOR()	
	initData();
}

void DeliveryClient::initData()
{
	
	static mixr::base::String generic("DeliveryClient");
	setType(&generic);
	

}


////////////// SLOT Funcions /////////////////////////////////////

bool DeliveryClient::setSlotNumberOfUavs(const mixr::base::Number* const msg)
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