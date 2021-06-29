#ifndef __asa_ubf_UAVMulticopterBeh_H__
#define __asa_ubf_UAVMulticopterBeh_H__

#include "mixr/base/ubf/AbstractBehavior.hpp"
#include "asacomponents/include/UAV/UAVMulticopterState.h"
#include "asacomponents/include/UAV/UAVMulticopterAgent.h"

//------------------------------------------------------------------------------
// Class: UAVMulticopterBeh
//
// Description: base for UAV Behaviours
//------------------------------------------------------------------------------

namespace asa {
namespace ubf {

		
//------------------------------------------------------------------------------
// Class: UAVNavigation
//------------------------------------------------------------------------------
class UAVNavigation : public mixr::base::ubf::AbstractBehavior
{
	DECLARE_SUBCLASS(UAVNavigation, mixr::base::ubf::AbstractBehavior)

public:
	UAVNavigation();

	virtual mixr::base::ubf::AbstractAction* genAction(const mixr::base::ubf::AbstractState* const state, const double dt) override;
	virtual void resetBehavior() {};

	//Variáveis que armazenam o deslocamento em relação ao ponto de referência que 
	//está em uso, por exemplo para fazer uma entrada em órbita em um raio específico
	double offX, offY{ 0 }; // [meters]
	
	int lapsCount{ -1 }; //Conta números de órbitas após entrar em modo automático
	
	bool turnSignalDecision{ true }; //auxiliar para inverter o lado da órbita conforme necessidade
	
	int turnSignal{ -1 }; //define o lado da curva para manter uma óbita -1 (esq) / 1 (dir)
};



//------------------------------------------------------------------------------
// Class: UAVAutoMoves
//------------------------------------------------------------------------------
class UAVAutoMoves : public mixr::base::ubf::AbstractBehavior
{
	DECLARE_SUBCLASS(UAVAutoMoves, mixr::base::ubf::AbstractBehavior)

public:
	UAVAutoMoves();

	virtual mixr::base::ubf::AbstractAction* genAction(const mixr::base::ubf::AbstractState* const state, const double dt) override;
	virtual void resetBehavior() {};

	//Define o mode atumático desejado 
	UAVAutoMode selectedMode{ UAVAutoMode::NONE };

	//Controla o número de órbitas já realizadas
	int lapsCount{ 0 };
		

private:
	bool setSlotMovement(const mixr::base::String* const msg);
};

} // namespace ubf
} // namespace asa
#endif