#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <mutex>

#include "GLCore/Shader.h"

#include <k4a/k4a.hpp>


namespace rgbd
{
	class imuTrack
	{
	private:
		// theta is the angle of camera rotation in x, y and z components
		glm::vec3 theta;
		std::mutex theta_mtx;
		/* alpha indicates the part that gyro and accelerometer take in computation of theta; higher alpha gives more weight to gyro, but too high
		values cause drift; lower alpha gives more weight to accelerometer, which is more sensitive to disturbances */
		float alpha = 0.98;
		bool first = true;
		// Keeps the arrival time of previous gyro frame
		double last_ts_gyro = 0;

	public:
		imuTrack();
		~imuTrack();

		void processGyro(k4a_imu_sample_t imuValues);

		void processAccel(k4a_imu_sample_t imuValues);
		
		glm::vec3 imuTrack::getTheta();
		void setTheta();

	};
}