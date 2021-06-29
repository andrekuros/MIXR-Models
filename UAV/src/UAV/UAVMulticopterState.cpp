#include "asacomponents/include/UAV/UAVMulticopterState.h"
#include "asacomponents/include/UAV/UAVMulticopterAgent.h"
#include "asacomponents/include/UAV/UAVMulticopter.h"
#include "asacomponents/include/Buildings/UAVBaseStation.h"

#include "mixr/models/navigation/Navigation.hpp"
#include "mixr/models/navigation/Route.hpp"
#include "mixr/models/navigation/Steerpoint.hpp"

#include "mixr/models/WorldModel.hpp"
#include "mixr/simulation/Station.hpp"
#include "mixr/simulation/Simulation.hpp"
#include "mixr/base/PairStream.hpp"

namespace asa {
namespace ubf {

IMPLEMENT_SUBCLASS(UAVMulticopterState, "UAVMulticopterState")
EMPTY_COPYDATA(UAVMulticopterState)
EMPTY_SLOTTABLE(UAVMulticopterState)

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
UAVMulticopterState::UAVMulticopterState() { STANDARD_CONSTRUCTOR() }


//------------------------------------------------------------------------------
// deleteData
//------------------------------------------------------------------------------
void UAVMulticopterState::deleteData()
{
	BaseClass::deleteData();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void UAVMulticopterState::reset()
{
	BaseClass::reset();
}

//------------------------------------------------------------------------------
// updateState
//------------------------------------------------------------------------------
void UAVMulticopterState::updateState(const mixr::base::Component* const actor)
{

	this->actor = actor;
	BaseClass::updateState(actor);	
	hasActor = true;

	// Get and cast required pointers
	const UAVMulticopterAgent* agent = getAgent();
	if (agent == nullptr) return;
	
	const asa::components::UAVMulticopter* uav{ dynamic_cast<const asa::components::UAVMulticopter*>(actor) };
	if (uav == nullptr) return;
	
	pos = uav->getPosition();
	hdg = uav->getHeading(); //Heading in Rads		
	
	//Navigation System State
	/*const mixr::base::Pair* pairNav{ uav->findByType(typeid(mixr::models::Navigation)) };
	if (pairNav != nullptr){
		const mixr::models::Navigation* nav = static_cast<const mixr::models::Navigation*>(pairNav->object());
		
		if (nav != nullptr){
			const mixr::models::Route* route = nav->getPriRoute();
			
			if (route != nullptr)
			{									
				nextWpt = route->getStptIndex(); 
				steerPointPos = route->getSteerpoint()->getPosition();
			}
		}
	}*/
	
}

//Método que retorna um custo tendo em vista todos os pontos "AVOID" que estiverem
//a uma distância menor que seus raios
const double UAVMulticopterState::calculateAvoidCost(double hdgFwd, double dist) const
{
	//Identifica o Agente necessário para a execução do comportamento
	const UAVMulticopterAgent* agent = getAgent();
	if (agent == nullptr) return 0;
	
	//Caso não exista nenhum POI "AVOID" o custo é zero
	if (agent->avoidPointMap.size() == 0) return 0;
			
	double costAvoid = 0;
	double dx = cos(hdgFwd) * dist; //Projeta ponto a ser avaliado no eixo X
	double dy = sin(hdgFwd) * dist; //Projeta ponto a ser avaliado no eixo Y
				
	//Para cada POI "AVOID" é adicionado um custo 
	for (const auto& avoidPOI : agent->avoidPointMap)
	{
		//Posição POI
		mixr::base::Vec3d posAv = avoidPOI.second->getPosition();
			
		//Calcula distâncias para o pponto
		double dist_x = pos.x() + dx - posAv.x();
		double dist_y = pos.y() + dy - posAv.y();
		double gndRng = std::sqrt(dist_x*dist_x + dist_y*dist_y);
		
		//Caso a distância seja menor que o raio definido para o ponto
		if (gndRng < avoidPOI.second->radius)
		{				
			//Custo é calculado como raio * log(raio / distância_do_ponto), deste modo sempre
			//será positivo sendo 0 no raio e crescendo logaritmicamente para o centro
			costAvoid = costAvoid + avoidPOI.second->radius * log(avoidPOI.second->radius / gndRng);
		}					
	}				
	return costAvoid;
}

//Método que retorna um custo tendo em vista o ponto definido como próximo objetivo 
//para a navegação do drone
const double UAVMulticopterState::calculateRouteCost(double hdgFwd, double dist, double offX, double offY) const
{
	//Identifica o Agente necessário para a execução do comportamento
	const UAVMulticopterAgent* agent = getAgent();
	if (agent == nullptr) return 0;

	
	//Caso nenhum ponto esteja definido para ser alcançado é returnado zero para
	//manter a proa atual e custos maiores para proas que mudariam o curso. 
	//Neste caso, se rota definida o drone mantém a mesma direção
	if (agent->nextWpt.wptId == 0)
		return (abs(hdg - hdgFwd)) ;
		
	double costRoute = 0;
	double dx = cos(hdgFwd) * dist; //Projeta ponto a ser avaliado no eixo X
	double dy = sin(hdgFwd) * dist; //Projeta ponto a ser avaliado no eixo Y
	 	
	//Cálculo das distâcias para o ponto de destino, considerando a posição atual
	//e considerando a possível posição futura
	double dist_x = pos.x() - (agent->nextWpt.POI->getXPosition() + offX);
	double dist_y = pos.y() - (agent->nextWpt.POI->getYPosition() + offY);				
	double gndRngNew     = std::sqrt(pow(dist_x+dx,2) + pow(dist_y+dy,2));
	double gndRngCurrent = std::sqrt(pow(dist_x   ,2) + pow(dist_y   ,2));
	
	//O custo é a proporção entre a distância atual e a projetada, caso a projetada
	//se afaste da rota ideal o custo sobre. Portanto é criado um campo que 
	//dierciona a aeronave para o ponto de interesse.
	costRoute = gndRngNew / gndRngCurrent ;
				
	return costRoute;
}

const UAVMulticopterAgent* UAVMulticopterState::getAgent() const
{
	const UAVMulticopterAgent* asaAgent{};

	if (hasActor)
	{
		const mixr::base::Pair* pairAgent{ actor->findByType(typeid(UAVMulticopterAgent)) };
		if (pairAgent != nullptr)
		{
			const UAVMulticopterAgent* agent{ static_cast<const UAVMulticopterAgent*>(pairAgent->object()) };
			if (agent != nullptr)
			{
				asaAgent = agent;

				if (asaAgent != nullptr)
					asaAgent->ref();
			}
		}
	}

	return asaAgent;
}
} // namespace ubf
} // namespace asa