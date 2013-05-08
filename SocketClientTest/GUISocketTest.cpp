#include "ClientSocket.h"
#include "../SocketException.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/*
* 101<->201<->301
*/
#define CONF_STR_3 "<SIM_CONFIG>101,1.1.1.0/24,0,100,60,3,0_0_1;201,2.2.2.0/24,0,100,60,3,0_0_0,1_0_2;301,3.1.0.0/16,0,100,60,3,0_1_1</SIM_CONFIG>"

/*
 * 		 501
 * 		  |
 * 		 401
 *		  |
 * 101<->201<->301
 */
#define CONF_STR_5 "<SIM_CONFIG>101,1.1.1.0/24,0,100,60,3,0_0_1;201,2.2.2.0/24,0,100,60,3,0_0_0,1_0_2,2_0_3;301,3.1.0.0/16,0,100,60,3,0_1_1;401,4.1.0.0/16,0,100,60,3,0_2_1,1_0_4;501,5.1.0.0/16,0,100,60,3,0_1_3</SIM_CONFIG>"

/*
 * 		 501
 * 		  |
 * 		 401_
 *		  |  \
 * 101<->201<->301
 */
#define CONF_STR_5_1 "<SIM_CONFIG>101,1.1.1.0/24,0,100,60,3,0_0_1;201,2.2.2.0/24,0,100,60,3,0_0_0,1_0_2,2_0_3;301,3.1.0.0/16,0,100,60,3,0_1_1,1_2_3;401,4.1.0.0/16,0,100,60,3,0_2_1,1_0_4,2_1_2;501,5.1.0.0/16,0,100,60,3,0_1_3</SIM_CONFIG>"


int main ( int argc, char **argv )
{

	bool state = true, flag = true;

	ClientSocket client_socket ( "localhost", 50000 );
	// client_socket.set_non_blocking(true);
	cout << "Client sends the following configuration to the simulation:" << endl << CONF_STR_5_1 << endl;

	std::string reply = "";
	while(flag){
		try
		{
			if(state)
			{
				client_socket << CONF_STR_5_1;
				state = false;
			}

			if(!state)
			{
				client_socket >> reply;
				if(reply != "")
				{
					state = true;
					std::cout << "Received: " << reply << std::endl;
					if(reply.compare("ACK") == 0)
						flag = false;
				}
			}


		}
		catch ( SocketException e )
		{
			std::cout << "socket empty: " << e. description() << std::endl;
			if(!(client_socket.is_valid()))
			{
				std::cout << "Socket not valid!" << std::endl;
				return 0;
			}
		}
	}

	cout << "Entering to Promt loop" << endl;
	flag = true;
	state = true;
	string l_Cmd = "", l_SInput;


	while(flag)
	{
		try
		{
			if(state)
			{
				l_Cmd = "<CMD>";
				l_SInput = "";

				std::cout << "Promt@ ";

				cin >> l_SInput;

				l_Cmd += l_SInput;
				l_Cmd += "</CMD>";
//				cout << "Command: " << l_Cmd << endl;
				client_socket << l_Cmd;
				state = false;
			}

			if(!state)
			{
				reply = "";
				client_socket >> reply;

				if(reply != "")
				{

					if(reply.find("<TABLE>") != string::npos)
					{
						string line = "", field = "";
						unsigned start = 7, end = 0, starT = 0, enD = 0, count = 0;

						if(l_SInput.substr(0,10).compare("READ_TABLE") == 0 || l_SInput.substr(0,14).compare("READ_RAW_TABLE") == 0)
						{

							if(l_SInput.substr(0,10).compare("READ_TABLE") == 0)
								cout << endl << "Routing table of router " << l_SInput.substr(11) << endl << "*********************************" << endl;
							else
								cout << endl << "Raw table of router " << l_SInput.substr(15) << endl << "*********************************" << endl;

						do {
							end = reply.find(";", start);
							line = reply.substr(start,end-start);
//							cout << line << endl;
							if(line.compare("</TABLE>") == 0)
								break;
							starT = 0;
							enD = 0;
							count = 0;
							do
							{
								enD = line.find(",", starT);
								field = line.substr(starT,enD-starT);
								starT = enD + 1;
								if(count == 0)
									cout << "ID: " << field;
								else if(count == 1)
									cout << " | Prefix: " << field;
								else if(count == 2)
									cout << "/" << field;
								else if(count == 3)
									cout << " | AS_path: " << field << endl;
								count++;
							}while(enD != string::npos);

							 start = end+1;

						}while(end != string::npos);
						cout << endl << "*********************************" << endl;
						}
						else
						{
							unsigned counT = 1;
							cout << endl << "Packet buffer of router " << l_SInput.substr(12) << endl << "*********************************" << endl;

							do {
								end = reply.find(";", start);
								line = reply.substr(start,end-start);
	//							cout << line << endl;
								if(line.compare("</TABLE>") == 0)
									break;
								starT = 0;
								enD = 0;
								count = 0;
								do
								{
									enD = line.find(",", starT);
									field = line.substr(starT,enD-starT);
									starT = enD + 1;
									if(count == 0)
										cout << "Packet: " << counT++ << endl << "#################################" << endl << "Version: " << field;
									else if(count == 1)
										cout << " | IHL: " << field;
									else if(count == 2)
										cout << " | DSCP: " << field;
									else if(count == 3)
										cout << " | ECN: " << field;
									else if(count == 4)
										cout << " | TOTAL LENGTH: " << field << endl;
									else if(count == 5)
										cout << "IDENTIFICATION: " << field;
									else if(count == 6)
										cout << " | FLAGS: [" << field;
									else if(count == 7)
										cout << "][" << field;
									else if(count == 8)
										cout << "][" << field << "]";
									else if(count == 9)
										cout << " | FRAGMENT OFFSET: " << field << endl;
									else if(count == 10)
										cout << "TTL: " << field;
									else if(count == 11)
										cout << " | PROTOCOL: " << field;
									else if(count == 12)
										cout << " | HEADER CHECKSUM: " << field << endl;
									else if(count == 13)
										cout << "SOURCE IP: " << field << endl;
									else if(count == 14)
										cout << "DESTINATION IP: " << field << endl;
									else if(count == 15)
										cout << "---------------------------------" << endl << "PAYLOAD:"  << endl << field << endl<< "#################################" << endl;
									count++;
								}while(enD != string::npos);

								 start = end+1;

							}while(end != string::npos);
							cout << endl << "*********************************" << endl;

						}
					}
					else
						std::cout << "Received: " << reply << std::endl;

					if(reply.compare("ACK") == 0 && l_SInput.compare("STOP") == 0)
						flag = false;
					if(reply.compare("STOP") == 0)
						cout << "Simulation terminates." << endl;
					else
					{
						state = true;
					}

				}

			}
		}
		catch ( SocketException e )
		{
			// std::cout << "Exception was caught:" << e.description() << "\n";
			//                  client_socket << "Waiting the simulation to end...";
			if(!(client_socket.is_valid()))
			{
				std::cout << "Socket not valid!" << std::endl;
				return 0;
			}

		}

	}


	return 0;
}
