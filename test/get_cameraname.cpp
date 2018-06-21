#include <pylon/PylonIncludes.h>
#include <iostream>

void getCameraName()
{
    Pylon::PylonAutoInitTerm autoInitTerm;

    Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();

    Pylon::DeviceInfoList_t devices;

    if (tlFactory.EnumerateDevices(devices) != 1)
        std::cout<<"camera link error."<<std::endl;

    Pylon::CInstantCameraArray cameras(devices.size());

    cameras[0].Attach(tlFactory.CreateDevice(devices[0]));

    std::cout<<cameras[0].GetDeviceInfo().GetFullName()<<std::endl;
}

int main(int argc, char **argv)
{
	getCameraName();

	return 0;
}
