#ifndef __asa_ubf_UAVMulticopterAction_H__
#define __asa_ubf_UAVMulticopterAction_H__

#include "mixr/base/ubf/AbstractAction.hpp"
#include "asacomponents/include/UAV/UAVMulticopterState.h"
#include "asacomponents/include/UAV/UAVMulticopterAgent.h"

#include "mixr/models/player/air/AirVehicle.hpp"

//------------------------------------------------------------------------------
// Class: UAVMulticopterAction
//
// Description:
//------------------------------------------------------------------------------
namespace asa {
namespace ubf {
//class UAVMulticopterState;

typedef enum {
	FLYTO,
	HDGALTSPD,
	FLYCRS,
	NAV,
} AutopilotMode;

typedef enum {
	CHANGEROUTE,
	CHANGEMODE,
} ActionType;

class UAVMulticopterAction : public mixr::base::ubf::AbstractAction
{
	DECLARE_SUBCLASS(UAVMulticopterAction, mixr::base::ubf::AbstractAction)

public:
	UAVMulticopterAction();												

	//----------------------------------------
	// Autopilot
	//----------------------------------------
	void setAutopilotActionFlag(const bool b) { isAutopilotActionFlag = b; }
	void setApFlyLatitudeDeg(const double lat) { apFlyLatitudeDeg = lat; }
	void setApFlyLongitudeDeg(const double lon) { apFlyLongitudeDeg = lon; }
	void setApFlyAltitudeFt(const double alt) { apFlyAltitudeFt = alt; }
	void setApFlySpeedKts(const double spd) { apFlySpeedKts = spd; }
	void setApFlyHeadingDeg(const double hdg) { apFlyHeadingDeg = hdg; }
	void setApMaxBankAngleDeg(const double bnk) { apMaxBankAngleDeg = bnk; }
	void setApMode(const AutopilotMode m) { apMode = m; }

	bool getAutopilotActionFlag() { return isAutopilotActionFlag; }
	double getApFlyLatitudeDeg() { return apFlyLatitudeDeg; }
	double getApFlyLongitudeDeg() { return apFlyLongitudeDeg; }
	double getApFlyAltitudeFt() { return apFlyAltitudeFt; }
	double getApFlySpeedKts() { return apFlySpeedKts; }
	double getApFlyHeadingDeg() { return apFlyHeadingDeg; }
	double getApMaxBankAngleDeg() { return apMaxBankAngleDeg; }
	AutopilotMode getApMode() { return apMode; }

	// Navigation
	//-----------------------------------------------------
	void setNavigationActionFlag(const bool b) { isNavigationActionFlag = b; }
	bool getNavigationActionFlag() { return isNavigationActionFlag; }
	void setChangeToWpt(unsigned int wpt) { changeToWpt = wpt; }
	unsigned int getChangeToWpt() { return changeToWpt; }

	// AutoMode
	//-----------------------------------------------------
	void setAutoModeActionFlag(const bool b) { isAutoModeActionFlag = b; }
	bool getAutoModeActionFlag() { return isAutoModeActionFlag; }
	void setAutoModeNextWpt(agentWpt wpt) { wptNext = wpt; }
	agentWpt getAutoModeNextWpt() { return wptNext; }
	void setAutoModeLastWpt(agentWpt wpt) { wptLast = wpt; }
	agentWpt getAutoModeLastWpt() { return wptLast; }
	
	void setAutoModeStatus(unsigned int wpt) { autoModeStatus = wpt; }
	unsigned int getAutoModeStatus() { return autoModeStatus; }

	void setAutoMode(UAVAutoMode mode) { autoMode = mode; }
	UAVAutoMode getAutoMode() { return autoMode; }

			
	void UAVMulticopterAction::copyAutopilotActionFrom(UAVMulticopterAction* const src);
	void UAVMulticopterAction::copyNavigationActionFrom(UAVMulticopterAction* const src);
	void UAVMulticopterAction::copyAutoModeActionFrom(UAVMulticopterAction* const src);

	bool execute(mixr::base::Component* actor) override;

private:			
	void executeAutopilotAction(mixr::models::AirVehicle* av);
	void executeNavigationAction(mixr::models::AirVehicle* av);
	void executeAutoModeAction(mixr::models::AirVehicle* av);

private:
			
	// Autopilot Action
	bool isAutopilotActionFlag{false};			
	double apFlyLatitudeDeg{};
	double apFlyLongitudeDeg{};
	double apFlyAltitudeFt{};
	double apFlySpeedKts{};
	double apFlyHeadingDeg{};
	double apMaxBankAngleDeg{};
	AutopilotMode apMode{};

	//Navigation Action
	bool isNavigationActionFlag{false};
	unsigned int changeToWpt;

	//AutoMode Action
	bool isAutoModeActionFlag{false};
	unsigned int autoModeStatus{ 0 };
	agentWpt wptNext = { 0, 0, 0, nullptr };
	agentWpt wptLast = { 0, 0, 0, nullptr };
	UAVAutoMode autoMode{ UAVAutoMode::NONE };
			
	UAVMulticopterState* state;
};

}
}
#endif
