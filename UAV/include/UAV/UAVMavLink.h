
#ifndef __mixr_models_UAV_H__
#define __mixr_models_UAV_H__

#include "mixr/models/player/air/AirVehicle.hpp"
#include "asacomponents/include/C/common/mavlink.h"
#include <thread>

namespace asa {
	namespace components {

		//------------------------------------------------------------------------------
		// Class: UAVMavLink
		// Factory name: UAVMavLink
		//------------------------------------------------------------------------------
		class UAVMavLink : public mixr::models::AirVehicle
		{
            DECLARE_SUBCLASS(UAVMavLink, mixr::models::AirVehicle)

		public:
            UAVMavLink();	

			void updateTC(const double dt = 0.0) override;
			
			void recvMAV();
						
			double lat, lon, alt, vel, hdg;

			mixr::base::Vec3f attitudes;

			bool connected;

			mavlink_status_t lastStatus;
			SOCKET s;

			std::thread* recvThread;


		protected:


		};
	}
}

#endif



void sendWinsock(SOCKET s, char * buf, int len, int flags);


