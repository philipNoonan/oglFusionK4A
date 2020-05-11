#include "interface.h"

K4AInterface::K4AInterface()
{



}

K4AInterface::~K4AInterface()
{

}

void K4AInterface::init(k4a_depth_mode_t depthMode, k4a_color_resolution_t colorResolution) {
    // Check for devices
    //

    deviceCount = k4a::device::get_installed_count();
    if (deviceCount == 0)
    {
        throw std::runtime_error("No Azure Kinect devices detected!");
    }

    config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;
    config.depth_mode = depthMode; // 640x576
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = colorResolution;


    config.synchronized_images_only = true;
}

bool K4AInterface::startCamera() {
    dev = k4a::device::open(K4A_DEVICE_DEFAULT);
    dev.start_cameras(&config);
    dev.start_imu();
    cameraRunning = true;

    //    K4A_CALIBRATION_TYPE_UNKNOWN = -1, /**< Calibration type is unknown */
    //    K4A_CALIBRATION_TYPE_DEPTH,        /**< Depth sensor */
    //    K4A_CALIBRATION_TYPE_COLOR,        /**< Color sensor */
    //    K4A_CALIBRATION_TYPE_GYRO,         /**< Gyroscope sensor */
    //    K4A_CALIBRATION_TYPE_ACCEL,        /**< Accelerometer sensor */
    //    K4A_CALIBRATION_TYPE_NUM,          /**< Number of types excluding unknown type*/

    calibration = dev.get_calibration(config.depth_mode, config.color_resolution);




    return true;
}



bool K4AInterface::getImages(k4a::image& depthImage, k4a::image& colorImage, k4a::image& infraImage) {
    if (dev.get_capture(&capture, std::chrono::milliseconds(0))) {
        depthImage = capture.get_depth_image();
        colorImage = capture.get_color_image();
        infraImage = capture.get_ir_image();
        
        return true;
    }
    else {
        return false;
    }
}

bool K4AInterface::getIMU(k4a_imu_sample_t& imuSample) {

    if (dev.get_imu_sample(&imuSample)) {
        return true;
    }
    else {
        return false;
    }
}