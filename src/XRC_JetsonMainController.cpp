#include "ZEDCom.h"
#include "Uart.h"   
#include "TCPServer.h"
#include "utils.hpp"
#include <stdlib.h>

ZEDCom zedController;
Uart uartController;
TCPServer serverController;

bool isZED = false;
bool isTCPServer = true;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
    
    system("clear");
    cout << "=====================================================================" << "\n";
    cout << "XRC JetsonNano Controller" << "\n";
    cout << "=====================================================================" << "\n\n\n";

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    if(isZED)
    {
        zedController.setConfiguration(argc, argv);
        zedController.openCam();
        SetCtrlHandler();

        while(!exit_app)
        {
            zedController.grabImage();
        }
        zedController.disableStreaming();
        zedController.closeCam();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    if(isTCPServer)
    {
        serverController.initSocket();
        
        if(serverController.openSocket())
        {
            serverController.listenForClient();
            
            if(serverController.connectToClient())
            {
                serverController.getSessionTime();
                
                while (1)
                {
                    serverController.receiveMsgFromClient();
                }        
                serverController.closeConnection();
            }
        }


    }

    exit(0);
}