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
			
	UAVMulticopterAction* action{}; //Cria nova a��o que ser� gerada como resultado do comportamento

	//Identifica o estado relativo ao drone
	const UAVMulticopterState* uavState{ dynamic_cast<const UAVMulticopterState*>(state) };
	if (uavState == nullptr) return action;

	//Identifica o Agente necess�rio para a execu��o do comportamentno
	const UAVMulticopterAgent* agent = uavState->getAgent();
	if (agent == nullptr) return action;


	//Define os parametros gerais para que a ac��o que aua no piloto autm�tico
	//possa ser executada adequadamente
	action = new UAVMulticopterAction();
	action->setAutopilotActionFlag(true); //define como a��o de PA
	//Define o modo para manuten��o de Proa, Dire��o, Altitude e Velocidade
	action->setApMode(asa::ubf::AutopilotMode::HDGALTSPD); 
	action->setApFlyAltitudeFt(2500); //Define n�vel de voo em 2500ft				
	action->setApMaxBankAngleDeg(60); //define m�xima inclina��o em curvas para 60 graus								
	action->setApFlySpeedKts(120); //define velocidade de voo para 120Kts

	//Neste trecho, baseando nas informa��es dos Status no modo de voo autom�tico, 
	//� definido o offSet em rela��o ao POI necess�rio a ser considerado para que o 
    //perfil desejado seja cumprido. O ponto com offSet representa um m�nimo no
	//campo gravitacional, portanto � para onde o drone dever� ir
	
	//S� atua se est� em algum dos mdos j� implementados
	if (agent->autoMode == UAVAutoMode::STAYAT || 
		agent->autoMode == UAVAutoMode::EIGHT  || 
		agent->autoMode == UAVAutoMode::OVAL)
	{
		
		//Caso esteja iniciando um ciclo 
		if (agent->autoModeStatus == 1)
		{
			//A variav�l turnSignalDecision indica que � necess�rio decidir o
			//lado da �rbita para a pr�xima fase
			turnSignalDecision = true;			
			lapsCount++; //conta ciclos
		}
		
		//Caso enteja navegando para interceptar uma �rbita
		if (agent->autoModeStatus == 2)
		{
			//OffSets s�o os j� definidos pois aproa-se os pontos de entrada 
			offX = agent->nextWpt.offSetX;
			offY = agent->nextWpt.offSetY;
		}

		//Caso esteja voando em uma �rbita o offSet deve ser para um ponto futuro 
		//na pr�pria �rbita
		if (agent->autoModeStatus == 3)
		{
			//Proa para o ponto de referencia original
			double hdgToWpt = atan2(agent->nextWpt.POI->getYPosition() - uavState->pos.y(), 
				                    agent->nextWpt.POI->getXPosition() - uavState->pos.x());
			
			//Proa para o ponto de refer�ncia com o Offset de 90 graus
			double hdgToOffWpt = 
				atan2(agent->nextWpt.POI->getYPosition() - uavState->pos.y() + agent->nextWpt.offSetX, 
					  agent->nextWpt.POI->getXPosition() - uavState->pos.x() + agent->nextWpt.offSetY);

			//Caso esteja chegando na �rbita o sentido de rota��o � definido			
			if (turnSignalDecision)
			{
				//Caso seja Eight cada �rbita � percorrida em um sentido diferente e 
				//turnSignal deve ser invertido
				turnSignal = turnSignal * agent->autoMode == UAVAutoMode::EIGHT ? -1 : 1;
				turnSignalDecision = false;	//para impedir midar nos pr�ximos frames			
			}

			//O Offset � calculado para 10 graus � frente da posi��o do drone na �rbita
			//de modo a ser mantido a rota sobre o raio n sentido desejado(turnSignal)
			offX = cos(hdgToWpt + turnSignal * 170 * mixr::base::angle::D2RCC) * agent->nextWpt.POI->radius;
			offY = sin(hdgToWpt + turnSignal * 170 * mixr::base::angle::D2RCC) * agent->nextWpt.POI->radius;
		}
	}

	
	//Neste segundo Trecho, usando o offset calculado para definir o 
	//novo ponto que deve ser consirado como objetivo, busca-se poss�veis
	//posi��es futuras para se adotar aquele com menor custo. Ou seja 
	//que leve para a posi��o desejada e evite obst�culos

	double hdg, bestHdg; //proa a ser pesquisava e a melhor at� ent�o
	double bestHdgCost = 99999999; //para armazenar menor custo encontra
	//angulo de abertura para fazer a busca (neste caso 30 para cada lado)
	double gimbal = 30 * mixr::base::angle::D2RCC;
	//resolu��o da busca (neste caso de 2 em 2 gruas)
	double gimbalStep = 2 * mixr::base::angle::D2RCC; //2 degrees step
	//Distancia [metros] deste ponto de busca, neste caso foi considerado 
	//10s ( 120Kts/3600s * 10 * covers�o Milhas para KM)
	double fwdRange = 120.0 / 3600.0 * 10.0 * mixr::base::distance::NM2M;//[meters]

	//Neste Loop avalia-se todos os pontos definidos 
	for (hdg = -gimbal + uavState->hdg; hdg <= gimbal + uavState->hdg; hdg = hdg + gimbalStep)
	{
		//Solicita ao State o custo em rela��o a "AVOID" points (+pr�x ponto >)
		double costAvoids = uavState->calculateAvoidCost(hdg, fwdRange);						
			
		//Solicita ao State o custo relativo � rota desejada (+fora da rota >)
		double costChangeHdg = uavState->calculateRouteCost(hdg, fwdRange, offX, offY);
		
		//Atualiza melhor proa caso menor custo seja encontrado
		if (costAvoids + costChangeHdg < bestHdgCost)
		{
			bestHdg = hdg;
			bestHdgCost = costAvoids + costChangeHdg;
		}		
	}				
	//Por fim define o par�metro de proa a ser seguida pelo piloto
	//Autom�tico em fun��o da melhor dire��o encontrada
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
			
	UAVMulticopterAction* action{}; //Cria nova a��o que ser� gerada como resultado do comportamento
	
	//Identifica o estado relativo ao drone
	const UAVMulticopterState* uavState{ dynamic_cast<const UAVMulticopterState*>(state) };
	if (uavState == nullptr) return action;

	//Identifica o Agente necess�rio para a execu��o do comportamentno
	const UAVMulticopterAgent* agent = uavState->getAgent();
	if (agent == nullptr) return action;
												
	//Condi��o para apenas prosseguir se est� em um dos modos j� implementados
	if (selectedMode == UAVAutoMode::STAYAT || 
		selectedMode == UAVAutoMode::EIGHT  || 
		selectedMode == UAVAutoMode::OVAL     
	   )
	{		
		//Inicializa��o do AutoMode. Ajusta o Agente para iniciar o ciclo do modo desejado
		if (agent->autoModeStatus == 0)
		{																					
			//Define pontos de refer�ncia os que tiverem os IDs 1 e 2
			agentWpt nextWpt = { 1,0,0,nullptr };
			agentWpt lastWpt = { 2,0,0,nullptr };

			//Preenche objeto action para que seja processado ao final
			action = new UAVMulticopterAction();
			action->setAutoModeActionFlag(true);//Define que � umaa a��o de mode de voo
			action->setAutoMode(selectedMode); //define qual o modo
			action->setAutoModeNextWpt(nextWpt);//ajusta par�metros iniciais do primeiro ponto
			action->setAutoModeLastWpt(lastWpt);//ajusta par�metros iniciais do segundo ponto
			action->setAutoModeStatus(1); //altera status para pr�xima fase (s� muda no  pr�ximo frame)						
		}		

		//Com status em 1, s�o ajustados os par�metros Iniciais em cada troca de Waypoint
		//no caso de Eight ou Oval, no caso de StayAt n�o h� troca
		else if (agent->autoModeStatus == 1)
		{
			//Inicializa novos pontos para substituir anteriores
			agentWpt nextWpt = { agent->nextWpt.wptId,agent->nextWpt.offSetX,agent->nextWpt.offSetY,agent->nextWpt.POI };
			agentWpt lastWpt = { agent->lastWpt.wptId,agent->lastWpt.offSetX,agent->lastWpt.offSetY,agent->lastWpt.POI };

			double radiusNext = agent->nextWpt.POI->radius; //Raio do pr�ximo POI
			double radiusLast = agent->lastWpt.POI->radius; //Raio do POI anterior
			double hdgPoints;

			//Antes da primeira volta o c�lculo � feito para interceptar a �rbita e
			//o offSet do ponto de refe�ncia � diferente
			if (lapsCount == 0)
			{
				//C�lculo da Proa entre a posi��o atual e o pr�ximo POI visando 
				//encontrar o ponto de intercpta��o
				double hdgI = atan2( agent->nextWpt.POI->getYPosition() - uavState->pos.y(),
									 agent->nextWpt.POI->getXPosition() - uavState->pos.x());
				//Proa � rebatida 90 graus para intecpta��o, no raio, em ponto perpendicular
				//a proa atual. 
			    //Obs: a fun��o mixr::base::angle::aepcdRad mant�m proa entre pi e -pi
				hdgI= mixr::base::angle::aepcdRad(hdgI + 90 * mixr::base::angle::D2RCC);
										
				//C�lculos para encontrar o OffSet necess�rio para que seja identificado
				//o ponto de interesse. Neste caso a intecepta��o com o raio na radial 
				//90 graus defasada da proa de aproxima��o				
				nextWpt.offSetX = cos(hdgI) * radiusNext; //Proje��o em X
				nextWpt.offSetY = sin(hdgI) * radiusNext;// Proje��o em Y

				//Esta vari�vel modifica a proje��o do offset no ponto de refer�ncia 2
				//sendo que para o Eight ambas ficam para o mesmo lado e Oval s�o alternadas
				int offSginal = (selectedMode == UAVAutoMode::EIGHT)? 1 : -1 ;
								
				//C�lculo da Proa entre a os POIs de interesse visando 
				//encontrar os ponto de proje��o a 90 graus
				double hdgPoints = atan2(agent->nextWpt.POI->getYPosition() - agent->lastWpt.POI->getYPosition(),
									 	 agent->nextWpt.POI->getXPosition() - agent->lastWpt.POI->getXPosition());	
				hdgPoints = mixr::base::angle::aepcdRad(hdgPoints + offSginal * 90 * mixr::base::angle::D2RCC);

				lastWpt.offSetX = cos(hdgPoints) * radiusLast;
				lastWpt.offSetY = sin(hdgPoints) * radiusLast;

			}
			//Ao entrar em �rbita pela primeira vez a proje��o do POI inicial � ajustada para 90 em
			//rela��o aos dois. Inicialmente estava em fun��o da aproxima��o inicial
			else if (lapsCount == 1)
			{						

				double hdgPoints = atan2(agent->nextWpt.POI->getYPosition() - agent->lastWpt.POI->getYPosition(),
										 agent->nextWpt.POI->getXPosition() - agent->lastWpt.POI->getXPosition());
				hdgPoints = mixr::base::angle::aepcdRad(hdgPoints - 90 * mixr::base::angle::D2RCC);

				lastWpt.offSetX = cos(hdgPoints) * radiusLast;
				lastWpt.offSetY = sin(hdgPoints) * radiusLast;

			}		
			lapsCount++;
				
			//A a��o � preenchida para enviar as atualiza��es necess�rias que ir�o
			//Modificar o Agent passando para status 2
			action = new UAVMulticopterAction();
			action->setAutoModeActionFlag(true);
			action->setAutoMode(selectedMode);
			action->setAutoModeNextWpt(nextWpt);
			action->setAutoModeLastWpt(lastWpt);
			action->setAutoModeStatus(2);			
		}
		
		//Com Status em 2 o drone est� navegando para entrada na circunfefencia
		//ou entre as duas se for Eight ou Oval
		else if (agent->autoModeStatus == 2)
		{
							
			//C�lculo da dist�ncia para o raio da �rbita desejada, considerando ponto de
			//intercept���o desejado (com offSet)
			double radius = agent->nextWpt.POI->radius;			
			double dist_x = agent->nextWpt.POI->getXPosition()+ agent->nextWpt.offSetX - uavState->pos.x();
			double dist_y = agent->nextWpt.POI->getYPosition()+ agent->nextWpt.offSetY - uavState->pos.y();
			double gndRng = std::sqrt(dist_x*dist_x + dist_y*dist_y);
			
			//Definiu-se que a 10% do raio pode-se considerar que o ponto
			//foi interceptado
			if (gndRng < radius * 0.1 )
			{
								
				//A a��o � preenchida para enviar as atualiza��es necess�rias que ir�o
			    //Modificar o Agent passando para Status 3
				action = new UAVMulticopterAction();
				action->setAutoModeActionFlag(true);
				action->setAutoMode(selectedMode);				
				action->setAutoModeNextWpt(agent->nextWpt);
				action->setAutoModeLastWpt(agent->lastWpt);
				action->setAutoModeStatus(3);				
			}
		}

		//Em autoModeStatus == 3 a aeronave se mant�m em �rbita sobre o raio definido
		//Caso esteja em Eight ou OVAL, monitora o momento troca de �rbitas
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
					

			//Crit�rio para abondonar para outra circunfer�ncia caso a aeronave esteja 
			//coma proa +-0.015 radianos da proa ideal para o pr�ximo ponto de interesse
			if ( abs(mixr::base::angle::aepcdRad(hdgToWptOff - uavState->hdg)) < 0.015)
			{
				//Nova a��o � criada, atualizando o status para reiniciar invertendo
				//os pontos de refer�ncia
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