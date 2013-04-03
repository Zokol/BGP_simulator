#include "ClientSocket.h"
#include "../SocketException.h"
#include <iostream>
#include <string>

int main ( int argc, char **argv )
{

  bool flag = true;

      ClientSocket client_socket ( "localhost", 30000 );
      client_socket.set_non_blocking(true);
      std::string reply;
      while(flag){
      try
          {
              client_socket << "STAR";
              client_socket >> reply;
              flag = false;
          }
      catch ( SocketException e ) {std::cout << "socket empty: " << e. description() << std::endl;}
      }
      std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;

  flag = true;
  while(flag)
      {
          try
              {
                  
                  client_socket >> reply;
              }
          catch ( SocketException e )
              {
                  // std::cout << "Exception was caught:" << e.description() << "\n";
                  //                  client_socket << "Waiting the simulation to end...";
              }
          if(reply.compare("END") == 0)
              {
                  flag = false;
                  std::cout << "End received: " << reply << std::endl;
              }
          // else
          //     std::cout << "Simulation running..." << std::endl;
      }


  return 0;
}
