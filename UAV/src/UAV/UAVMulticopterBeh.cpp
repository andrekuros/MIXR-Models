#include "asacomponents/include/UAV/UAVMulticopterBeh.h"
#include "asacomponents/include/UAV/UAVMulticopterAction.h"
#include "asacomponents/include/Buildings/UAVBaseStation.h"

#include "mixr/models/player/air/AirVehicle.hpp"
#include "mixr/models/system/Autopilot.hpp"



//------------------------------------------------------------------------------
// UAV Navigation Behaviour
//------------------------------------------------------------------------------
namespace asa {
namespace ubf {

IMPLEMENT_SUBCLASS(UAVNavigation, "UAVNavigation")
EMPTY_SLOTTABLE(UAVNavigation)
EMPTY_COPYDATA(UAVNavigation)
EMPTY_DELETEDATA(UAVNavigation)

UAVNavigation::UAVNavigation()
{
	STANDARD_CONSTRUCTOR()
}

mixr::base::ubf::AbstractAction* UAVNavigation::genAction(const mixr::base::ubf::AbstractState* const state, const double dt)
{
			
	UAVMulticopterAction* action{}; //Cria nova ação que será gerada como resultado do comportamento

	//Identifica o estado relativo ao drone
	const UAVMulticopterState* uavState{ dynamic_cast<const UAVMulticopterState*>(state) };
	if (uavState == nullptr) return action;

	//Identifica o Agente necessário para a execução do comportamentno
	const UAVMulticopterAgent* agent = uavState->getAgent();
	if (agent == nullptr) return action;


	//Define os parametros gerais para que a acção que aua no piloto autmático
	//possa ser executada adequadamente
	action = new UAVMulticopterAction();
	action->setAutopilotActionFlag(true); //define como ação de PA
	//Define o modo para manutenção de Proa, Direção, Altitude e Velocidade
	action->setApMode(asa::ubf::AutopilotMode::HDGALTSPD); 
	action->setApFlyAltitudeFt(2500); //Define nível de voo em 2500ft				
	action->setApMaxBankAngleDeg(60); //define máxima inclinação em curvas para 60 graus								
	action->setApFlySpeedKts(120); //define velocidade de voo para 120Kts

	//Neste trecho, baseando nas informações dos Status no modo de voo automático, 
	//é definido o offSet em relação ao POI necessário a ser considerado para que o 
    //perfil desejado seja cumprido. O ponto com offSet representa um mínimo no
	//campo gravitacional, portanto é para onde o drone deverá ir
	
	//Só atua se está em algum dos mdos já implementados
	if (agent->autoMode == UAVAutoMode::STAYAT || 
		agent->autoMode == UAVAutoMode::EIGHT  || 
		agent->autoMode == UAVAutoMode::OVAL)
	{
		
		//Caso esteja iniciando um ciclo 
		if (agent->autoModeStatus == 1)
		{
			//A variavél turnSignalDecision indica que é necessário decidir o
			//lado da órbita para a próxima fase
			turnSignalDecision = true;			
			lapsCount++; //conta ciclos
		}
		
		//Caso enteja navegando para interceptar uma órbita
		if (agent->autoModeStatus == 2)
		{
			//OffSets são os já definidos pois aproa-se os pontos de entrada 
			offX = agent->nextWpt.offSetX;
			offY = agent->nextWpt.offSetY;
		}

		//Caso esteja voando em uma órbita o offSet deve ser para um ponto futuro 
		//na própria órbita
		if (agent->autoModeStatus == 3)
		{
			//Proa para o ponto de referencia original
			double hdgToWpt = atan2(agent->nextWpt.POI->getYPosition() - uavState->pos.y(), 
				                    agent->nextWpt.POI->getXPosition() - uavState->pos.x());
			
			//Proa para o ponto de referência com o Offset de 90 graus
			double hdgToOffWpt = 
				atan2(agent->nextWpt.POI->getYPosition() - uavState->pos.y() + agent->nextWpt.offSetX, 
					  agent->nextWpt.POI->getXPosition() - uavState->pos.x() + agent->nextWpt.offSetY);

			//Caso esteja chegando na órbita o sentido de rotação é definido			
			if (turnSignalDecision)
			{
				//Caso seja Eight cada órbita é percorrida em um sentido diferente e 
				//turnSignal deve ser invertido
				turnSignal = turnSignal * agent->autoMode == UAVAutoMode::EIGHT ? -1 : 1;
				turnSignalDecision = false;	//para impedir midar nos próximos frames			
			}

			//O Offset é calculado para 10 graus á frente da posição do drone na órbita
			//de modo a ser mantido a rota sobre o raio n sentido desejado(turnSignal)
			offX = cos(hdgToWpt + turnSignal * 170 * mixr::base::angle::D2RCC) * agent->nextWpt.POI->radius;
			offY = sin(hdgToWpt + turnSignal * 170 * mixr::base::angle::D2RCC) * agent->nextWpt.POI->radius;
		}
	}

	
	//Neste segundo Trecho, usando o offset calculado para definir o 
	//novo ponto que deve ser consirado como objetivo, busca-se possíveis
	//posições futuras para se adotar aquele com menor custo. Ou seja 
	//que leve para a posição desejada e evite obstáculos

	double hdg, bestHdg; //proa a ser pesquisava e a melhor até então
	double bestHdgCost = 99999999; //para armazenar menor custo encontra
	//angulo de abertura para fazer a busca (neste caso 30 para cada lado)
	double gimbal = 30 * mixr::base::angle::D2RCC;
	//resolução da busca (neste caso de 2 em 2 gruas)
	double gimbalStep = 2 * mixr::base::angle::D2RCC; //2 degrees step
	//Distancia [metros] deste ponto de busca, neste caso foi considerado 
	//10s ( 120Kts/3600s * 10 * coversão Milhas para KM)
	double fwdRange = 120.0 / 3600.0 * 10.0 * mixr::base::distance::NM2M;//[meters]

	//Neste Loop avalia-se todos os pontos definidos 
	for (hdg = -gimbal + uavState->hdg; hdg <= gimbal + uavState->hdg; hdg = hdg + gimbalStep)
	{
		//Solicita ao State o custo em relação a "AVOID" points (+próx ponto >)
		double costAvoids = uavState->calculateAvoidCost(hdg, fwdRange);						
			
		//Solicita ao State o custo relativo á rota desejada (+fora da rota >)
		double costChangeHdg = uavState->calculateRouteCost(hdg, fwdRange, offX, offY);
		
		//Atualiza melhor proa caso menor custo seja encontrado
		if (costAvoids + costChangeHdg < bestHdgCost)
		{
			bestHdg = hdg;
			bestHdgCost = costAvoids + costChangeHdg;
		}		
	}				
	//Por fim define o parâmetro de proa a ser seguida pelo piloto
	//Automático em função da melhor direção encontrada
	action->setApFlyHeadingDeg(bestHdg*mixr::base::angle::R2DCC);				
						
	return action;
}


//------------------------------------------------------------------------------
// UAV Navigation Behaviour
//------------------------------------------------------------------------------
IMPLEMENT_SUBCLASS(UAVAutoMoves, "UAVAutoMoves")

BEGIN_SLOTTABLE(UAVAutoMoves)
"automode",          //1) Ref radius distance to avoid
END_SLOTTABLE(UAVAutoMoves)

BEGIN_SLOT_MAP(UAVAutoMoves)
ON_SLOT(1, setSlotMovement, mixr::base::String)
END_SLOT_MAP()

EMPTY_COPYDATA(UAVAutoMoves)
EMPTY_DELETEDATA(UAVAutoMoves)

UAVAutoMoves::UAVAutoMoves()
{
	STANDARD_CONSTRUCTOR()
}

	
mixr::base::ubf::AbstractAction* UAVAutoMoves::genAction(const mixr::base::ubf::AbstractState* const state, const double dt)
{
			
	UAVMulticopterAction* action{}; //Cria nova ação que será gerada como resultado do comportamento
	
	//Identifica o estado relativo ao drone
	const UAVMulticopterState* uavState{ dynamic_cast<const UAVMulticopterState*>(state) };
	if (uavState == nullptr) return action;

	//Identifica o Agente necessário para a execução do comportamentno
	const UAVMulticopterAgent* agent = uavState->getAgent();
	if (agent == nullptr) return action;
												
	//Condição para apenas prosseguir se está em um dos modos já implementados
	if (selectedMode == UAVAutoMode::STAYAT || 
		selectedMode == UAVAutoMode::EIGHT  || 
		selectedMode == UAVAutoMode::OVAL     
	   )
	{		
		//Inicialização do AutoMode. Ajusta o Agente para iniciar o ciclo do modo desejado
		if (agent->autoModeStatus == 0)
		{																					
			//Define pontos de referência os que tiverem os IDs 1 e 2
			agentWpt nextWpt = { 1,0,0,nullptr };
			agentWpt lastWpt = { 2,0,0,nullptr };

			//Preenche objeto action para que seja processado ao final
			action = new UAVMulticopterAction();
			action->setAutoModeActionFlag(true);//Define que é umaa ação de mode de voo
			action->setAutoMode(selectedMode); //define qual o modo
			action->setAutoModeNextWpt(nextWpt);//ajusta parâmetros iniciais do primeiro ponto
			action->setAutoModeLastWpt(lastWpt);//ajusta parâmetros iniciais do segundo ponto
			action->setAutoModeStatus(1); //altera status para próxima fase (só muda no  próximo frame)						
		}		

		//Com status em 1, são ajustados os parâmetros Iniciais em cada troca de Waypoint
		//no caso de Eight ou Oval, no caso de StayAt não há troca
		else if (agent->autoModeStatus == 1)
		{
			//Inicializa novos pontos para substituir anteriores
			agentWpt nextWpt = { agent->nextWpt.wptId,agent->nextWpt.offSetX,agent->nextWpt.offSetY,agent->nextWpt.POI };
			agentWpt lastWpt = { agent->lastWpt.wptId,agent->lastWpt.offSetX,agent->lastWpt.offSetY,agent->lastWpt.POI };

			double radiusNext = agent->nextWpt.POI->radius; //Raio do próximo POI
			double radiusLast = agent->lastWpt.POI->radius; //Raio do POI anterior
			double hdgPoints;

			//Antes da primeira volta o cálculo é feito para interceptar a órbita e
			//o offSet do ponto de refeência é diferente
			if (lapsCount == 0)
			{
				//Cálculo da Proa entre a posição atual e o próximo POI visando 
				//encontrar o ponto de intercptação
				double hdgI = atan2( agent->nextWpt.POI->getYPosition() - uavState->pos.y(),
									 agent->nextWpt.POI->getXPosition() - uavState->pos.x());
				//Proa é rebatida 90 graus para intecptação, no raio, em ponto perpendicular
				//a proa atual. 
			    //Obs: a função mixr::base::angle::aepcdRad mantém proa entre pi e -pi
				hdgI= mixr::base::angle::aepcdRad(hdgI + 90 * mixr::base::angle::D2RCC);
										
				//Cálculos para encontrar o OffSet necessário para que seja identificado
				//o ponto de interesse. Neste caso a inteceptação com o raio na radial 
				//90 graus defasada da proa de aproximação				
				nextWpt.offSetX = cos(hdgI) * radiusNext; //Projeção em X
				nextWpt.offSetY = sin(hdgI) * radiusNext;// Projeção em Y

				//Esta variável modifica a projeção do offset no ponto de referência 2
				//sendo que para o Eight ambas ficam para o mesmo lado e Oval são alternadas
				int offSginal = (selectedMode == UAVAutoMode::EIGHT)? 1 : -1 ;
								
				//Cálculo da Proa entre a os POIs de interesse visando 
				//encontrar os ponto de projeção a 90 graus
				double hdgPoints = atan2(agent->nextWpt.POI->getYPosition() - agent->lastWpt.POI->getYPosition(),
									 	 agent->nextWpt.POI->getXPosition() - agent->lastWpt.POI->getXPosition());	
				hdgPoints = mixr::base::angle::aepcdRad(hdgPoints + offSginal * 90 * mixr::base::angle::D2RCC);

				lastWpt.offSetX = cos(hdgPoints) * radiusLast;
				lastWpt.offSetY = sin(hdgPoints) * radiusLast;

			}
			//Ao entrar em órbita pela primeira vez a projeção do POI inicial é ajustada para 90 em
			//relação aos dois. Inicialmente estava em função da aproximação inicial
			else if (lapsCount == 1)
			{						

				double hdgPoints = atan2(agent->nextWpt.POI->getYPosition() - agent->lastWpt.POI->getYPosition(),
										 agent->nextWpt.POI->getXPosition() - agent->lastWpt.POI->getXPosition());
				hdgPoints = mixr::base::angle::aepcdRad(hdgPoints - 90 * mixr::base::angle::D2RCC);

				lastWpt.offSetX = cos(hdgPoints) * radiusLast;
				lastWpt.offSetY = sin(hdgPoints) * radiusLast;

			}		
			lapsCount++;
				
			//A ação é preenchida para enviar as atualizações necessárias que irão
			//Modificar o Agent passando para status 2
			action = new UAVMulticopterAction();
			action->setAutoModeActionFlag(true);
			action->setAutoMode(selectedMode);
			action->setAutoModeNextWpt(nextWpt);
			action->setAutoModeLastWpt(lastWpt);
			action->setAutoModeStatus(2);			
		}
		
		//Com Status em 2 o drone está navegando para entrada na circunfefencia
		//ou entre as duas se for Eight ou Oval
		else if (agent->autoModeStatus == 2)
		{
							
			//Cálculo da distância para o raio da órbita desejada, considerando ponto de
			//interceptãção desejado (com offSet)
			double radius = agent->nextWpt.POI->radius;			
			double dist_x = agent->nextWpt.POI->getXPosition()+ agent->nextWpt.offSetX - uavState->pos.x();
			double dist_y = agent->nextWpt.POI->getYPosition()+ agent->nextWpt.offSetY - uavState->pos.y();
			double gndRng = std::sqrt(dist_x*dist_x + dist_y*dist_y);
			
			//Definiu-se que a 10% do raio pode-se considerar que o ponto
			//foi interceptado
			if (gndRng < radius * 0.1 )
			{
								
				//A ação é preenchida para enviar as atualizações necessárias que irão
			    //Modificar o Agent passando para Status 3
				action = new UAVMulticopterAction();
				action->setAutoModeActionFlag(true);
				action->setAutoMode(selectedMode);				
				action->setAutoModeNextWpt(agent->nextWpt);
				action->setAutoModeLastWpt(agent->lastWpt);
				action->setAutoModeStatus(3);				
			}
		}

		//Em autoModeStatus == 3 a aeronave se mantém em órbita sobre o raio definido
		//Caso esteja em Eight ou OVAL, monitora o momento troca de órbitas
		else if (agent->autoModeStatus == 3 && 
				(selectedMode == UAVAutoMode::EIGHT || selectedMode == UAVAutoMode::OVAL))
		{
			
			double radius = agent->lastWpt.POI->radius;									
			//Coordenadas com OffSet
			double wptX = agent->lastWpt.POI->getXPosition() + agent->lastWpt.offSetX;
			double wptY = agent->lastWpt.POI->getYPosition() + agent->lastWpt.offSetY;
			
			//Calculo da proa para o ponte de interesse , considerando o OffSet
			double hdgToWptOff = mixr::base::angle::aepcdRad(
				          atan2(wptY - uavState->pos.y(), wptX - uavState->pos.x()));
					

			//Critério para abondonar para outra circunferência caso a aeronave esteja 
			//coma proa +-0.015 radianos da proa ideal para o próximo ponto de interesse
			if ( abs(mixr::base::angle::aepcdRad(hdgToWptOff - uavState->hdg)) < 0.015)
			{
				//Nova ação é criada, atualizando o status para reiniciar invertendo
				//os pontos de referência
				action = new UAVMulticopterAction();
				action->setAutoModeActionFlag(true);
				action->setAutoMode(selectedMode);
				action->setAutoModeNextWpt(agent->lastWpt);
				action->setAutoModeLastWpt(agent->nextWpt);
				action->setAutoModeStatus(1);			
				
			}
		}
	}			

	return action;
}

bool UAVAutoMoves::setSlotMovement(const mixr::base::String* const msg)
{
	bool ok{};
	if (*msg == "eight" || *msg == "EIGHT") { selectedMode = UAVAutoMode::EIGHT; ok = true; }
	if (*msg == "stayat" || *msg == "STAYAT") { selectedMode = UAVAutoMode::STAYAT; ok = true; }
	if (*msg == "oval" || *msg == "OVAL") { selectedMode = UAVAutoMode::OVAL; ok = true; }
	if (*msg == "scan" || *msg == "SCAN") { selectedMode = UAVAutoMode::SCAN; ok = true; }
	if (*msg == "none" || *msg == "NONE") { selectedMode = UAVAutoMode::NONE; ok = true; }
	return ok;
}
	
}
}