
#include "../include/factory.hpp"

//#include "mixr/base/Object.hpp"

#include "asacomponents/include/UAV/UAVMavLink.h"
#include "asacomponents/include/UAV/UAVMulticopter.h"
#include "asacomponents/include/UAV/UAVMulticopterAgent.h"
#include "asacomponents/include/UAV/UAVMulticopterState.h"
#include "asacomponents/include/UAV/UAVMulticopterBeh.h"
#include "asacomponents/include/UAV/UAVFuser.h"

#include "asacomponents/include/UAVControlStation/UAVControlStation.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationAgent.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationState.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationBeh.h"
#include "asacomponents/include/UAVControlStation/UAVControlStationAction.h"


#include "asacomponents/include/Buildings/UAVBaseStation.h"
#include "asacomponents/include/Buildings/POI.h"




#include <string>

namespace asa {
    namespace components {

        mixr::base::Object* factory(const std::string& name)
        {
            mixr::base::Object* obj = nullptr;

            //UAV
            if (name == UAVMavLink::getFactoryName())
            {
                obj = new UAVMavLink();
            }

			// UAVMulticopter
            if (name == UAVMulticopter::getFactoryName())
            {
                obj = new UAVMulticopter();
            }

			// UAVMulticopterAgent
			if (name == asa::ubf::UAVMulticopterAgent::getFactoryName())
			{
				obj = new asa::ubf::UAVMulticopterAgent();
			}

			// UAVMulticopterAgent
			if (name == asa::ubf::UAVMulticopterState::getFactoryName())
			{
				obj = new asa::ubf::UAVMulticopterState();
			}

			// UAV Behaviours
			if (name == asa::ubf::UAVAutoMoves::getFactoryName())
			{
				obj = new asa::ubf::UAVAutoMoves();
			}
			
			if (name == asa::ubf::UAVNavigation::getFactoryName())
			{
				obj = new asa::ubf::UAVNavigation();
			}

			// UAV Fuser
			if (name == asa::ubf::UAVFuser::getFactoryName())
			{
				obj = new asa::ubf::UAVFuser();
			}
			
			// UAVControlStation
			if (name == asa::components::UAVControlStation::getFactoryName())
			{
				obj = new asa::components::UAVControlStation();
			}

			// UAVControlStationBeh
			if (name == asa::ubf::UAVControlStationBeh::getFactoryName())
			{
				obj = new asa::ubf::UAVControlStationBeh();
			}

			// UAVControlStationState
			if (name == asa::ubf::UAVControlStationState::getFactoryName())
			{
				obj = new asa::ubf::UAVControlStationState();
			}
			
			// UAVControlStationAgent
			if (name == asa::ubf::UAVControlStationAgent::getFactoryName())
			{
				obj = new asa::ubf::UAVControlStationAgent();
			}

			// UAVControlStationAction
			if (name == asa::ubf::UAVControlStationAction::getFactoryName())
			{
				obj = new asa::ubf::UAVControlStationAction();
			}

			// UAVBaseStation
			if (name == asa::components::UAVBaseStation::getFactoryName())
			{
				obj = new asa::components::UAVBaseStation();
			}



			// POI
			if (name == asa::components::POI::getFactoryName())
			{
				obj = new asa::components::POI();
			}



            return obj;
        }

    }
}
