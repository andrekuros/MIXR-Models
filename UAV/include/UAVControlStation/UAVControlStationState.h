#ifndef __asa_ubf_UAVControlState_H__
#define __asa_ubf_UAVControlState_H__

#include "mixr/base/ubf/AbstractState.hpp"


//------------------------------------------------------------------------------
// Class: UAVControlStationState
//
// Description:
//------------------------------------------------------------------------------

namespace asa {
namespace ubf {


class UAVControlStationAgent;

class UAVControlStationState : public mixr::base::ubf::AbstractState
{
    DECLARE_SUBCLASS(UAVControlStationState, mixr::base::ubf::AbstractState)

public:
    UAVControlStationState();
  
    void updateState(const mixr::base::Component* const actor) override;
	void reset() override;
	
	UAVControlStationAgent* UAVControlStationState::getAgent();
	const UAVControlStationAgent* UAVControlStationState::getAgent() const;
		
protected:
    bool hasActor{};
    mixr::base::Component* actor{};
};

} // namespace ubf
} // namespace asa
#endif
