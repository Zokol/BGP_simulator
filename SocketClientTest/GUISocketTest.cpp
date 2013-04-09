#include "ClientSocket.h"
#include "../SocketException.h"
#include <iostream>
#include <string>

using namespace std;

#define CONF_STR "<SIM_CONFIG>1,192.168.1.0/24,80,90,60,3,0_0_1;2,192.168.2.0/24,80,90,60,3,0_0_0</SIM_CONFIG>" 


int main ( int argc, char **argv )
{

    bool state = true, flag = true;

      ClientSocket client_socket ( "localhost", 50000 );
      // client_socket.set_non_blocking(true);
      cout << "Client sends the following configuration to the simulation:" << endl << CONF_STR << endl;

      std::string reply = "";
      while(flag){
      try
          {
              if(state)
                  {
                      client_socket << CONF_STR;
                      state = false;
                  }

              if(!state)
                  {
                      client_socket >> reply;
                      if(reply != "")
                          {
                              state = true;
                              std::cout << "Received: " << reply << std::endl;
                              if(reply.compare("Simu") == 0)
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


  flag = true;
  state = true;
  while(flag)
      {
          try
              {
                  if(state)
                      {
                          string l_Cmd;
                          std::cout << "Promt@ ";
                          cin >> l_Cmd;
                          client_socket << l_Cmd;
                          state = false;
                      }

                  if(!state)
                      {

                          client_socket >> reply;

                          if(reply != "")
                              {

                                  std::cout << "Received: " << reply << std::endl;
                                  if(reply.compare("END") == 0)
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
