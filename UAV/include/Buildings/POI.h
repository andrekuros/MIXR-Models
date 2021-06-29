#ifndef __mixr_models_POI_H__
#define __mixr_models_POI_H__

#include "asacomponents/include/UAV/UAVMulticopter.h"

#include "mixr/models/player/Player.hpp"
#include "mixr/base/List.hpp"
#include "mixr/base/String.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Number.hpp"

namespace asa {
namespace components {

//------------------------------------------------------------------------------
// Class: POI
// Factory name: POI
//------------------------------------------------------------------------------

//Definição dos possíveis tipos de POI	
typedef enum {	AVOID, WPT,	ENDPOINT, AUTOMODE,} POI_Type;

class POI : public mixr::models::Player
{
    DECLARE_SUBCLASS(POI, mixr::models::Player)

public:
    POI();
	void initData();	
	
	//Variável que define o raio de interesse. 
	//Em AVOID é a distância que ele passa a ter efeito
	//Em AUTOMODE esse raio presenta o raio das circunferencias a serem contornadas
	double radius{ 500 }; //(meters)
	
	//Variável que define o tipo de POI
	POI_Type type{ POI_Type::WPT};
	
	//Identificação do POI, por exemplo, em AUTOMODE ele busca pontos com ID 1 e 2 
	//para adotar como referência
	unsigned int poiId{ 0 };

private:
	bool setSlotRadius(const mixr::base::Number* const msg);
	bool setSlotType(const mixr::base::String* const msg);
	bool setSlotPoiID(const mixr::base::Number* const msg);
	
};

} // namespace models
} // namespace mixr

#endif
