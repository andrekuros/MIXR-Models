#ifndef __asa_ubf_UAVFuser_H__
#define __asa_ubf_UAVFuser_H__

#include "mixr/base/ubf/Arbiter.hpp"

namespace asa {
namespace ubf {

//------------------------------------------------------------------------------
// Class: UAVFuser
//
// Description: 
//------------------------------------------------------------------------------
class UAVFuser : public mixr::base::ubf::Arbiter
{
	DECLARE_SUBCLASS(UAVFuser, mixr::base::ubf::Arbiter)

public:
	UAVFuser();	

protected:
	mixr::base::ubf::AbstractAction* genComplexAction(mixr::base::List* const actionSet) override;

private:

};

}
}

#endif
