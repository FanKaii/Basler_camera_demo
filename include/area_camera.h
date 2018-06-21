#ifndef AREA_CAMERA_H_
#define AREA_CAMERA_H_

#include <opencv2/opencv.hpp>
#include "camera.h"

namespace cam
{
	class AreaCamera :public Camera
	{
	public:
		AreaCamera() = default;
		using Camera::Camera;
		~AreaCamera() {}

		AreaCamera& setParams(int64 cameraNum, const std::string &cameraName, int64 exposureTime, bool setRoi = false, const ImgRoi &roi = ImgRoi())
		{
			this->cameraNum_ = cameraNum;
			this->exposureTime_ = exposureTime;
			this->cameraName_ = cameraName;
			this->setRoi_ = setRoi;
			this->roi = roi;

			return *this;
		}

		const cv::Mat &getImage()override;
		const ReturnInfo cameraInit()override;

	private:
		bool checkRoi(int64, int64);
	};
}

#endif