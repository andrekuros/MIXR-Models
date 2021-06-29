#include "asacomponents/include/UAV/UAVMavLink.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
//#define _WINSOCKAPI_

#include <mutex>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mavlink_lib.lib")


bool silent = false;              ///< Wether console output should be enabled
bool verbose = false;             ///< Enable verbose output
bool debug = false;               ///< Enable debug functions and output
std::mutex mtx;

namespace asa {
	namespace components {

		IMPLEMENT_SUBCLASS(UAVMavLink, "UAVMavLink")
		EMPTY_SLOTTABLE(UAVMavLink)
        EMPTY_DELETEDATA(UAVMavLink)
		EMPTY_COPYDATA(UAVMavLink)
	

		UAVMavLink::UAVMavLink()
		{
			STANDARD_CONSTRUCTOR()
			static mixr::base::String generic("UAVMavLink");
			setType(&generic);

			connected = false;
			
			//Estabelecer a conexão via Socket
			WSADATA wsa;
			struct sockaddr_in server;

			printf("Initialising Winsock... \n");

			if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
			{
				printf("Failed. Error Code : %d \n", WSAGetLastError());
			}

							
			printf("Initialised.\n");
			
			//Criar Socket
			if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
			{
				printf("Could not create socket : %d \n", WSAGetLastError());
			}

			printf("Socket created.\n");

			//Parametros do MavLink (SITL)
			server.sin_addr.s_addr = inet_addr("127.0.0.1");
			server.sin_family = AF_INET;
			server.sin_port = htons(5772);

			
			//Connect to remote server
			if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
			{
				puts("connect error! \n");
			}
			
			printf("Connected. \n");

			lastStatus.packet_rx_drop_count = 0;

			//Solicitar envio de mensagem específica

			/* Mensagens para bloco de comando
			mavlink_message_t msg; //Mensagem a ser enviada
			mavlink_command_long_t comando;
			char buf[300];

			mavlink_request_data_stream_t data_cmd;

			comando.command = 511; //settaggio valori della struct relativa al comando
			comando.param1 = uint16_t(242);
			comando.param2 = 1000;

			data_cmd.target_system = uint8_t(1);
			data_cmd.target_component = uint8_t(0);
			data_cmd.req_message_rate = uint16_t(1000);
			data_cmd.req_stream_id = uint8_t(33);
			data_cmd.start_stop = uint8_t(1);

			unsigned len = mavlink_msg_request_data_stream_pack(1,1,&msg,255,0,33,uint16_t(10),1);

			t = mavlink_msg_request_data_stream_pack(mavlink_system.sysid, mavlink_system.compid, buf,Md.System, Md.Compent, MAVStreams[i], MAVRates[i], 1);

			*/

			//Mensagem para Ativar Stream de mensagens

			const int maxStreams = 3;
			const uint8_t MAVStreams[] = {
				MAV_DATA_STREAM_EXTRA1,     1,        //RAW1 manda attitude no Ardupilot
				MAV_DATA_STREAM_POSITION,    1,
				MAV_DATA_STREAM_EXTRA2, 1,
				//MAV_DATA_STREAM_RAW_SENSORS
			};
			const uint16_t MAVRates[] = { 0x3c, 0x3c , 0x3c };  //taxa de envio em Hz

			//Para cada Strem desejada cria a mensagem padrão e envia
			for (int i = 0; i < maxStreams * 2; i = i + 2)
			{
				mavlink_message_t msg; //Mensagem a ser enviada
				char buf[300];

				//cria a mensagem no padrão do MavLink
				//cout << int(MAVStreams[i]) << "---" << int(MAVStreams[i+1])<< endl;
				mavlink_msg_request_data_stream_pack(1, 1, &msg, 1, 255, MAVStreams[i], MAVRates[i/2], MAVStreams[i + 1]);
				//prepara para enviar em serial bitstream
				unsigned len = mavlink_msg_to_send_buffer((uint8_t*)buf, &msg);

				sendWinsock(s, buf, len, 0);

				//std::cout << "\n\nEnviado  STREAMs";

				Sleep(100);
			}
			
			lat = getInitLatitude();
			lon = getInitLongitude();
			alt = getInitAltitude();
			hdg = 0;

			//Sleep(500);			
			recvThread = new std::thread(&UAVMavLink::recvMAV, this);
		}
				
		void UAVMavLink::recvMAV()
        {

			bool run = true;
			
			while (run) {

				uint8_t msgReceived = false;

				//Loop que recebe respostas do MavLink
				uint8_t cp;
				mavlink_message_t message;
				mavlink_status_t status;
				msgReceived = false;

				if ((msgReceived = recv(s, (char*)&cp, 1, 0)) == SOCKET_ERROR)
				{
					printf("recv failed! \n");
				}

				if (msgReceived > 0)
				{
					// Check if a message could be decoded, return the message in case yes
					msgReceived = mavlink_parse_char(MAVLINK_COMM_1, cp, &message, &status);

					if (lastStatus.packet_rx_drop_count != status.packet_rx_drop_count)
					{
						if (verbose || debug) printf("ERROR: DROPPED %d PACKETS! \n", status.packet_rx_drop_count);
						if (debug)
						{
							unsigned char v = cp;
							fprintf(stderr, "%02x, ", v);
						}
					}
					lastStatus = status;
				}
				else
				{
					if (!silent) fprintf(stderr, "ERROR: Could not read from tcp! \n");

				}

				// If a message could be decoded, handle it
				if (msgReceived)
				{
					if (verbose || debug) std::cout << std::dec << "Received and forwarded tcp port message with id " << static_cast<unsigned int>(message.msgid) << " from system " << static_cast<int>(message.sysid) << std::endl;

					// DEBUG output
					if (debug)
					{
						fprintf(stderr, "Received tcp data: ");
						unsigned int i;
						uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
						unsigned int messageLength = mavlink_msg_to_send_buffer(buffer, &message);

						if (messageLength > MAVLINK_MAX_PACKET_LEN)
						{
							fprintf(stderr, "\nFATAL ERROR: MESSAGE LENGTH IS LARGER THAN BUFFER SIZE \n");
						}
						else
						{
							for (i = 0; i<messageLength; i++)
							{
								unsigned char v = buffer[i];
								fprintf(stderr, "%02x ", v);
							}
							fprintf(stderr, "\n");
						}
					}

					if (verbose || debug)
						printf("Received message from tcp with ID #%d (sys:%d|comp:%d):\n", message.msgid, message.sysid, message.compid);

					switch (message.msgid)
					{

						case MAVLINK_MSG_ID_HIGHRES_IMU:
						{
						mavlink_highres_imu_t imu;
						mavlink_msg_highres_imu_decode(&message, &imu);

						printf("Got message HIGHRES_IMU");
						printf("\t time: %llu\n", imu.time_usec);
						printf("\t acc  (NED):\t% f\t% f\t% f (m/s^2)\n", imu.xacc, imu.yacc, imu.zacc);
						printf("\t gyro (NED):\t% f\t% f\t% f (rad/s)\n", imu.xgyro, imu.ygyro, imu.zgyro);
						printf("\t mag  (NED):\t% f\t% f\t% f (Ga)\n", imu.xmag, imu.ymag, imu.zmag);
						printf("\t baro: \t %f (mBar)\n", imu.abs_pressure);
						printf("\t altitude: \t %f (m)\n", imu.pressure_alt);
						printf("\t temperature: \t %f C\n", imu.temperature);
						printf("\n");
						}
						break;

					case MAVLINK_MSG_ID_ATTITUDE:
					{
						mavlink_attitude_t packet;
						mavlink_msg_attitude_decode(&message, &packet);
						//printf("ATTITUDE ->  roll : %f ,  pitch: %f ,  yaw: %f , \n", packet.roll, packet.pitch, packet.yaw);

						mtx.lock();
						attitudes[0] = 0;//packet.roll;
						attitudes[1] = 0;//packet.pitch;
						mtx.unlock();
					}
					break;
					
					case 74:
					{
					
						mavlink_vfr_hud_t packet;

						mavlink_msg_vfr_hud_decode(&message, &packet);
						
						mtx.lock();
						
						alt = packet.alt;
						vel = packet.groundspeed;
						hdg = packet.heading;

						attitudes[2] = (float)(hdg*mixr::base::angle::D2RCC);
						//std::cout << "\nVFR: " << alt << "  -  hdg: " << hdg;
						
						mtx.unlock();

					}
					break;

					case MAVLINK_MSG_ID_GPS_RAW_INT:
					{
						mavlink_gps_raw_int_t packet;
						mavlink_msg_gps_raw_int_decode(&message, &packet);
						printf("WGS84- > lat: %d , long : %d , altitude(AMSL): %d, ground_speed: %d , tracking: %d\n", packet.lat, packet.lon, packet.alt, packet.vel, packet.cog);
					}
					break;

					case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
					{
						mavlink_global_position_int_t packet;
						mavlink_msg_global_position_int_decode(&message, &packet);
						
						mtx.lock();
						
						lat = packet.lat;
						lon = packet.lon;
						//alt = packet.alt;

						lat = lat / 10000000;
						lon = lon / 10000000;
						//alt = alt / 100000;
						
						//printf("Fused GPS- > lat: %d , long : %d , altitude(AMSL): %d, vx: %d , vy: %d, hdgg: %d\n", lat, lon, alt, packet.vx, packet.vy, packet.hdg);
						
						//std::cout << "\nlat: " << lat << " - lon: " << lon << " alt: :" << alt;
						mtx.unlock();
						//mixr::base::Vec3d vec{ lat, lon, 500 };						
					}
					break;
					}
				}
			}
		}


		void UAVMavLink::updateTC(const double dt)
		{
			
			BaseClass::updateTC(dt);
			mtx.lock();			
			
			setPositionLLA(lat, lon, alt);
			setEulerAngles(attitudes);
			//std::cout << "Init: " << getInitGeocentricPosition()[1];
			//std::cout << "\nlat: " << lat << " - lon: " << lon << " alt: :" << alt;
			
			mtx.unlock();
		}
	}
}

void sendWinsock(SOCKET s, char* buf, int len, int flags) {

	send(s, buf, len, 0);
}

