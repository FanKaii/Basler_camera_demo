#ifndef CAMERA_H_
#define CAMERA_H_

#include <pylon/PylonIncludes.h>
#include <opencv2/opencv.hpp>

namespace cam
{
	class Camera
	{
	public:
		using ImgRoi = cv::Rect;

		class ReturnInfo
		{
		public:
			ReturnInfo() = default;
			ReturnInfo(const std::string &_ErrorType, bool _Success) :errorType(_ErrorType), success(_Success) {}

			const std::string &getErrorType()const { return errorType; }
			bool initSuccess()const { return success; }

		private:
			std::string errorType;
			bool success;
		};

		Camera() = default;
		Camera(int64 cameraNum, const std::string &cameraName, int64 &exposureTime, bool setRoi = false, const ImgRoi &imageRoi = ImgRoi())
			:cameraNum_(cameraNum), exposureTime_(exposureTime), cameraName_(cameraName), setRoi_(setRoi), roi(imageRoi) {}
		Camera(const Camera &) = delete;
		Camera &operator=(const Camera &) = delete;

		virtual ~Camera() {}

		virtual const cv::Mat &getImage() = 0;
		virtual const ReturnInfo cameraInit() = 0;    //cameraOpen() included

		virtual void close() { camera->Close(); delete camera; }
	protected:
		int64 cameraNum_ = 1;     //当前所链接相机的总个数

		int64 exposureTime_ = 5000;
		std::string cameraName_;

		bool setRoi_ = false;
		ImgRoi roi;

		cv::Mat image;
		cv::Mat imageRoi;

		Pylon::CInstantCamera *camera = nullptr;
		Pylon::CGrabResultPtr ptrGrabResult;
	};
}

#endif
