#include "linescan_camera.h"

namespace cam
{
	bool LineScanCamera::checkRoi(int64 imgWidth)
	{
		if (roi.x >= 0 && roi.y >= 0 && roi.x + roi.width <= imgWidth)
			return true;
		else
			return false;
	}

	const LineScanCamera::ReturnInfo LineScanCamera::cameraInit()
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
		height->SetValue(height->GetMin());

		lineSize.width = width->GetValue();
		lineSize.height = height->GetValue();
		size = lineSize.height*lineSize.width;

		GenApi::CIntegerPtr exposureTimeRaw(nodemap.GetNode("ExposureTimeRaw"));
		exposureTimeRaw->SetValue(exposureTime_);

		GenApi::CFloatPtr acqLineRate(nodemap.GetNode("AcquisitionLineRateAbs"));
		acqLineRate->SetValue(lineRate_);
		GenApi::CFloatPtr resLineRate(nodemap.GetNode("ResultingLineRateAbs"));
		if (fabs(lineRate_ - resLineRate->GetValue()) > 3)
			return{ cameraName_ + " line rate set too big.", false };

		if (setRoi_)
			if (!checkRoi(width->GetValue()))
				return{ cameraName_ + " roi set error.",false };

		return{ cameraName_ + " init success.", true };
	}

	bool LineScanCamera::grab()
	{
		if (grabbing)
		{
			camera->RetrieveResult(5000, ptrGrabResult);
			const uchar *pImageBuffer = (const uchar *)ptrGrabResult->GetBuffer();

			uchar *lineBuf = new uchar[size];
			memcpy(lineBuf, pImageBuffer, size);

			imgBuffer.push_back(lineBuf);
		}
		else
			if (!imgCombed)
			{
				image = cv::Mat(cv::Size(lineSize.width, imgBuffer.size()), CV_8UC1);
				for (size_t i = 0; i < imgBuffer.size(); ++i)
				{
					uchar *data = image.ptr<uchar>(i);
					memcpy(data, imgBuffer[i], size);
				}

				for (auto i : imgBuffer)
					delete[]i;
				imgBuffer.clear();

				imgCombed = true;
				camera->StopGrabbing();

				return true;
			}

		return false;
	}

	void LineScanCamera::grabStart()
	{
		camera->StartGrabbing();
		grabbing = true;
	}

	void LineScanCamera::grabStop()
	{
		grabbing = false;
		imgCombed = false;
	}
}
