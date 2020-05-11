#ifndef INTERF_H
#define INTERF_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <thread>

#include <glm/glm.hpp>

//#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
//#include <librealsense2/rs_advanced_mode.hpp>
//#include<librealsense2/rsutil.h>

#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "device.h"

#include <k4a/k4a.hpp>


struct FrameProperties
{
	int width;
	int height;
	int rate;
	int depthUnits;
};

struct FrameIntrinsics
{
	float cx;
	float cy;
	float fx;
	float fy;

	float k1, k2, p1, p2, k3;
};

class K4AInterface
{
public:
	K4AInterface();
	~K4AInterface();
	void init(k4a_depth_mode_t depthMode, k4a_color_resolution_t colorResolution);
	bool startCamera();
	bool getImages(k4a::image& depthBuffer, k4a::image& colorBuffer, k4a::image& infraImage);
	bool getIMU(k4a_imu_sample_t& imuSample);

	k4a::calibration getCalibration() {
		return calibration;
	}

private:
	uint32_t deviceCount;
	k4a_device_configuration_t config;
	k4a::calibration calibration;
	k4a::device dev;
	bool cameraRunning = false;
	k4a::capture capture;

};






#endif

