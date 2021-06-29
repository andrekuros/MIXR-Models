#ifndef __asa_ubf_AsaAgentState_H__
#define __asa_ubf_AsaAgentState_H__

#include "mixr/base/ubf/AbstractState.hpp"
#include <list>
#include <map>


#include "asacomponents/include/Buildings/POI.h"

//------------------------------------------------------------------------------
// Class: UAVMulticopterState
//
// Description:
//------------------------------------------------------------------------------

namespace asa {
namespace ubf {

typedef enum
{
	INFLIGHT,
	WAITING,
	RECHARGING,
	OUTOFSERVICE,
} UAVStatus;


typedef enum
{
	NONE,
	OVAL,
	EIGHT,
	STAYAT,
	WAYPOINT,
	SCAN,
	ROUTE,
} UAVAutoMode;

class UAVMulticopterAgent;

class UAVMulticopterState : public mixr::base::ubf::AbstractState
{
    DECLARE_SUBCLASS(UAVMulticopterState, mixr::base::ubf::AbstractState)

public:
    UAVMulticopterState();
  
    void updateState(const mixr::base::Component* const actor) override;
	void reset() override;
		
	const UAVMulticopterAgent* UAVMulticopterState::getAgent() const;
		
	//M�todo que retorna o custo total (campo gravitacional) em fun��o de todos 
	//os POIs definidos como AVOID que estejam a uma dist�ncia menor que seu raio 
	//hdgFwd -> proa que se pretende avaliar de -pi a pi em [rad]
	//dist   -> distancia da posi��o atual a ser avaliada [metros]
	const double calculateAvoidCost(double hdgFwd, double dist) const;
	
	//M�todo que calcula o custo em fun��o da mudan�a de proa em fun��o da rota 
	//que est� sendo seguida, visando "atrair" a aeronave para manter sua rota
	//hdgFwd -> proa que se pretende avaliar de -pi a pi em [rad]
	//dist   -> distancia da posi��o atual a ser avaliada [metros]
	//offSetX e offSetY -> permiter considerar um ponto deslocado em dx e dy [m]
	//do ponto de interesse
	const double calculateRouteCost(double hdgFwd, double dist, double offX = 0, double offY = 0) const;
	
	//Define o status atual do drone conforme UAVStatus
	UAVStatus uavStatus = UAVStatus::INFLIGHT;
	
	//Armazena posi��o atual do drone na simula��o em coordenadas locais
	mixr::base::Vec3d pos; //[NED]
	
	//Armazena proa atual em radianos do drone
	double hdg{ 0 }; //[rad]


protected:
    bool hasActor{};
    const mixr::base::Component* actor{};
};

} // namespace ubf
} // namespace asa
#endif
