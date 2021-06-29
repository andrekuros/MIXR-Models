#include "asacomponents/include/UAVControlStation/UAVControlStationAgent.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationState.h"

namespace asa {
namespace ubf {

	IMPLEMENT_SUBCLASS(UAVControlStationAgent, "UAVControlStationAgent")
	EMPTY_COPYDATA(UAVControlStationAgent)

	BEGIN_SLOTTABLE(UAVControlStationAgent)
	"externalProcessor",
	END_SLOTTABLE(UAVControlStationAgent)

    BEGIN_SLOT_MAP(UAVControlStationAgent) 		
        ON_SLOT(1, setSlotExternalProcessor, asa::processor::ExternalProcessor) 
		END_SLOT_MAP()

	BEGIN_EVENT_HANDLER(UAVControlStationAgent)
		// AsaEvent
		ON_EVENT_OBJ(ASA_EVENTS, onAsaEvents, asa::event::AsaEventObject)
	END_EVENT_HANDLER()

		//------------------------------------------------------------------------------
		// Constructor
		//------------------------------------------------------------------------------
	UAVControlStationAgent::UAVControlStationAgent()
	{
		STANDARD_CONSTRUCTOR()
	}	

//------------------------------------------------------------------------------
// deleteData
//------------------------------------------------------------------------------
void UAVControlStationAgent::deleteData()
{
   
    if (processor != nullptr)
        processor->unref();

    BaseClass::deleteData();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void UAVControlStationAgent::reset()
{
    
    // Atualização de Seed dos Behaviours
    std::mt19937 gen(getComponentSeed());
    mixr::base::Component* beh = (mixr::base::Component*)getBehavior(); // setComponentSeed(gen());

	if (beh != nullptr) beh->setComponentSeed(gen());

    BaseClass::reset();
}

//------------------------------------------------------------------------------
// updateData
//------------------------------------------------------------------------------
void UAVControlStationAgent::updateData(const double dt) { BaseClass::updateData(dt); }


//------------------------------------------------------------------------------
// Slot functions
//------------------------------------------------------------------------------
bool UAVControlStationAgent::setSlotExternalProcessor(processor::ExternalProcessor* const msg)
{
    bool ok{};

    if (msg != nullptr)
    {
        processor = msg;
        processor->ref();

        ok = true;
    }

    return ok;
}

} // namespace ubf
} // namespace asa