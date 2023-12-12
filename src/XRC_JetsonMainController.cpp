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
bool isTCPRecv = false;
bool isUart = true;
bool isUartSend = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char **argv)
{
    
    system("clear");
    cout << "[ XRC JetsonNano Controller Debug ]" << "\n";
    cout << "__________________________________________________________________" << "\n\n";

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    if(isUart)
    {
        uartController.initUartPort();
    }

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
                    if(serverController.receiveMsgFromClient())
                    {
                        if(!isTCPRecv)
                        {
                            cout << "[TCPServer] Receiving data from client ..." << "\n";
                            isTCPRecv = true;
                        }  
                    }
                    char * msg = serverController.getClientMsg();
                    //cout << "[TCPServer] Data received: " << msg << endl;
                    if(isUart)
                    {
                        auto uchrs = reinterpret_cast<unsigned char *>(const_cast<char *>(msg));
                        
                        if(!isUartSend)
                        {
                            cout << "[SERIAL] Sending data to ESP32 ..." << "\n";
                            isUartSend = true;
                        }
                        uartController.sendUart(uchrs);
                    }
                    
                }        
                serverController.closeConnection();
            }
        }


    }

    exit(0);
}