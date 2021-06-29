#include "asacomponents/include/UAV/UAVMulticopterAction.h"
#include "asacomponents/include/UAV/UAVMulticopterState.h"

#include "mixr/models/player/air/AirVehicle.hpp"
#include "mixr/models/system/Autopilot.hpp"

#include "mixr/models/navigation/Navigation.hpp"
#include "mixr/models/navigation/Route.hpp"

namespace asa {
namespace ubf {
IMPLEMENT_SUBCLASS(UAVMulticopterAction, "UAVMulticopterAction")
EMPTY_COPYDATA(UAVMulticopterAction)
EMPTY_DELETEDATA(UAVMulticopterAction)
EMPTY_SLOTTABLE(UAVMulticopterAction)

UAVMulticopterAction::UAVMulticopterAction()
{
	STANDARD_CONSTRUCTOR()	
		
}	
		
//------------------------------------------------------------------------------
// execute
//------------------------------------------------------------------------------
bool UAVMulticopterAction::execute(mixr::base::Component* actor)
{
	mixr::models::AirVehicle* av{ dynamic_cast<mixr::models::AirVehicle*>(actor) };
			
	if (av != nullptr)
	{
		if (isAutopilotActionFlag)
			executeAutopilotAction(av);	

		if (isNavigationActionFlag)
			executeNavigationAction(av);

		if (isAutoModeActionFlag)
			executeAutoModeAction(av);
	}

	// Let AsaAirVehicleAction take care of autopilot
	return true;
}

		
//------------------------------------------------------------------------------
// Navigation action
//------------------------------------------------------------------------------
void UAVMulticopterAction::executeNavigationAction(mixr::models::AirVehicle* av)
{			
	if (av != nullptr)
	{
		mixr::base::Pair* pairNav{ av->findByType(typeid(mixr::models::Navigation)) };
		if (pairNav != nullptr)
		{
			mixr::models::Navigation* nav = static_cast<mixr::models::Navigation*>(pairNav->object());
			if (nav != nullptr)
			{
				mixr::models::Route* route = nav->getPriRoute();

				if (route != nullptr)
				{
					route->directTo(changeToWpt);
				}
			}
		}
	}		
}


//------------------------------------------------------------------------------
// AutoMode action
//------------------------------------------------------------------------------
void UAVMulticopterAction::executeAutoModeAction(mixr::models::AirVehicle* av)
{	
	if (av != nullptr)
	{	
		mixr::base::Pair* pairAgent{ av->findByType(typeid(UAVMulticopterAgent)) };
		if (pairAgent != nullptr)
		{			
			UAVMulticopterAgent* agent{ static_cast<UAVMulticopterAgent*>(pairAgent->object())};
			if (agent != nullptr)
			{												
				agent->nextWpt = wptNext;
				agent->lastWpt = wptLast;
				agent->autoMode = autoMode;
				agent->autoModeStatus = autoModeStatus;	
				agent->setPOIs();
				
			}
		}
	}
}

//------------------------------------------------------------------------------
// Autopilot action
//------------------------------------------------------------------------------
void UAVMulticopterAction::executeAutopilotAction(mixr::models::AirVehicle* airVehicle)
{
	mixr::models::Autopilot* ap{ dynamic_cast<mixr::models::Autopilot*>(airVehicle->getPilot()) };			

	if (ap != nullptr)
	{
		ap->setPowerSwitch(mixr::models::System::PWR_ON);
		ap->setMaxBankAngleDeg(ap->getMaxBankAngle());				

		switch (apMode)
		{

		case AutopilotMode::NAV:
		{
			ap->setNavMode(true);
			ap->setFollowTheLeadMode(false);
			ap->setFlyToMode(false);
			ap->setAttitudeMode(false);
			ap->setFlyCourseMode(false);

			break;
		}
		case AutopilotMode::FLYTO:
		{
			ap->setNavMode(false);
			ap->setFollowTheLeadMode(false);
			ap->setLoiterMode(false);
			ap->setAttitudeMode(false);
			ap->setFlyCourseMode(false);

			ap->setFlyToMode(true);
			ap->setFlyToLat(apFlyLatitudeDeg);
			ap->setFlyToLon(apFlyLongitudeDeg);
			ap->setFlyToAltitudeFt(apFlyAltitudeFt);
			ap->setFlyToAirspeedKts(apFlySpeedKts);

			break;
		}

		case AutopilotMode::HDGALTSPD:
		{					
			ap->setNavMode(false);
			ap->setFollowTheLeadMode(false);
			ap->setFlyToMode(false);
			ap->setAttitudeMode(false);
			ap->setFlyCourseMode(false);

			ap->setHeadingHoldMode(true);
			ap->setCommandedHeadingD(apFlyHeadingDeg);					

			ap->setAltitudeHoldMode(true);
			ap->setCommandedAltitudeFt(apFlyAltitudeFt);

			ap->setVelocityHoldMode(true);
			ap->setCommandedVelocityKts(apFlySpeedKts);

			break;
		}

		case AutopilotMode::FLYCRS:
		{
			ap->setNavMode(false);
			ap->setFollowTheLeadMode(false);
			ap->setFlyToMode(false);
			ap->setAttitudeMode(false);

			ap->setFlyCourseMode(true);

			ap->setFlyCourseLat(apFlyLatitudeDeg);
			ap->setFlyCourseLon(apFlyLongitudeDeg);
			ap->setFlyCourseCrsDeg(apFlyHeadingDeg);
			ap->setFlyCourseAltitudeFt(apFlyAltitudeFt);
			ap->setFlyCourseAirspeedKts(apFlySpeedKts);

			break;
		}

		default:
		{
			ap->setNavMode(false);
			ap->setLoiterMode(false);
			ap->setFollowTheLeadMode(false);
			ap->setFlyToMode(false);
			ap->setAttitudeMode(false);
			ap->setFlyCourseMode(false);

			ap->setHeadingHoldMode(false);
			ap->setAltitudeHoldMode(false);
			ap->setVelocityHoldMode(false);
			ap->setAttitudeHoldMode(false);

			ap->setPowerSwitch(mixr::models::System::PWR_OFF);

			break;
		}
		}						
	}
}		

//------------------------------------------------------------------------------
// Copy Functions
//------------------------------------------------------------------------------

void UAVMulticopterAction::copyAutopilotActionFrom(UAVMulticopterAction* const src)
{
	setAutopilotActionFlag(src->getAutopilotActionFlag());
	setApFlyLatitudeDeg(src->getApFlyLatitudeDeg());
	setApFlyLongitudeDeg(src->getApFlyLongitudeDeg());
	setApFlyAltitudeFt(src->getApFlyAltitudeFt());
	setApFlySpeedKts(src->getApFlySpeedKts());
	setApFlyHeadingDeg(src->getApFlyHeadingDeg());
	setApMaxBankAngleDeg(src->getApMaxBankAngleDeg());
	setApMode(src->getApMode());
}

void UAVMulticopterAction::copyNavigationActionFrom(UAVMulticopterAction* const src)
{
	setNavigationActionFlag(src->getNavigationActionFlag());
	setChangeToWpt(src->getChangeToWpt());
}

void UAVMulticopterAction::copyAutoModeActionFrom(UAVMulticopterAction* const src)
{
	setAutoModeActionFlag(src->getAutoModeActionFlag());
	setAutoMode(src->getAutoMode());
	setAutoModeNextWpt(src->getAutoModeNextWpt());
	setAutoModeLastWpt(src->getAutoModeLastWpt());	
	setAutoModeStatus(src->getAutoModeStatus());
}
}
}