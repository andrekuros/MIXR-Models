#ifndef __asa_ubf_UAVControlStationAgent_H__
#define __asa_ubf_UAVControlStationAgent_H__

#include "mixr/base/ExternalProcessor.hpp"
#include "mixr/base/Object.hpp"
#include "mixr/base/ubf/Agent.hpp"

//------------------------------------------------------------------------------
// Class: UAVControlStationAgent
//
// Description:
//------------------------------------------------------------------------------

namespace asa {
namespace ubf {

class UAVControlStationAgent : public mixr::base::ubf::Agent
{
    DECLARE_SUBCLASS(UAVControlStationAgent, mixr::base::ubf::Agent)

public:
    UAVControlStationAgent();	
	     
    processor::ExternalProcessor* getExternalProcessor() { return processor; }
    const processor::ExternalProcessor* getExternalProcessor() const { return processor; }

    // Interface for handle AsaEvents
    bool onAsaEvents(asa::event::AsaEventObject* msg) override { return false; }
	  
    //-------------------------------------------
    // Inheritance
    void reset() override;
    void updateData(const double dt) override;
    bool event(const int event, Object* const obj = nullptr) override;

protected:
    processor::ExternalProcessor* processor{nullptr};

private:
    bool setSlotExternalProcessor(processor::ExternalProcessor* const msg);
};

} // namespace ubf
} // namespace asa
#endif
