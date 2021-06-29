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

	//Vari�veis que armazenam o deslocamento em rela��o ao ponto de refer�ncia que 
	//est� em uso, por exemplo para fazer uma entrada em �rbita em um raio espec�fico
	double offX, offY{ 0 }; // [meters]
	
	int lapsCount{ -1 }; //Conta n�meros de �rbitas ap�s entrar em modo autom�tico
	
	bool turnSignalDecision{ true }; //auxiliar para inverter o lado da �rbita conforme necessidade
	
	int turnSignal{ -1 }; //define o lado da curva para manter uma �bita -1 (esq) / 1 (dir)
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

	//Define o mode atum�tico desejado 
	UAVAutoMode selectedMode{ UAVAutoMode::NONE };

	//Controla o n�mero de �rbitas j� realizadas
	int lapsCount{ 0 };
		

private:
	bool setSlotMovement(const mixr::base::String* const msg);
};

} // namespace ubf
} // namespace asa
#endif