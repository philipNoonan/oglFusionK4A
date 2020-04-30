#include "Frame.h"

namespace rgbd
{
	Frame::Frame()
	{
	}

	Frame::~Frame()
	{
	}

	void Frame::setCalibration(k4a::calibration calibration)
	{
		cal = calibration;
	}

	void Frame::create()
	{
		int wD = cal.depth_camera_calibration.resolution_width;
		int hD = cal.depth_camera_calibration.resolution_height;

		int wC = cal.color_camera_calibration.resolution_width;
		int hC = cal.color_camera_calibration.resolution_height;

		width = wD; // CHECK THIS!
		height = hD; // CHECK THIS!


		int idx = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				depthToColor[i][j] = cal.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation[idx];
				idx++;
			}
		}
		for (int i = 0; i < 3; i++) {
			depthToColor[3][i] = cal.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[i];
		}
		K = glm::mat4(1.0f);

		K[0][0] = cal.depth_camera_calibration.intrinsics.parameters.param.fx;
		K[1][1] = cal.depth_camera_calibration.intrinsics.parameters.param.fy;
		K[2][0] = cal.depth_camera_calibration.intrinsics.parameters.param.cx;
		K[2][1] = cal.depth_camera_calibration.intrinsics.parameters.param.cy;

		frameData.resize(1);
		int numberOfLevelsDepth = GLHelper::numberOfLevels(glm::ivec3(wD, hD, 1));
		int numberOfLevelsColor = GLHelper::numberOfLevels(glm::ivec3(wC, hC, 1));

		// Color needs only "one" level
		// Note: Color must be 4ch since compute shader does not support rgb8 internal format.
		frameData[0].colorMap = std::make_shared<gl::Texture>();
		frameData[0].colorMap->createStorage(numberOfLevelsColor, wC, hC, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
		frameData[0].colorMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].colorMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].colorPreviousMap = std::make_shared<gl::Texture>();
		frameData[0].colorPreviousMap->createStorage(numberOfLevelsColor, wC, hC, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
		frameData[0].colorPreviousMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].colorPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].colorFilteredMap = std::make_shared<gl::Texture>();
		frameData[0].colorFilteredMap->createStorage(numberOfLevelsColor, wC, hC, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
		frameData[0].colorFilteredMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].colorFilteredMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].colorAlignedToDepthMap = std::make_shared<gl::Texture>();
		frameData[0].colorAlignedToDepthMap->createStorage(numberOfLevelsColor, wC, hC, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
		frameData[0].colorAlignedToDepthMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		frameData[0].colorAlignedToDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].depthMap = std::make_shared<gl::Texture>();
		frameData[0].depthMap->createStorage(numberOfLevelsDepth, wD, hD, 1, GL_R32F, gl::TextureType::FLOAT32, 0);
		frameData[0].depthMap->setFiltering(GL_NEAREST, GL_NEAREST);
		frameData[0].depthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].vertexMap = std::make_shared<gl::Texture>();
		frameData[0].vertexMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].vertexMap->setFiltering(GL_LINEAR, GL_LINEAR);
		frameData[0].vertexMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].normalMap = std::make_shared<gl::Texture>();
		frameData[0].normalMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].normalMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].normalMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].virtualVertexMap = std::make_shared<gl::Texture>();
		frameData[0].virtualVertexMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].virtualVertexMap->setFiltering(GL_LINEAR, GL_LINEAR);
		frameData[0].virtualVertexMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].normalMap = std::make_shared<gl::Texture>();
		frameData[0].normalMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].normalMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].normalMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].virtualNormalMap = std::make_shared<gl::Texture>();
		frameData[0].virtualNormalMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].virtualNormalMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
		frameData[0].virtualNormalMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].depthPreviousMap = std::make_shared<gl::Texture>();
		frameData[0].depthPreviousMap->createStorage(numberOfLevelsDepth, wD, hD, 1, GL_R32F, gl::TextureType::FLOAT32, 0);
		frameData[0].depthPreviousMap->setFiltering(GL_NEAREST, GL_NEAREST);
		frameData[0].depthPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		frameData[0].vertexPreviousMap = std::make_shared<gl::Texture>();
		frameData[0].vertexPreviousMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		frameData[0].vertexPreviousMap->setFiltering(GL_NEAREST, GL_NEAREST);
		frameData[0].vertexPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		shortDepthMap = std::make_shared<gl::Texture>();
		shortDepthMap->create(0, wD, hD, 1, gl::TextureType::UINT16);
		shortDepthMap->setFiltering(GL_NEAREST, GL_NEAREST);
		shortDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		infraMap = std::make_shared<gl::Texture>();
		infraMap->create(0, wD, hD, 1, gl::TextureType::UINT16);
		infraMap->setFiltering(GL_NEAREST, GL_NEAREST);
		infraMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		rawDepthMap = std::make_shared<gl::Texture>();
		rawDepthMap->create(0, wD, hD, 1, gl::TextureType::FLOAT32);
		rawDepthMap->setFiltering(GL_NEAREST, GL_NEAREST);
		rawDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);


		trackMap = std::make_shared<gl::Texture>();
		trackMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
		trackMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		trackMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);



		//infraMap = std::make_shared<gl::Texture>();
		//infraMap->createStorage(numberOfLevelsDepth, wD, hD, 1, GL_R16UI, gl::TextureType::UINT16, 0);
		//infraMap->setFiltering(GL_NEAREST, GL_NEAREST);
		//infraMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		testMap = std::make_shared<gl::Texture>();
		testMap->createStorage(numberOfLevelsDepth, wD, hD, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
		testMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		testMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		mappingD2CMap = std::make_shared<gl::Texture>();
		mappingD2CMap->createStorage(numberOfLevelsDepth, wD, hD, 2, GL_RG16UI, gl::TextureType::UINT16, 0);
		mappingD2CMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		mappingD2CMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

		mappingC2DMap = std::make_shared<gl::Texture>();
		mappingC2DMap->createStorage(numberOfLevelsColor, wC, hC, 2, GL_RG16UI, gl::TextureType::UINT16, 0);
		mappingC2DMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
		mappingC2DMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);


		bilateralFilter = std::make_shared<rgbd::BilateralFilter>(progs["BilateralFilter"]);
		casFilter = std::make_shared<rgbd::CASFilter>(progs["CASFilter"]);
		alignDC = std::make_shared<rgbd::AlignDepthColor>(progs["alignDepthColor"]);

		vertexMapProc = std::make_shared<rgbd::CalcVertexMap>(K, progs["CalcVertexMap"]);
		normalMapProc = std::make_shared<rgbd::CalcNormalMap>(progs["CalcNormalMap"]);
		downSampling.resize(numberOfLevelsDepth - 1);

		for (int lv = 0; lv < downSampling.size(); ++lv)
		{
			downSampling[lv] = std::make_shared<rgbd::DownSampling>(
				progs["DownSamplingC"], progs["DownSamplingD"],
				progs["DownSamplingV"], progs["DownSamplingN"]
				);
		}

	}

	void Frame::loadShaders(
		const std::string& folderPath
	)
	{
		progs.insert(std::make_pair("BilateralFilter", std::make_shared<gl::Shader>(folderPath + "BilateralFilter.comp")));
		progs.insert(std::make_pair("CASFilter", std::make_shared<gl::Shader>(folderPath + "contrastAdaptiveSharpening.comp")));
		progs.insert(std::make_pair("alignDepthColor", std::make_shared<gl::Shader>(folderPath + "alignDepthColor.comp")));
		progs.insert(std::make_pair("CalcVertexMap", std::make_shared<gl::Shader>(folderPath + "CalcVertexMap.comp")));
		progs.insert(std::make_pair("CalcNormalMap", std::make_shared<gl::Shader>(folderPath + "CalcNormalMap.comp")));
		progs.insert(std::make_pair("DownSamplingC", std::make_shared<gl::Shader>(folderPath + "DownSamplingC.comp")));
		progs.insert(std::make_pair("DownSamplingD", std::make_shared<gl::Shader>(folderPath + "DownSamplingD.comp")));
		progs.insert(std::make_pair("DownSamplingV", std::make_shared<gl::Shader>(folderPath + "DownSamplingV.comp")));
		progs.insert(std::make_pair("DownSamplingN", std::make_shared<gl::Shader>(folderPath + "DownSamplingN.comp")));

	}

	//void Frame::create(
	//	int width,
	//	int height,
	//	int maxLevel,
	//	glm::mat4 K,
	//	float depthScale,
	//	std::map<std::string, const gl::Shader::Ptr>& progs
	//)
	//{
	//	this->width = width;
	//	this->height = height;
	//	this->K = K;

	//	frameData.resize(1);
	//	int numberOfLevels = GLHelper::numberOfLevels(glm::ivec3(width, height, 1));

	//	// Color needs only "one" level
	//	// Note: Color must be 4ch since compute shader does not support rgb8 internal format.
	//	frameData[0].colorMap = std::make_shared<gl::Texture>();
	//	frameData[0].colorMap->createStorage(numberOfLevels, width, height, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
	//	//frameData[0].colorMap->create(0, width, height, 4, gl::TextureType::COLOR);
	//	frameData[0].colorMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	//	frameData[0].colorMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].colorPreviousMap = std::make_shared<gl::Texture>();
	//	frameData[0].colorPreviousMap->createStorage(numberOfLevels, width, height, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
	//	//frameData[0].colorPreviousMap->create(0, width, height, 4, gl::TextureType::COLOR);
	//	frameData[0].colorPreviousMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	//	frameData[0].colorPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].colorFilteredMap = std::make_shared<gl::Texture>();
	//	frameData[0].colorFilteredMap->createStorage(numberOfLevels, width, height, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
	//	//frameData[0].colorFilteredMap->create(0, width, height, 4, gl::TextureType::COLOR);
	//	frameData[0].colorFilteredMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	//	frameData[0].colorFilteredMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].colorAlignedToDepthMap = std::make_shared<gl::Texture>();
	//	frameData[0].colorAlignedToDepthMap->createStorage(numberOfLevels, width, height, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
	//	//frameData[0].colorAlignedToDepthMap->create(0, width, height, 4, gl::TextureType::COLOR);
	//	frameData[0].colorAlignedToDepthMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	frameData[0].colorAlignedToDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].depthMap = std::make_shared<gl::Texture>();
	//	frameData[0].depthMap->createStorage(maxLevel, width, height, 1, GL_R32F, gl::TextureType::FLOAT32, 0);
	//	frameData[0].depthMap->setFiltering(GL_NEAREST, GL_NEAREST);
	//	frameData[0].depthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].vertexMap = std::make_shared<gl::Texture>();
	//	frameData[0].vertexMap->createStorage(maxLevel, width, height, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
	//	frameData[0].vertexMap->setFiltering(GL_LINEAR, GL_LINEAR);
	//	frameData[0].vertexMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].normalMap = std::make_shared<gl::Texture>();
	//	frameData[0].normalMap->createStorage(maxLevel, width, height, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
	//	frameData[0].normalMap->setFiltering(GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST);
	//	frameData[0].normalMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].depthPreviousMap = std::make_shared<gl::Texture>();
	//	frameData[0].depthPreviousMap->createStorage(maxLevel, width, height, 1, GL_R32F, gl::TextureType::FLOAT32, 0);
	//	frameData[0].depthPreviousMap->setFiltering(GL_NEAREST, GL_NEAREST);
	//	frameData[0].depthPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	frameData[0].vertexPreviousMap = std::make_shared<gl::Texture>();
	//	frameData[0].vertexPreviousMap->createStorage(maxLevel, width, height, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
	//	frameData[0].vertexPreviousMap->setFiltering(GL_NEAREST, GL_NEAREST);
	//	frameData[0].vertexPreviousMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	shortDepthMap = std::make_shared<gl::Texture>();
	//	shortDepthMap->create(0, width, height, 1, gl::TextureType::UINT16);
	//	shortDepthMap->setFiltering(GL_NEAREST, GL_NEAREST);
	//	shortDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	rawDepthMap = std::make_shared<gl::Texture>();
	//	rawDepthMap->create(0, width, height, 1, gl::TextureType::FLOAT32);
	//	rawDepthMap->setFiltering(GL_NEAREST, GL_NEAREST);
	//	rawDepthMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);


	//	trackMap = std::make_shared<gl::Texture>();
	//	trackMap->createStorage(maxLevel, width, height, 4, GL_RGBA8, gl::TextureType::COLOR, 1);
	//	trackMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	trackMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	infraMap = std::make_shared<gl::Texture>();
	//	infraMap->create(0, width, height, 1, gl::TextureType::COLOR);
	//	infraMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	infraMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	testMap = std::make_shared<gl::Texture>();
	//	testMap->createStorage(maxLevel, width, height, 4, GL_RGBA32F, gl::TextureType::FLOAT32, 0);
	//	testMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	testMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	mappingD2CMap = std::make_shared<gl::Texture>();
	//	mappingD2CMap->createStorage(maxLevel, width, height, 2, GL_RG16UI, gl::TextureType::UINT16, 0);
	//	mappingD2CMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	mappingD2CMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	mappingC2DMap = std::make_shared<gl::Texture>();
	//	mappingC2DMap->createStorage(maxLevel, width, height, 2, GL_RG16UI, gl::TextureType::UINT16, 0);
	//	mappingC2DMap->setFiltering(GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST);
	//	mappingC2DMap->setWarp(gl::TextureWarp::CLAMP_TO_EDGE);

	//	bilateralFilter = std::make_shared<rgbd::BilateralFilter>(progs["BilateralFilter"]);
	//	casFilter = std::make_shared<rgbd::CASFilter>(progs["CASFilter"]);
	//	alignDC = std::make_shared<rgbd::AlignDepthColor>(progs["alignDepthColor"]);

	//	vertexMapProc = std::make_shared<rgbd::CalcVertexMap>(K, progs["CalcVertexMap"]);
	//	normalMapProc = std::make_shared<rgbd::CalcNormalMap>(progs["CalcNormalMap"]);
	//	downSampling.resize(maxLevel - 1);

	//	for (int lv = 0; lv < downSampling.size(); ++lv)
	//	{
	//		downSampling[lv] = std::make_shared<rgbd::DownSampling>(
	//			progs["DownSamplingC"], progs["DownSamplingD"],
	//			progs["DownSamplingV"], progs["DownSamplingN"]
	//			);
	//	}
	//}

	//void Frame::update(
	//	int numberOfCameras,
	//	float depthScale,
	//	float depthMin,
	//	float depthMax,
	//	glm::vec2 bottomLeft,
	//	glm::vec2 topRight,
	//	const glm::ivec2 pixel,
	//	glm::vec3 &vertex,
	//	cv::Mat &depthM,
	//	float sharpness,
	//	float bfSigma,
	//	float bfDSigma
	//) 
	//{
	//	std::vector<rs2::frame> depthFrame(numberOfCameras);
	//	std::vector<rs2::frame> colorFrame(numberOfCameras);
	//	std::vector<rs2::frame> infraFrame(numberOfCameras);

	//	if (!firstFrame)
	//	{
	//		for (int lvl = 0; lvl < GLHelper::numberOfLevels(glm::ivec3(frameData[0].colorFilteredMap->getWidth(), frameData[0].colorFilteredMap->getHeight(), 1)); lvl++)
	//		{
	//			if (colorFrameArrived)
	//			{
	//				glCopyImageSubData(frameData[0].colorFilteredMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//					frameData[0].colorPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//					frameData[0].colorFilteredMap->getWidth() >> lvl, frameData[0].colorFilteredMap->getHeight() >> lvl, 1);
	//			}


	//			glCopyImageSubData(frameData[0].depthMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].depthPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].depthMap->getWidth() >> lvl, frameData[0].depthMap->getHeight() >> lvl, 1);

	//			glCopyImageSubData(frameData[0].vertexMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].vertexPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].vertexMap->getWidth() >> lvl, frameData[0].vertexMap->getHeight() >> lvl, 1);
	//		}

	//		colorFrameArrived = false;

	//	}


	//	for (int camNumber = 0; camNumber < numberOfCameras; camNumber++)
	//	{
	//		colorQ[camNumber].poll_for_frame(&colorFrame[camNumber]);
	//		if (colorFrame[camNumber] != NULL)
	//		{
	//			frameData[0].colorMap->update(colorFrame[camNumber].get_data());
	//			if (colorFrame[camNumber].supports_frame_metadata(RS2_FRAME_METADATA_SENSOR_TIMESTAMP))
	//			{
	//				colorTime = colorFrame[camNumber].get_frame_metadata(RS2_FRAME_METADATA_SENSOR_TIMESTAMP);
	//			}

	//			colorFrameArrived = true;
	//		}

	//		infraQ[camNumber].poll_for_frame(&infraFrame[camNumber]);
	//		if (infraFrame[camNumber] != NULL)
	//		{
	//			infraMap->update(infraFrame[camNumber].get_data());
	//		}

	//		depthQ[camNumber].poll_for_frame(&depthFrame[camNumber]);
	//		if (depthFrame[camNumber] != NULL)
	//		{
	//			shortDepthMap->update(depthFrame[camNumber].get_data());

	//			if (depthFrame[camNumber].supports_frame_metadata(RS2_FRAME_METADATA_SENSOR_TIMESTAMP))
	//			{
	//				depthTime = depthFrame[camNumber].get_frame_metadata(RS2_FRAME_METADATA_SENSOR_TIMESTAMP);
	//			}

	//			if (depthFrame[camNumber].supports_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER))
	//			{
	//				depthCount++;// = depthFrame[camNumber].get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
	//			}
	//						
	//			const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depthFrame[camNumber].get_data());

	//			int depth_pixel_index = (pixel.y * shortDepthMap->getWidth() + pixel.x);

	//			glm::vec4 tempPoint(0.0f, 0.0f, 0.0f, 1.0f);

	//			tempPoint.z = p_depth_frame[depth_pixel_index] * depthScale;

	//			tempPoint = tempPoint.z * (glm::inverse(K) * glm::vec4(pixel, 1.0f, 0.0f));

	//			vertex.x = tempPoint.x;
	//			vertex.y = tempPoint.y;
	//			vertex.z = tempPoint.z;

	//			//depthM = cv::Mat(height, width, CV_16SC1, (void*)depthFrame[camNumber].get_data());
	//		}
	//	}

	//	std::dynamic_pointer_cast<rgbd::BilateralFilter>(bilateralFilter)->execute(shortDepthMap, rawDepthMap, frameData[0].depthMap, depthScale, bfSigma, bfDSigma);
	//	std::dynamic_pointer_cast<rgbd::CASFilter>(casFilter)->execute(frameData[0].colorMap, frameData[0].colorFilteredMap, sharpness);
	//	std::dynamic_pointer_cast<rgbd::CalcVertexMap>(vertexMapProc)->execute(frameData[0].depthMap, frameData[0].vertexMap, depthMin, depthMax, bottomLeft, topRight);
	//	std::dynamic_pointer_cast<rgbd::CalcNormalMap>(normalMapProc)->execute(frameData[0].vertexMap, frameData[0].normalMap);

	//	frameData[0].colorFilteredMap->mipmap();

	//	if (firstFrame)
	//	{
	//		frameCount++;
	//		// CAN WE JUST SWAP IDs?
	//		for (int lvl = 0; lvl < GLHelper::numberOfLevels(glm::ivec3(frameData[0].colorFilteredMap->getWidth(), frameData[0].colorFilteredMap->getHeight(), 1)); lvl++)
	//		{
	//			glCopyImageSubData(frameData[0].colorFilteredMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].colorPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].colorFilteredMap->getWidth() >> lvl, frameData[0].colorFilteredMap->getHeight() >> lvl, 1);
	//			glCopyImageSubData(frameData[0].depthMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].depthPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].depthMap->getWidth() >> lvl, frameData[0].depthMap->getHeight() >> lvl, 1);
	//			glCopyImageSubData(frameData[0].vertexMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].vertexPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
	//				frameData[0].vertexMap->getWidth() >> lvl, frameData[0].vertexMap->getHeight() >> lvl, 1);
	//		}
	//		firstFrame = false;
	//	}
	//	update();
	//}


void Frame::update(
	k4a::image& depthImage,
	k4a::image& colorImage,
	k4a::image& infraImage,
	int numberOfCameras,
	float depthMin,
	float depthMax,
	glm::vec2 bottomLeft,
	glm::vec2 topRight,
	const glm::ivec2 pixel,
	glm::vec3 &vertex,
	float sharpness,
	float bfSigma,
	float bfDSigma
	) 
	{

		if (!firstFrame)
		{
			for (int lvl = 0; lvl < GLHelper::numberOfLevels(glm::ivec3(frameData[0].colorFilteredMap->getWidth(), frameData[0].colorFilteredMap->getHeight(), 1)); lvl++)
			{
				if (colorFrameArrived)
				{
					glCopyImageSubData(frameData[0].colorFilteredMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
						frameData[0].colorPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
						frameData[0].colorFilteredMap->getWidth() >> lvl, frameData[0].colorFilteredMap->getHeight() >> lvl, 1);
				}


				glCopyImageSubData(frameData[0].depthMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].depthPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].depthMap->getWidth() >> lvl, frameData[0].depthMap->getHeight() >> lvl, 1);

				glCopyImageSubData(frameData[0].vertexMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].vertexPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].vertexMap->getWidth() >> lvl, frameData[0].vertexMap->getHeight() >> lvl, 1);
			}

			colorFrameArrived = false;

		}


		for (int camNumber = 0; camNumber < numberOfCameras; camNumber++)
		{

				frameData[0].colorMap->update(colorImage.get_buffer());

				colorTime = colorImage.get_device_timestamp();
				
				colorFrameArrived = true;
			
				infraMap->update(infraImage.get_buffer());
			
				shortDepthMap->update(depthImage.get_buffer());

				depthTime = depthImage.get_device_timestamp();
				
				depthCount++;// = depthFrame[camNumber].get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
				
							
				const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depthImage.get_buffer());

				int depth_pixel_index = (pixel.y * shortDepthMap->getWidth() + pixel.x);

				glm::vec4 tempPoint(0.0f, 0.0f, 0.0f, 1.0f);

				tempPoint.z = p_depth_frame[depth_pixel_index] * 0.001f; // depthScale

				tempPoint = tempPoint.z * (glm::inverse(K) * glm::vec4(pixel, 1.0f, 0.0f));

				vertex.x = tempPoint.x;
				vertex.y = tempPoint.y;
				vertex.z = tempPoint.z;

				//std::cout << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;


				//depthM = cv::Mat(height, width, CV_16SC1, (void*)depthFrame[camNumber].get_data());
			
		}

		std::dynamic_pointer_cast<rgbd::BilateralFilter>(bilateralFilter)->execute(shortDepthMap, rawDepthMap, frameData[0].depthMap, 0.001f, bfSigma, bfDSigma);
		std::dynamic_pointer_cast<rgbd::CASFilter>(casFilter)->execute(frameData[0].colorMap, frameData[0].colorFilteredMap, sharpness);
		std::dynamic_pointer_cast<rgbd::CalcVertexMap>(vertexMapProc)->execute(frameData[0].depthMap, frameData[0].vertexMap, depthMin, depthMax, bottomLeft, topRight);
		std::dynamic_pointer_cast<rgbd::CalcNormalMap>(normalMapProc)->execute(frameData[0].vertexMap, frameData[0].normalMap);

		frameData[0].colorFilteredMap->mipmap();

		if (firstFrame)
		{
			frameCount++;
			// CAN WE JUST SWAP IDs?

			for (int lvl = 0; lvl < GLHelper::numberOfLevels(glm::ivec3(frameData[0].colorFilteredMap->getWidth(), frameData[0].colorFilteredMap->getHeight(), 1)); lvl++)
			{
				glCopyImageSubData(frameData[0].colorFilteredMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].colorPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].colorFilteredMap->getWidth() >> lvl, frameData[0].colorFilteredMap->getHeight() >> lvl, 1);

				glCopyImageSubData(frameData[0].depthMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].depthPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].depthMap->getWidth() >> lvl, frameData[0].depthMap->getHeight() >> lvl, 1);

				glCopyImageSubData(frameData[0].vertexMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].vertexPreviousMap->getID(), GL_TEXTURE_2D, lvl, 0, 0, 0,
					frameData[0].vertexMap->getWidth() >> lvl, frameData[0].vertexMap->getHeight() >> lvl, 1);

			}
			firstFrame = false;
		}
		update();
	}


	std::chrono::microseconds Frame::getColorTime()
	{
		return colorTime;
	}

	std::chrono::microseconds Frame::getDepthTime()
	{
		return depthTime;
	}

	uint64_t Frame::getDepthFrameCount()
	{
		return depthCount;
	}


	void Frame::getDepthAndColorMats(cv::Mat &depth, cv::Mat &color)
	{
		//depth = this->depthMat.clone();
		//color = this->colorMat.clone();
	}

	//void Frame::update(
	//	const void *colorData,
	//	const void *depthData,
	//	float bfSigma,
	//	float bfDSigma
	//) const
	//{
	//	frameData[0].colorMap->update(colorData);
	//	rawDepthMap->update(depthData);
	//	std::dynamic_pointer_cast<rgbd::BilateralFilter>(bilateralFilter)->execute(shortDepthMap, rawDepthMap, frameData[0].depthMap, 1.0f, bfSigma, bfDSigma);
	//	std::dynamic_pointer_cast<rgbd::CalcVertexMap>(vertexMapProc)->execute(frameData[0].depthMap, frameData[0].vertexMap, depthMin, depthMax, bottomLeft, topRight);
	//	std::dynamic_pointer_cast<rgbd::CalcNormalMap>(normalMapProc)->execute(frameData[0].vertexMap, frameData[0].normalMap);

	//	update();
	//}


	void Frame::alignDepthTocolor(glm::vec4 depthIntrins, glm::vec4 colorIntrins, std::vector<unsigned char> &colorVec)
	{
		// from the verteex map, apply the depth to color extrinsic
		// project from 3D to color space using color intrins
		// read color from color map and update color map for the original vertID in the vertex map

		std::dynamic_pointer_cast<rgbd::AlignDepthColor>(alignDC)->execute(frameData[0].vertexMap, frameData[0].colorFilteredMap, frameData[0].colorAlignedToDepthMap, this->mappingC2DMap, this->mappingD2CMap, depthToColor, colorIntrins);


		//colorVec.resize(width * height * 4);
		//frameData[0].colorAlignedToDepthMap->read(colorVec.data());


		


	}



	void Frame::update() const
	{

		frameData[0].depthMap->mipmap();
		frameData[0].vertexMap->mipmap();
		frameData[0].normalMap->mipmap();

		/*for (int lv = 0; lv < downSampling.size(); ++lv)
		{
			std::dynamic_pointer_cast<rgbd::DownSampling>(downSampling[lv])->execute(frameData[lv].depthMap, frameData[lv + 1].depthMap, MAP_TYPE::DEPTH);
			std::dynamic_pointer_cast<rgbd::DownSampling>(downSampling[lv])->execute(frameData[lv].vertexMap, frameData[lv + 1].vertexMap, MAP_TYPE::VERTEX);
			std::dynamic_pointer_cast<rgbd::DownSampling>(downSampling[lv])->execute(frameData[lv].normalMap, frameData[lv + 1].normalMap, MAP_TYPE::NORMAL);
		}*/
	}

	void Frame::clearAll()
	{
		frameData[0].colorMap->update(NULL);

		for (int lv = 0; lv < frameData.size(); ++lv)
		{
			frameData[lv].depthMap->update(NULL);
			frameData[lv].vertexMap->update(NULL);
			frameData[lv].normalMap->update(NULL);
		}

		shortDepthMap->update(NULL);
		rawDepthMap->update(NULL);
		std::cout << "Frames cleared" << std::endl;
	}

	int Frame::getWidth(int lv) const
	{
		return width / int(pow(2, lv));
	}

	int Frame::getHeight(int lv) const
	{
		return height / int(pow(2, lv));
	}



	gl::Texture::Ptr Frame::getColorMap(int lv) const
	{
		return frameData[lv].colorMap;
	}
	gl::Texture::Ptr Frame::getColorPreviousMap(int lv) const
	{
		return frameData[lv].colorPreviousMap;
	}
	
	gl::Texture::Ptr Frame::getColorFilteredMap(int lv) const
	{
		return frameData[lv].colorFilteredMap;
	}

	gl::Texture::Ptr Frame::getColorAlignedToDepthMap(int lv) const
	{
		return frameData[lv].colorAlignedToDepthMap;
	}

	gl::Texture::Ptr Frame::getDepthMap(int lv) const
	{
		return frameData[lv].depthMap;
	}

	gl::Texture::Ptr Frame::getDepthPreviousMap(int lv) const
	{
		return frameData[lv].depthPreviousMap;
	}

	gl::Texture::Ptr Frame::getVertexMap(int lv) const
	{
		return frameData[lv].vertexMap;
	}

	gl::Texture::Ptr Frame::getVirtualVertexMap(int lv) const
	{
		return frameData[lv].virtualVertexMap;
	}

	gl::Texture::Ptr Frame::getVertexPreviousMap(int lv) const
	{
		return frameData[lv].vertexPreviousMap;
	}

	gl::Texture::Ptr Frame::getNormalMap(int lv) const
	{
		return frameData[lv].normalMap;
	}

	gl::Texture::Ptr Frame::getVirtualNormalMap(int lv) const
	{
		return frameData[lv].virtualNormalMap;
	}

	gl::Texture::Ptr Frame::getTrackMap() const
	{
		return trackMap;
	}

	gl::Texture::Ptr Frame::getTestMap() const
	{
		return testMap;
	}

	gl::Texture::Ptr Frame::getInfraMap() const
	{
		return infraMap;
	}

	gl::Texture::Ptr Frame::getMappingC2DMap() const
	{
		return mappingC2DMap;
	}
	gl::Texture::Ptr Frame::getMappingD2CMap() const
	{
		return mappingD2CMap;
	}

	void Frame::reset()
	{
		depthCount = 0;
	}
}