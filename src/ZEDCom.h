
#include <sl/Camera.hpp>
#include "utils.hpp"


using namespace sl;
using namespace std;


class ZEDCom
{
    private:
        Camera _zedCam;
        InitParameters _init_parameters;

        int _argc;
        char **_argv;
        int _res_arg;

        void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");
        int parseArgs(int argc, char **argv, sl::InitParameters& param);

    public:
        ZEDCom(/* args */);
        ~ZEDCom();
        
        void setConfiguration(int argc, char **argv);
        int openCam();
        bool grabImage();
        void disableStreaming();
        void closeCam();
};


ZEDCom::ZEDCom(){}
ZEDCom::~ZEDCom(){}

bool ZEDCom::grabImage()
{
    if (_zedCam.grab() != ERROR_CODE::SUCCESS)
            sleep_ms(1);
    return true;
}

void ZEDCom::disableStreaming()
{
    _zedCam.disableStreaming();
}

void ZEDCom::closeCam()
{
    _zedCam.close();
}

int ZEDCom::openCam()
{
    auto returned_state = _zedCam.open(_init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Camera Open ...", returned_state, "Exit program...");
        return EXIT_FAILURE;
    }

    StreamingParameters stream_params;
    if (_argc == 2 && _res_arg == 1)
    {
        stream_params.port = atoi(_argv[1]);
        return 0;
    }
    if (_argc > 2)
    { 
        stream_params.port = atoi(_argv[2]);
        return 0;
    }

    returned_state = _zedCam.enableStreaming(stream_params);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Streaming initialization error: ", returned_state);
        return EXIT_FAILURE;
    }

    print("Streaming on port " + to_string(stream_params.port));

    // TODO Move to main -> ctrl input for cancelation
    SetCtrlHandler();
    return 0;
}

void ZEDCom::setConfiguration(int argc, char **argv)
{   
    _argc = argc;
    _argv = argv;

    _init_parameters.camera_resolution = sl::RESOLUTION::AUTO;
    _init_parameters.depth_mode = DEPTH_MODE::NONE;
    _init_parameters.sdk_verbose = 1;
    _res_arg = parseArgs(_argc, _argv, _init_parameters);

}

void ZEDCom::print(string msg_prefix, ERROR_CODE err_code, string msg_suffix) {
    cout << "[Sample]";
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout << " ";
    cout << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

int ZEDCom::parseArgs(int argc, char **argv, sl::InitParameters& param) {
    if (argc > 1 && string(argv[1]).find(".svo") != string::npos) {
        // SVO input mode
        param.input.setFromSVOFile(argv[1]);
        cout << "[Sample] Using SVO File input: " << argv[1] << endl;
    } else if (argc > 1 && string(argv[1]).find(".svo") == string::npos) {
        string arg = string(argv[1]);
        unsigned int a, b, c, d, port;
        if (sscanf(arg.c_str(), "%u.%u.%u.%u:%d", &a, &b, &c, &d, &port) == 5) {
            // Stream input mode - IP + port
            string ip_adress = to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d);
            param.input.setFromStream(sl::String(ip_adress.c_str()), port);
            cout << "[Sample] Using Stream input, IP : " << ip_adress << ", port : " << port << endl;
        } else if (sscanf(arg.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
            // Stream input mode - IP only
            param.input.setFromStream(sl::String(argv[1]));
            cout << "[Sample] Using Stream input, IP : " << argv[1] << endl;
        } else if (arg.find("HD2K") != string::npos) {
            param.camera_resolution = RESOLUTION::HD2K;
            cout << "[Sample] Using Camera in resolution HD2K" << endl;
        }else if (arg.find("HD1200") != string::npos) {
            param.camera_resolution = RESOLUTION::HD1200;
            cout << "[Sample] Using Camera in resolution HD1200" << endl;
        } else if (arg.find("HD1080") != string::npos) {
            param.camera_resolution = RESOLUTION::HD1080;
            cout << "[Sample] Using Camera in resolution HD1080" << endl;
        } else if (arg.find("HD720") != string::npos) {
            param.camera_resolution = RESOLUTION::HD720;
            cout << "[Sample] Using Camera in resolution HD720" << endl;
        }else if (arg.find("SVGA") != string::npos) {
            param.camera_resolution = RESOLUTION::SVGA;
            cout << "[Sample] Using Camera in resolution SVGA" << endl;
        }else if (arg.find("VGA") != string::npos) {
            param.camera_resolution = RESOLUTION::VGA;
            cout << "[Sample] Using Camera in resolution VGA" << endl;
        } else
            return 1;
    } else {
        // Default
        return 1;
    }
    return 0;
}
