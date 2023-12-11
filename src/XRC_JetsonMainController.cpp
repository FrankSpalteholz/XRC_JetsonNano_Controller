#include "Dependencies.h"

int main(int argc, char **argv)
{
    ZEDCom zedController;

    zedController.setConfiguration(argc, argv);
    zedController.openCam();

    while(!exit_app)
    {
        zedController.grabImage();
    }

    zedController.disableStreaming();
    zedController.closeCam();

    exit(0);
}