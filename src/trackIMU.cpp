#include "trackIMU.h"

namespace rgbd
{
	imuTrack::imuTrack()
	{
        theta = glm::vec3(0);
	}

	imuTrack::~imuTrack()
	{

	}

	void imuTrack::processGyro(k4a_imu_sample_t imuValues) {
        if (first) // On the first iteration, use only data from accelerometer to set the camera's initial position
        {
            last_ts_gyro = imuValues.gyro_timestamp_usec;
            return;
        }
        // Holds the change in angle, as calculated from gyro
        glm::vec3 gyro_angle;

        // Initialize gyro_angle with data from gyro
        gyro_angle.z = -imuValues.gyro_sample.xyz.x; // Pitch
        gyro_angle.x = -imuValues.gyro_sample.xyz.y; // Yaw
        gyro_angle.y = imuValues.gyro_sample.xyz.z; // Roll

        // Compute the difference between arrival times of previous and current gyro frames
        double dt_gyro = (imuValues.gyro_timestamp_usec - last_ts_gyro) / 1000000.0;
        last_ts_gyro = imuValues.gyro_timestamp_usec;

        // Change in angle equals gyro measures * time passed since last measurement
        gyro_angle = gyro_angle * dt_gyro;

        // Apply the calculated change of angle to the current angle (theta)
        std::lock_guard<std::mutex> lock(theta_mtx);
        theta += glm::vec3(-gyro_angle.z, -gyro_angle.y, gyro_angle.x);
	}

    void imuTrack::processAccel(k4a_imu_sample_t imuValues) {
        // Holds the angle as calculated from accelerometer data
        glm::vec3 accel_angle;

        // Calculate rotation angle from accelerometer data
        accel_angle.z = atan2(imuValues.acc_sample.xyz.z, -imuValues.acc_sample.xyz.x);
        accel_angle.x = atan2(-imuValues.acc_sample.xyz.y, sqrt(imuValues.acc_sample.xyz.z * imuValues.acc_sample.xyz.z + imuValues.acc_sample.xyz.x * imuValues.acc_sample.xyz.x));

        // If it is the first iteration, set initial pose of camera according to accelerometer data (note the different handling for Y axis)
        std::lock_guard<std::mutex> lock(theta_mtx);
        if (first)
        {
            first = false;
            theta = accel_angle;
            // Since we can't infer the angle around Y axis using accelerometer data, we'll use PI as a convetion for the initial pose
            theta.y = 3.14159265358979323846;
        }
        else
        {
            /*
            Apply Complementary Filter:
                - high-pass filter = theta * alpha:  allows short-duration signals to pass through while filtering out signals
                  that are steady over time, is used to cancel out drift.
                - low-pass filter = accel * (1- alpha): lets through long term changes, filtering out short term fluctuations
            */
            theta.x = theta.x * alpha + accel_angle.x * (1 - alpha);
            theta.z = theta.z * alpha + accel_angle.z * (1 - alpha);
        }
    }

    glm::vec3 imuTrack::getTheta() {
        std::lock_guard<std::mutex> lock(theta_mtx);
        return theta;
    }

    void imuTrack::setTheta() {
        theta = glm::vec3(0);
        first = true;
    }


}