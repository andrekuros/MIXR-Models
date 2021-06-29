#include "asacomponents/include/UAV/UAVMulticopterAgent.h"
#include "asacomponents/include/UAV/UAVMulticopterState.h"

namespace asa {
namespace ubf {

	IMPLEMENT_SUBCLASS(UAVMulticopterAgent, "UAVMulticopterAgent")
	EMPTY_COPYDATA(UAVMulticopterAgent)

	BEGIN_SLOTTABLE(UAVMulticopterAgent)
	"externalProcessor",
	END_SLOTTABLE(UAVMulticopterAgent)

    BEGIN_SLOT_MAP(UAVMulticopterAgent) 		
        ON_SLOT(1, setSlotExternalProcessor, asa::processor::ExternalProcessor) 
		END_SLOT_MAP()

	BEGIN_EVENT_HANDLER(UAVMulticopterAgent)
		// AsaEvent
		ON_EVENT_OBJ(ASA_EVENTS, onAsaEvents, asa::event::AsaEventObject)
	END_EVENT_HANDLER()

		//------------------------------------------------------------------------------
		// Constructor
		//------------------------------------------------------------------------------
	UAVMulticopterAgent::UAVMulticopterAgent()
	{
		STANDARD_CONSTRUCTOR()
				
	}	

//------------------------------------------------------------------------------
// deleteData
//------------------------------------------------------------------------------
void UAVMulticopterAgent::deleteData()
{
   
    if (processor != nullptr)
        processor->unref();

    BaseClass::deleteData();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void UAVMulticopterAgent::reset()
{
    
    // Atualização de Seed dos Behaviours
    std::mt19937 gen(getComponentSeed());
    mixr::base::Component* beh = (mixr::base::Component*)getBehavior(); // setComponentSeed(gen());

	if (beh != nullptr) beh->setComponentSeed(gen());
	
	if (newScene) updateNewScene();

    BaseClass::reset();
}

//------------------------------------------------------------------------------
// updateData
//------------------------------------------------------------------------------
void UAVMulticopterAgent::updateData(const double dt) 
{ 
	BaseClass::updateData(dt); 				
}


void UAVMulticopterAgent::setPOIs()
{
	if (autoMode != UAVAutoMode::NONE && automodePointMap.size() >= 2)
	{
		if (nextWpt.wptId != 0 && lastWpt.wptId != 0)
		{
			nextWpt.POI = automodePointMap[nextWpt.wptId];
			lastWpt.POI = automodePointMap[lastWpt.wptId];						
		}
	}
}


void UAVMulticopterAgent::updateNewScene()
{	

	newScene = false;
	//Points of Interest Identification (TODO: jogar para Control Station)

	const asa::components::UAVMulticopter* uv{ dynamic_cast<const asa::components::UAVMulticopter*>(this->container()) };
	const mixr::models::WorldModel* wm = static_cast<const mixr::models::WorldModel*>(uv->getWorldModel());
	const mixr::simulation::Station* station = static_cast<const mixr::simulation::Station*>(wm->getStation());
	const mixr::simulation::Simulation* sim = static_cast<const mixr::simulation::Simulation*>(station->getSimulation());

	const mixr::base::PairStream* plist = sim->getPlayers();
	const mixr::base::List::Item* itemPl{ plist->getFirstItem() };

	while (itemPl != nullptr)
	{
		const mixr::base::Pair* pairPl{ static_cast<const mixr::base::Pair*>(itemPl->getValue()) };
		const mixr::models::Player* p{ static_cast<const mixr::models::Player*>(pairPl->object()) };

		//Procura Bases do cenário
		const asa::components::UAVBaseStation* base{ dynamic_cast<const asa::components::UAVBaseStation*> (p) };
		if (base != nullptr)
		{
			haveBase = true;
		}

		//Identificar pontos de Avoid
		const asa::components::POI* poi{ dynamic_cast<const asa::components::POI*> (p) };
		if (poi != nullptr)
		{
			if (poi->type == asa::components::POI_Type::AVOID)
				avoidPointMap[poi->poiId] = poi;

			if (poi->type == asa::components::POI_Type::WPT)
				wptPointMap[poi->poiId] = poi;

			if (poi->type == asa::components::POI_Type::AUTOMODE)
				automodePointMap[poi->poiId] = poi;

			if (poi->type == asa::components::POI_Type::ENDPOINT)
				endPointMap[poi->poiId] = poi;
		}
		itemPl = itemPl->getNext();
	}	
}


//------------------------------------------------------------------------------
// Slot functions
//------------------------------------------------------------------------------
bool UAVMulticopterAgent::setSlotExternalProcessor(processor::ExternalProcessor* const msg)
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