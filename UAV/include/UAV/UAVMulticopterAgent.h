#ifndef __asa_ubf_UAVMulticopterAgent_H__
#define __asa_ubf_UAVMulticopterAgent_H__

#include "mixr/base/ExternalProcessor.hpp"
#include "mixr/base/Object.hpp"
#include "mixr/base/ubf/Agent.hpp"

#include "mixr/models/WorldModel.hpp"
#include "mixr/simulation/Station.hpp"
#include "mixr/simulation/Simulation.hpp"
#include "mixr/base/PairStream.hpp"
#include "asacomponents/include/Buildings/UAVBaseStation.h"

#include "asacomponents/include/UAV/UAVMulticopter.h"
#include "asacomponents/include/UAV/UAVMulticopterState.h"
#include "asacomponents/include/Buildings/POI.h"

//------------------------------------------------------------------------------
// Class: UAVMulticopterAgent
//
// Description:
//------------------------------------------------------------------------------

namespace asa {
namespace ubf {

typedef struct {
	int wptId;
	double offSetX;
	double offSetY;
	const asa::components::POI* POI;
}agentWpt;

class UAVMulticopterAgent : public mixr::base::ubf::Agent
{
    DECLARE_SUBCLASS(UAVMulticopterAgent, mixr::base::ubf::Agent)

public:
    UAVMulticopterAgent();	
	     
    processor::ExternalProcessor* getExternalProcessor() { return processor; }
    const processor::ExternalProcessor* getExternalProcessor() const { return processor; }

    // Interface for handle AsaEvents
    bool onAsaEvents(asa::event::AsaEventObject* msg) override { return false; }
	  
    //-------------------------------------------
    // Inheritance
    void reset() override;
    void updateData(const double dt) override;
    bool event(const int event, Object* const obj = nullptr) override;

	//Armazenam dados sobre o próximo ponto de interesse e o anterior
	agentWpt nextWpt =  {0,0,0,nullptr} ;
	agentWpt lastWpt =  {0,0,0,nullptr} ;	
	
	//Armazena o modo de voo automático que está selecionado
	UAVAutoMode autoMode{ UAVAutoMode::NONE };
	
	//Armazena a fase modo de voo automático que está ativa
	int autoModeStatus{ 0 };
	
	//Controla necessidade de se buscar na simulação novos elementos de cenário
	bool newScene = true;
	//Identifica a existencia de uma base de solo (ainda sem aplicação) 
	bool haveBase = false;
	
	//Maps que armazenam os ponteiros dos POIs encontrados na simulação
	//em função do tipo de cada um
	std::map <int, const asa::components::POI*> avoidPointMap;
	std::map <int, const asa::components::POI*> wptPointMap;
	std::map <int, const asa::components::POI*> endPointMap;
	std::map <int, const asa::components::POI*> automodePointMap;

	void setPOIs();

protected:
    processor::ExternalProcessor* processor{nullptr};
	void updateNewScene();

private:
    bool setSlotExternalProcessor(processor::ExternalProcessor* const msg);
};

} // namespace ubf
} // namespace asa
#endif
