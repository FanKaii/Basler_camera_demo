#ifndef LINE_SCAN_CAMERA_
#define LINE_SCAN_CAMERA_

#include <opencv2/opencv.hpp>
#include "camera.h"

namespace cam
{
	class LineScanCamera : public Camera
	{
	public:
		LineScanCamera() = default;
		LineScanCamera(int64 cameraNum, const std::string &cameraName,
			int64 exposureTime, int64 lineRate,
			bool setRoi = false,
			const ImgRoi &imageRoi = ImgRoi())
			:Camera(cameraNum, cameraName, exposureTime, setRoi, imageRoi),
			lineRate_(lineRate) {}
		~LineScanCamera() = default;

		LineScanCamera &setParams(int64 cameraNum, const std::string &cameraName, int64 exposureTime, int64 lineRate, bool setRoi = false, const ImgRoi &imageRoi = ImgRoi())
		{
			cameraNum_ = cameraNum;
			cameraName_ = cameraName;
			exposureTime_ = exposureTime;
			lineRate_ = lineRate;
			setRoi_ = setRoi;
			roi = imageRoi;

			return *this;
		}

		const cv::Mat &getImage()override
		{
			if (setRoi_)
				imageRoi = cv::Mat(image, roi);
			else
				imageRoi = image;

			return imageRoi;
		}
		const ReturnInfo cameraInit()override;

		bool grab();
		void grabStart();
		void grabStop();
	private:
		int64 lineRate_;
		std::vector<uchar *> imgBuffer;

		cv::Size lineSize;
		size_t size;

		bool grabbing = false;
		bool imgCombed = true;

		bool checkRoi(int64);
	};
}

#endif
