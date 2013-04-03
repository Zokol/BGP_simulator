#include "ClientSocket.h"
#include "../SocketException.h"
#include <iostream>
#include <string>

int main ( int argc, char **argv )
{

    bool state = true, flag = true;

      ClientSocket client_socket ( "localhost", 30000 );
      // client_socket.set_non_blocking(true);
      std::string reply = "";
      while(flag){
      try
          {
              if(state)
                  {
                      client_socket << "STAR";
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
      catch ( SocketException e ) {std::cout << "socket empty: " << e. description() << std::endl;}
      }


  flag = true;
  state = true;
  while(flag)
      {
          try
              {
                  if(state)
                      {
                          client_socket << "Request";
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
                                  else if(reply.compare("Response") == 0)
                                      state = true;
                              }

                      }
              }
          catch ( SocketException e )
              {
                  // std::cout << "Exception was caught:" << e.description() << "\n";
                  //                  client_socket << "Waiting the simulation to end...";

              }

      }


  return 0;
}
