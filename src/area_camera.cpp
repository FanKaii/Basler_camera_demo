#include "area_camera.h"

namespace cam
{
	bool AreaCamera::checkRoi(int64 imgWidth, int64 imgHeight)
	{
		if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width <= imgWidth && roi.y + roi.height <= imgHeight)
			return true;
		else
			return false;
	}

	const AreaCamera::ReturnInfo AreaCamera::cameraInit()
	{
		Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();

		Pylon::DeviceInfoList_t devices;

		if (tlFactory.EnumerateDevices(devices) != cameraNum_)
			return{ "cameras link error.", false };

		Pylon::CInstantCameraArray cameras(devices.size());
		Pylon::CDeviceInfo info;

		bool cameraFind = false;
		for (size_t i = 0; i < cameras.GetSize(); ++i)
		{
			cameras[i].Attach(tlFactory.CreateDevice(devices[i]));

			if (cameras[i].GetDeviceInfo().GetFullName() == cameraName_.c_str())
			{
				info = cameras[i].GetDeviceInfo();
				cameraFind = true;
			}
		}

		if (!cameraFind)
			return{ "camera name error.", false };

		Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateDevice(info);
		camera = new Pylon::CInstantCamera(pDevice);

		camera->Open();

		GenApi::INodeMap &nodemap = camera->GetNodeMap();

		GenApi::CIntegerPtr width(nodemap.GetNode("Width"));
		GenApi::CIntegerPtr height(nodemap.GetNode("Height"));

		image = cv::Mat(cv::Size(width->GetValue(), height->GetValue()), CV_8UC1);

		GenApi::CIntegerPtr exposureTimeRaw(nodemap.GetNode("ExposureTimeRaw"));
		exposureTimeRaw->SetValue(exposureTime_);

		if (setRoi_)
			if (!checkRoi(width->GetValue(), height->GetValue()))
				return{ cameraName_ + " roi set error.",false };

		return{ cameraName_ + " init success.", true };
	}

	const cv::Mat& AreaCamera::getImage()
	{
		camera->GrabOne(5000, ptrGrabResult);
		const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
		memcpy(image.data, pImageBuffer, image.rows*image.cols);

		if (setRoi_)
			imageRoi = cv::Mat(image, roi);
		else
			imageRoi = image;
		return imageRoi;
	}
}
