#include "area_camera.h"
#include "linescan_camera.h"

#include <Windows.h>
#include <iostream>

//#define _AREA_CAMERA_TEST
#define _LINE_SCAN_CAMERA_TEST

#ifdef _LINE_SCAN_CAMERA_TEST
#include <boost/thread.hpp>

/*
* 控制台键入s并按下enter开始扫描，按t并按下enter结束扫描
*/
void waitKey(cam::LineScanCamera *l_cam)
{
	if (std::cin.get() == 's')
		l_cam->grabStart();
	if (std::cin.get() == 't')
		l_cam->grabStop();
}
#endif

int main()
{
	Pylon::PylonAutoInitTerm autoInitTerm;

#ifdef _AREA_CAMERA_TEST
	cam::AreaCamera a_cam;
	a_cam.setParams(1, "Basler acA1300-75gm#00305323086E#169.254.111.8:3956", 5000);
	cam::AreaCamera::ReturnInfo info = a_cam.cameraInit();
	if (!info.initSuccess())
	{
		std::cout << info.getErrorType() << std::endl;
		
		system("pause");
		return -1;
	}

	while (true)
	{
		cv::Mat img_a = a_cam.getImage();

		cv::imshow("面阵图像", img_a);
		if (char(cv::waitKey(30)) == ' ')
			a_cam.close();
	}
#endif

#ifdef _LINE_SCAN_CAMERA_TEST
	cam::LineScanCamera l_cam;
	l_cam.setParams(1, "Basler acA1300-75gm#00305323086E#169.254.111.8:3956", 5000, 300);
	cam::AreaCamera::ReturnInfo info = l_cam.cameraInit();
	if (!info.initSuccess())
	{
		std::cout << info.getErrorType() << std::endl;

		system("pause");
		return -1;
	}

	boost::thread t(&waitKey, &l_cam);
	t.detach();

	while (true)
	{
		if (l_cam.grab())
		{
			cv::Mat img_l = l_cam.getImage();
			cv::imwrite("file.png", img_l);  //存储图像
		}
	}
	l_cam.close();
#endif

	system("pause");
	return 0;
}