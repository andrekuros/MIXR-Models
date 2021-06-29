#ifndef __asa_ubf_UAVControlStationBeh_H__
#define __asa_ubf_UAVControlStationBeh_H__

#include "mixr/base/ubf/AbstractBehavior.hpp"

//------------------------------------------------------------------------------
// Class: UAVControlStationBeh
//
// Description: base for UAV Behaviours
//------------------------------------------------------------------------------

namespace asa {
	namespace ubf {

		class UAVControlStationBeh : public mixr::base::ubf::AbstractBehavior
		{
			DECLARE_SUBCLASS(UAVControlStationBeh, mixr::base::ubf::AbstractBehavior)

		public:
			UAVControlStationBeh();

			virtual mixr::base::ubf::AbstractAction* genAction(const mixr::base::ubf::AbstractState* const state, const double dt) override;
			virtual void resetBehavior() {};
		};

	} // namespace ubf
} // namespace asa
#endif