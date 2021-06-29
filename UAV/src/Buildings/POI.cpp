#include "asacomponents/include/Buildings/POI.h"

namespace asa {
namespace components {

IMPLEMENT_SUBCLASS(POI, "POI")

BEGIN_SLOTTABLE(POI)
	"radius",          //1) Ref radius distance to avoid
	"poiType",          //1) Ref radius distance to avoid
	"poiId",
END_SLOTTABLE(POI)

BEGIN_SLOT_MAP(POI)
	ON_SLOT(1, setSlotRadius, mixr::base::Number)
	ON_SLOT(2, setSlotType, mixr::base::String)
	ON_SLOT(3, setSlotPoiID, mixr::base::Number)
END_SLOT_MAP()

EMPTY_DELETEDATA(POI)
EMPTY_COPYDATA(POI)

POI::POI()
{
    STANDARD_CONSTRUCTOR()	
	initData();
}

void POI::initData()
{	
	static mixr::base::String generic("POI");
	setType(&generic);
}


////////////// SLOT Funcions /////////////////////////////////////

bool POI::setSlotRadius(const mixr::base::Number* const msg)
{
	bool ok{};
	if (msg != nullptr)
	{
		double value{ msg->getReal() };
		radius = value;
		ok = true;
	}
	return ok;
}

bool POI::setSlotPoiID(const mixr::base::Number* const msg)
{
	bool ok{};
	if (msg != nullptr)
	{
		int value{ msg->getInt() };
		poiId = value;
		ok = true;
	}
	return ok;
}


bool POI::setSlotType(const mixr::base::String* const msg)
{
	bool ok{};
	if (*msg == "WPT" || *msg == "wpt") { type = POI_Type::WPT; ok = true; }
	else if (*msg == "AVOID" || *msg == "avoid") { type = POI_Type::AVOID; ok = true; }
	else if (*msg == "ENDPOINT" || *msg == "endpoint") { type = POI_Type::ENDPOINT; ok = true; }
	else if (*msg == "AUTOMODE" || *msg == "automode") { type = POI_Type::AUTOMODE; ok = true; }	
	return ok;
}

} // namespace models
} // namespace mixr