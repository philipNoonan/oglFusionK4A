#include "gDisOptFlow.h"

void gDisOptFlow::compileAndLinkShader()
{
	try {

	disFlowProg.compileShader("shaders/disFlow.cs");
	disFlowProg.link();
	  
	sobelProg.compileShader("shaders/sobelEdge.cs");
	sobelProg.link(); 
	 
	variRefineProg.compileShader("shaders/variRefine.cs"); 
	variRefineProg.link();  
	            
	}            
	catch (GLSLProgramException &e) {        
		std::cerr << e.what() << std::endl;           
		exit(EXIT_FAILURE);          
	}                                                 
}                                                                          
void gDisOptFlow::setLocations()                                          
{                        
	//sobel
	m_subroutine_SobelID = glGetSubroutineUniformLocation(sobelProg.getHandle(), GL_COMPUTE_SHADER, "launchSubroutine");
	m_getGradientsID = glGetSubroutineIndex(sobelProg.getHandle(), GL_COMPUTE_SHADER, "getGradients");
	m_getSmoothnessID = glGetSubroutineIndex(sobelProg.getHandle(), GL_COMPUTE_SHADER, "getSmoothness");
	m_imageType_cov_ID = glGetUniformLocation(sobelProg.getHandle(), "imageType");
	m_level_cov_ID = glGetUniformLocation(sobelProg.getHandle(), "level");
	// disflow
	m_subroutine_DISflowID = glGetSubroutineUniformLocation(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "launchSubroutine");
	m_makePatchesID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "makePatches");
	m_makePatchesHorID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "makePatchesHor");
	m_makePatchesVerID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "makePatchesVer");
	m_trackID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "track");

	m_patchInverseSearchID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "patchInverseSearch");
	m_patchInverseSearchDescentID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "patchInverseSearchDescent");

	m_densificationID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "densification");

	//m_prefixSum2D_HorID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "prefixSum2D_hor");
	//m_prefixSum2D_VerID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "prefixSum2D_ver");
	//patchInverseSearch_fwd1ID = glGetSubroutineIndex(disFlowProg.getHandle(), GL_COMPUTE_SHADER, "patchInverseSearch_fwd1");
	 
	m_patch_sizeID = glGetUniformLocation(disFlowProg.getHandle(), "patch_size");
	m_patch_strideID = glGetUniformLocation(disFlowProg.getHandle(), "patch_stride");
	m_level_dis_ID = glGetUniformLocation(disFlowProg.getHandle(), "level");
	m_iter_dis_ID = glGetUniformLocation(disFlowProg.getHandle(), "iter");
	m_trackWidthID = glGetUniformLocation(disFlowProg.getHandle(), "trackWidth");
	m_texSizeID = glGetUniformLocation(disFlowProg.getHandle(), "texSize");
	m_imageType_dis_ID = glGetUniformLocation(disFlowProg.getHandle(), "imageType");

	// variref
	m_subroutine_variRefineID = glGetSubroutineUniformLocation(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "launchSubroutine");
	m_prepareBuffersID = glGetSubroutineIndex(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "prepareBuffers");
	m_computeDataTermID = glGetSubroutineIndex(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "computeDataTerm");
	m_computeSmoothnessTermID = glGetSubroutineIndex(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "computeSmoothnessTerm");

	m_computeSORID = glGetSubroutineIndex(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "computeSOR");
	m_resizeID = glGetSubroutineIndex(variRefineProg.getHandle(), GL_COMPUTE_SHADER, "resize");

	
	m_level_var_ID = glGetUniformLocation(variRefineProg.getHandle(), "level");
	m_flipflopID = glGetUniformLocation(variRefineProg.getHandle(), "flipflop");
	m_iter_var_ID = glGetUniformLocation(variRefineProg.getHandle(), "iter");

	glGenQueries(1, timeQuery);

	variational_refinement_iter = 5;
	variational_refinement_alpha = 20.f;
	variational_refinement_gamma = 10.f;
	variational_refinement_delta = 5.f;

	/* Use separate variational refinement instances for different scales to avoid repeated memory allocation: */
	int max_possible_scales = 10;
	/*for (int i = 0; i < max_possible_scales; i++)
		variational_refinement_processors.push_back(cv::optflow::createVariationalFlowRefinement());*/






	     

	 

	 

	  
} 
 
void gDisOptFlow::setTexture(unsigned char * imageArray)
{
	//I1im = cv::Mat(m_texture_height, m_texture_width, CV_8UC4, imageArray);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureI1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RGBA, GL_UNSIGNED_BYTE, imageArray);
	glGenerateMipmap(GL_TEXTURE_2D); 

	//theErr = glGetError(); 
	glMemoryBarrier(GL_ALL_BARRIER_BITS); 




	if (firstFrame)  
	{ 

		glCopyImageSubData(m_textureI1, GL_TEXTURE_2D, 0, 0, 0, 0, 
							m_textureI0, GL_TEXTURE_2D, 0, 0, 0, 0,
							m_texture_width, m_texture_height, 1);
		firstFrame = false; 

		//I1im.copyTo(I0im);
	}   

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureI0); 
	glGenerateMipmap(GL_TEXTURE_2D);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	  
	//cv::Mat col = cv::Mat(m_texture_height / 2, m_texture_width / 2, CV_8UC4);
	////  

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI1);
	//glGetTexImage(GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE, col.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//

	//////theErr = glGetError();
	////   
	//cv::imshow("colo1", col);

	//cv::Mat col0 = cv::Mat(m_texture_height / 2, m_texture_width / 2, CV_8UC4);
	//// 

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI0);
	//glGetTexImage(GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE, col0.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//

	//////theErr = glGetError();
	////   
	//cv::imshow("colo0", col0);


	//cv::Mat col = cv::Mat(m_texture_height, m_texture_width, CV_8UC4);


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI1);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, col.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//
	 
	//cv::imshow("colo", col);
	  

}     
        

void gDisOptFlow::setTexture(float * imageArray)
{

	//I1im = cv::Mat(m_texture_height, m_texture_width, CV_32FC1, imageArray);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureI1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RED, GL_FLOAT, imageArray);
	glGenerateMipmap(GL_TEXTURE_2D);

	//theErr = glGetError(); 
	glMemoryBarrier(GL_ALL_BARRIER_BITS);




	if (firstFrame)
	{

		glCopyImageSubData(m_textureI1, GL_TEXTURE_2D, 0, 0, 0, 0,
			m_textureI0, GL_TEXTURE_2D, 0, 0, 0, 0,
			m_texture_width, m_texture_height, 1);
		firstFrame = false;

		//I1im.copyTo(I0im);

	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureI0);
	glGenerateMipmap(GL_TEXTURE_2D);

	glMemoryBarrier(GL_ALL_BARRIER_BITS);



	//cv::Mat col = cv::Mat(m_texture_height / 2, m_texture_width / 2, CV_8UC4);
	////  

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI1);
	//glGetTexImage(GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE, col.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
	//

	//////theErr = glGetError();
	////   
	//cv::imshow("colo1", col);

	//cv::Mat col0 = cv::Mat(m_texture_height / 2, m_texture_width / 2, CV_8UC4);
	//// 

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI0);
	//glGetTexImage(GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE, col0.data);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//////theErr = glGetError();
	////   
	//cv::imshow("colo0", col0);


	//cv::Mat col = cv::Mat(m_texture_height, m_texture_width, CV_32FC1);


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureI1);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, col.data);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//cv::imshow("colo", col);


}

 
   
GLuint gDisOptFlow::createTexture(GLenum target, int levels, int w, int h, int d, GLuint internalformat)
{
	GLuint texid;
	glGenTextures(1, &texid);  
	glBindTexture(target, texid);

	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// https://stackoverflow.com/questions/15405869/is-gltexstorage2d-imperative-when-auto-generating-mipmaps
	//glTexImage2D(target, 0, internalformat, w, h, 0, format, type, 0); // cretes mutable storage that requires glTexImage2D

	if (target == GL_TEXTURE_1D) 
	{   
		glTexStorage1D(target, levels, internalformat, w); 
	}  
	else if (target == GL_TEXTURE_2D)  
	{
		glTexStorage2D(target, levels, internalformat, w, h); // creates immutable storage and requires glTexSubImage2D

	}  
	else if (target == GL_TEXTURE_3D || d > 0) 
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexStorage3D(target, levels, internalformat, w, h, d);  
	} 
	return texid;      
}            
     
void gDisOptFlow::allocateBuffers()  
{ 
	// awesome BUFFER OBJECT
	m_refinementDataTerms.resize(m_texture_width * m_texture_height * 5, 0.25f); // a11, a12, a22, b1, b2
	size_t refineDataTermsSize = m_texture_width * m_texture_height * 5 * sizeof(float);

	glGenBuffers(1, &m_buffer_refinement_data_terms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer_refinement_data_terms);

	glBufferData(GL_SHADER_STORAGE_BUFFER, refineDataTermsSize, m_refinementDataTerms.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_buffer_refinement_data_terms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

	 
	      
	 
	// tracking buffer
	m_trackedPoints.resize(m_texture_height * m_texture_height * 2); 
	int xOffTrack = m_texture_height / 2; 
	int yOffTrack = m_texture_height / 2;
	int xSpacing = 2 * xOffTrack / m_texture_height;
	int ySpacing = 2 * yOffTrack / m_texture_height;
	   
	for (int i = 0; i < m_texture_height * 2; i += 2)
	{
		for (int j = 0; j < m_texture_height; j++)
		{
			m_trackedPoints[j * m_texture_height * 2 + i] = (m_texture_width >> 1) - xOffTrack + (i / 2) * xSpacing;
			m_trackedPoints[j * m_texture_height * 2 + i + 1] = (m_texture_height >> 1) - yOffTrack + j * ySpacing;

		}  
	}     
	     
	glGenBuffers(1, &m_trackedPointsBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_trackedPointsBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, m_trackedPointsBuffer);
	    
}  
    
          
void gDisOptFlow::allocateTextures(bool useInfrared) 
{
	// WITH OR WITHOUT THIS HINT, MIP MAPPING (at least the rg32f images) IS THE EXACT SAME AS cv::resize cv::INTER_AREA
	//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

	// the incoming texture should have been pre-mipmaped, but all other textures need to be allocated and wiped each frame
	if (useInfrared)
	{
		m_textureI0 = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_R32F);
		m_textureI1 = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_R32F);
	}
	else
	{
		m_textureI0 = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);
		m_textureI1 = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);
	}


	m_textureI0_prod_xx_yy_xy_aux = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width / m_patch_stride, m_texture_height , 0, GL_RGBA32F);
	m_textureI0_sum_x_y_aux = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width / m_patch_stride, m_texture_height, 0, GL_RG32F);

	m_textureI0_prod_xx_yy_xy = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width / m_patch_stride, m_texture_height / m_patch_stride, 0, GL_RGBA32F);
	m_textureI0_sum_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width / m_patch_stride, m_texture_height / m_patch_stride, 0, GL_RG32F);

	// all mip maps   
	    
	m_textureI0_grad_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);
	 
	m_textureS_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width / m_patch_stride, m_texture_height / m_patch_stride, 0, GL_RG32F);
	glBindTexture(GL_TEXTURE_2D, m_textureS_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width / m_patch_stride, m_texture_height / m_patch_stride, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	m_textureU_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);
	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D); 

	m_texture_init_U_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);
	glBindTexture(GL_TEXTURE_2D, m_texture_init_U_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	m_texture_previous_U_x_y = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);

	       
	m_textureWarp_I1 = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);
	  
	      
	// variational refine  

	//m_textureI_warp = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);
	//m_textureI_mix = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);
	//m_textureI_diff = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RGBA8);

	m_textureI_mix_diff_warp = createTexture(GL_TEXTURE_2D_ARRAY, m_numLevels, m_texture_width, m_texture_height, 3, GL_RGBA8);

	//m_textureI_warp_grad_x_y = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RG16I);
	//m_textureI_mix_grad_x_y = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RG16I);
	//m_textureI_diff_grad_x_y = createTexture(GL_TEXTURE_2D, numLevels, m_texture_width, m_texture_height, 0, GL_RG16I);

	m_textureI_grads_mix_diff_x_y = createTexture(GL_TEXTURE_2D_ARRAY, m_numLevels, m_texture_width, m_texture_height, 2, GL_RG32F);
	m_textureI_second_grads_mix_diff_x_y = createTexture(GL_TEXTURE_2D_ARRAY, m_numLevels, m_texture_width, m_texture_height, 2, GL_RG32F);

	m_texture_dup_dvp = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RGBA32F);
	//m_texture_smoothness_ux_uy_vx_vy = createTexture(GL_TEXTURE_2D_ARRAY, numLevels, m_texture_width, m_texture_height, 2, GL_RG16I);
	m_texture_total_flow = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);

	m_texture_smoothness_weight = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_R32F);
	m_texture_smoothness_terms = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_RG32F);


	m_texture_temp = createTexture(GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_R16I);
	m_texture_temp1 = createTexture(GL_TEXTURE_2D, 1, m_texture_width, m_texture_height, 0, GL_R16I);
	m_textureTest = createTexture(GL_TEXTURE_2D, m_numLevels, m_texture_width, m_texture_height, 0, GL_R32F);
	   
	   
}   
   
void gDisOptFlow::wipeFlow()
{
	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_texture_total_flow);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_texture_init_U_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_textureTest);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width, m_texture_height, GL_RED, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, m_textureS_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width / m_patch_stride, m_texture_height / m_patch_stride, GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindBuffer(GL_ARRAY_BUFFER, m_trackedPointsBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data());


}    

void gDisOptFlow::clearPoints()
{
	//glBindBuffer(GL_ARRAY_BUFFER, m_trackedPointsBuffer);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, m_trackedPoints.size() * sizeof(float), m_trackedPoints.data());
}

void gDisOptFlow::computeSobel(int level, bool useInfrared)
{ 
	sobelProg.use(); 
	glUniform1i(m_level_cov_ID, level);

	
	if (useInfrared)
	{
		//glBindImageTexture(0, m_textureI0, level, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureI0);
		glUniform1i(m_imageType_cov_ID, 4); // image type 0 = color rgba8ui 
	}
	else
	{
		glBindImageTexture(0, m_textureI0, level, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glUniform1i(m_imageType_cov_ID, 0); // image type 0 = color rgba8ui 
	}




	glBindImageTexture(4, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	
	int compWidth; 
	int compHeight;
	  
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getGradientsID);

	compWidth = divup(m_texture_width >> level, 32); // right bitshift does division by powers of 2
	compHeight = divup(m_texture_height >> level, 32);

	  
	  
	glDispatchCompute(compWidth, compHeight, 1);   
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	     
	if (level == 0)  
	{  
//		cv::Mat sx = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);
//		 
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, m_textureI0_grad_x_y);
//		glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sx.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
//		glBindTexture(GL_TEXTURE_2D, 0);
//		
//		 
//		cv::Mat image00[2]; 
//		cv::split(sx, image00); 
//		    
//		  
//		cv::imshow("ssss0", image00[0] / 2 + 0.5);
//		cv::imshow("ssss1", image00[1] / 2 + 0.5);  
//
//		cv::Mat I0xs = cv::Mat(1080 >> level, 1920 >> level, CV_16SC1);
//		cv::Mat I0ys = cv::Mat(1080 >> level, 1920 >> level, CV_16SC1);
//		cv::Mat greyI0;
//
//		cv::Mat I1xs = cv::Mat(1080 >> level, 1920 >> level, CV_16SC1);
//		cv::Mat I1ys = cv::Mat(1080 >> level, 1920 >> level, CV_16SC1);
//		cv::Mat greyI1;
////		 
//		cv::cvtColor(I0im, greyI0, CV_RGBA2GRAY); 
//		cv::cvtColor(I1im, greyI1, CV_RGBA2GRAY); 
////
//		cv::spatialGradient(greyI0, I0xs, I0ys); 
//
//		cv::imshow("sx", I0xs * 128);
//		cv::imshow("sy", I0ys * 128);
		 
///*
//		cv::Scharr(greyI0, I0xs, CV_16S, 1, 0);
//		cv::Scharr(greyI0, I0xs, CV_16S, 1, 0);
//
//		cv::Mat I0xs_grad;
//		cv::convertScaleAbs(I0xs, I0xs_grad);
//		cv::imshow("gryxx", I0xs_grad); 
//
//		cv::spatialGradient(greyI1, I1xs, I1ys);
//		cv::imshow("gryxasdasdasd", I1xs);
//*/
//		I0xs.convertTo(I0xs, CV_32FC1); 
//
//		cv::imshow("difffs0", (image00[0] - I0xs));   
//
//		double min, max;
//		cv::minMaxLoc(image00[0], &min, &max); 
//
//		double min1, max1;
//		cv::minMaxLoc(I0xs, &min1, &max1);
//
//		cv::Scalar mean0 = cv::mean(I0xs);
//		cv::Scalar mean1 = cv::mean(image00[0]);
//
//		std::cout << "mean0 : " << mean0[0] << " mean 1 :" << mean1[0] << " rato : " << mean1[0] / mean0[0] << " min : " << min << " max : " << max << " min : " << min1 << " max : " << max1 << " ratio : " << max / max1 << std::endl;
//

	} 




	//else 
	if (level == 0)
	{
		//cv::Mat sx = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_textureI0_grad_x_y);
		//glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sx.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
		//glBindTexture(GL_TEXTURE_2D, 0);
		//;
	 //
		//cv::Mat image11[2];
		//cv::split(sx, image11);


		//cv::imshow("s1x", image11[0] / 2 + 0.5);
		//cv::imshow("s1y", image11[1] / 2 + 0.5);
   
	}    
	//else if (level == 6)
	//{
	//	cv::Mat sx = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_16SC2);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_textureI0_grad_x_y);
	//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG_INTEGER, GL_SHORT, sx.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	;

	//	cv::Mat image112[2];
	//	cv::split(sx, image112);

	//	//sx.convertTo(sx, CV_8U, ); 

	//	cv::imshow("s2x", image112[0]);
	//	cv::imshow("s2y", image112[1]);



	//}   
	//   
	// 
	  
	     
	        
}       
    
void gDisOptFlow::makePatches(int level)
{
	//MAKE PATCHES
	disFlowProg.use(); 
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_makePatchesID);

	glBindImageTexture(0, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(4, m_textureI0_prod_xx_yy_xy, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(5, m_textureI0_sum_x_y, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	  
	     
	int compWidth, compHeight;
	compWidth = divup(m_texture_width >> level, 16); 
	compHeight = divup(m_texture_height >> level, 16); 
	     
	glDispatchCompute(compWidth, compHeight, 1);
	 
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); 
	  

	////MAKE PATCHES 
	//disFlowProg.use();  
	//glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_makePatchesHorID);

	//glBindImageTexture(0, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG16I);
	//glBindImageTexture(4, m_textureI0_prod_xx_yy_xy_aux, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//glBindImageTexture(5, m_textureI0_sum_x_y_aux, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);


	//int compWidth, compHeight;


	//compWidth = 1;
	//compHeight = divup(m_texture_height >> level, 4);
	// 

	//glDispatchCompute(compWidth, compHeight, 1);

	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);



	//glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_makePatchesVerID);

	//glBindImageTexture(0, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG16I);

	//glBindImageTexture(7, m_textureI0_prod_xx_yy_xy_aux, level, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	//glBindImageTexture(3, m_textureI0_sum_x_y_aux, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

	//glBindImageTexture(4, m_textureI0_prod_xx_yy_xy, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	//glBindImageTexture(5, m_textureI0_sum_x_y, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

	//compWidth = divup(m_texture_width >> level, 4);
	//compHeight = 1;

	//glDispatchCompute(compWidth, compHeight, 1);

	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	 
	 




	 




	 

	      
	     
	      
	        
	// if (level == 0)
	// {
	//	cv::Mat sxx = cv::Mat((m_texture_height >> level) / m_patch_stride, (m_texture_width >> level) / m_patch_stride, CV_32FC4);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_textureI0_prod_xx_yy_xy);
	//	glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_FLOAT, sxx.data);
	//	glBindTexture(GL_TEXTURE_2D, 0); 
	//	;
	//	  
	//	cv::Mat image0[4];     
	//	cv::split(sxx, image0);  

	//	cv::imshow("sflowx", image0[0] / 2.0 + 0.5); 
	//	cv::imshow("sflowy", image0[1] / 2.0 + 0.5);
	//	cv::imshow("sflowz", image0[2] / 2.0 + 0.5);

	//	cv::Mat sumxy = cv::Mat((m_texture_height >> level) / m_patch_stride, (m_texture_width >> level) / m_patch_stride, CV_32FC2);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_textureI0_sum_x_y);
	//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sumxy.data);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	;

	//	cv::Mat imsumxy[4]; 
	//	cv::split(sumxy, imsumxy);
	//	//cv::normalize(imsumxy[0], imsumxy[0], 0, 1, cv::NORM_MINMAX); 
	//	//cv::normalize(imsumxy[1], imsumxy[1], 0, 1, cv::NORM_MINMAX);

	//	cv::imshow("imsumx", imsumxy[0]);
	//	cv::imshow("imsumy", imsumxy[1]);
	//}    
	// else if (level == 1) 
	// {
	//	 cv::Mat sxx = cv::Mat((m_texture_height >> level) / m_patch_stride, (m_texture_width >> level) / m_patch_stride, CV_32FC4);

	//	 glActiveTexture(GL_TEXTURE0);
	//	 glBindTexture(GL_TEXTURE_2D, m_textureI0_prod_xx_yy_xy);
	//	 glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_FLOAT, sxx.data);
	//	 glBindTexture(GL_TEXTURE_2D, 0);
	//	 ;

	//	 cv::Mat image0[4];
	//	 cv::split(sxx, image0); 

	//	 cv::imshow("sflowx1", image0[0]);
	//	 cv::imshow("sflowy1", image0[1]);
	//	 cv::imshow("sflowz1", image0[2]);

	//	 cv::Mat sumxy = cv::Mat((m_texture_height >> level) / m_patch_stride, (m_texture_width >> level) / m_patch_stride, CV_32FC2);

	//	 glActiveTexture(GL_TEXTURE0);
	//	 glBindTexture(GL_TEXTURE_2D, m_textureI0_sum_x_y);
	//	 glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sumxy.data);
	//	 glBindTexture(GL_TEXTURE_2D, 0);
	//	 ;

	//	 cv::Mat imsumxy[4];
	//	 cv::split(sumxy, imsumxy);
	//	 //cv::normalize(imsumxy[0], imsumxy[0], 0, 1, cv::NORM_MINMAX); 
	//	 //cv::normalize(imsumxy[1], imsumxy[1], 0, 1, cv::NORM_MINMAX);

	//	 cv::imshow("imsumx1", imsumxy[0]);
	//	 cv::imshow("imsumy1", imsumxy[1]);

	//	  
  
	// }  
	   
}                          
                                
                      
bool gDisOptFlow::patchInverseSearch(int level, bool useInfrared)  
{            
	 	    
		disFlowProg.use();       
		             
		glUniform1i(m_level_dis_ID, level);  

		if (useInfrared)
		{
			glUniform1i(m_imageType_dis_ID, 0);
		}
		else
		{
			glUniform1i(m_imageType_dis_ID, 1);
		}

		//glUniform1i(m_imageFormatID);
		//glUniform1i(m_iter_dis_ID, outer_num_iter);     
		                   
		// INVERSE SEARCH 
		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_patchInverseSearchID);
		    
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, m_textureI0);       
		glActiveTexture(GL_TEXTURE1);      
		glBindTexture(GL_TEXTURE_2D, m_textureI1);        
		        
		                     
		glActiveTexture(GL_TEXTURE2);   
		glBindTexture(GL_TEXTURE_2D, m_textureI0_prod_xx_yy_xy);  
		glActiveTexture(GL_TEXTURE3);   
		glBindTexture(GL_TEXTURE_2D, m_textureI0_sum_x_y); 
		//   
		glActiveTexture(GL_TEXTURE4);   
		glBindTexture(GL_TEXTURE_2D, m_textureU_x_y); 

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_texture_init_U_x_y);

		glBindImageTexture(0, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		 
		glBindImageTexture(1, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);   
		glBindImageTexture(2, m_texture_init_U_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

		glBindImageTexture(4, m_textureI0_prod_xx_yy_xy, level, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(5, m_textureI0_sum_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

		glBindImageTexture(6, m_textureS_x_y, level, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
		 
		glBindImageTexture(3, m_textureTest, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
		   
		                    
		       
		int compWidth = divup(m_texture_width >> level, 16);   
		int compHeight = divup(m_texture_height >> level, 16);  

		glDispatchCompute(compWidth, compHeight, 1);    
		  
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  
		   
		  
		//if (level == 1 && outer_num_iter == 0)
		//{
		//	int offs = 0;
		//	cv::Mat sxx2s = cv::Mat(((m_texture_height / 4) >> level + offs), ((m_texture_width / 4) >> level + offs), CV_32FC2);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_textureS_x_y);
		//	glGetTexImage(GL_TEXTURE_2D, level + offs, GL_RG, GL_FLOAT, sxx2s.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	;

		//	cv::Mat image1s[2];
		//	cv::split(sxx2s, image1s);

		//	cv::imshow("qwsse", image1s[0]);
		//	cv::imshow("tyssu", image1s[1]);


		//	cv::Mat mag, ang;
		//	cv::Mat hsv_split[3], hsv;
		//	cv::Mat rgbs;
		//	cv::cartToPolar(image1s[0], image1s[1], mag, ang, true);
		//	cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
		//	hsv_split[0] = ang;
		//	hsv_split[1] = mag;
		//	hsv_split[2] = cv::Mat::ones(ang.size(), ang.type());
		//	cv::merge(hsv_split, 3, hsv);
		//	cv::cvtColor(hsv, rgbs, cv::COLOR_HSV2BGR);
		//	cv::imshow("flowsparse", rgbs);
		//} 
		    
		//disFlowProg.use(); 
		 
		//glUniform1i(m_level_dis_ID, level); 
		//glUniform1i(m_iter_dis_ID, outer_num_iter);

		//// INVERSE SEARCH 
		//glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_patchInverseSearchDescentID);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_textureI0);
		//glActiveTexture(GL_TEXTURE1);    
		//glBindTexture(GL_TEXTURE_2D, m_textureI1);


		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, m_textureI0_prod_xx_yy_xy);
		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, m_textureI0_sum_x_y);

		//glActiveTexture(GL_TEXTURE4);
		//glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);

		//glBindImageTexture(0, m_textureI0_grad_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

		//// init_U_u_x units are in top level pixels, so ach underlayers needs a /= 2^layer
		//glBindImageTexture(1, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		//// U_x_y units are in pixels for each layer
		//glBindImageTexture(2, m_texture_init_U_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

		//glBindImageTexture(4, m_textureI0_prod_xx_yy_xy, level, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		//glBindImageTexture(5, m_textureI0_sum_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);

		//glBindImageTexture(6, m_textureS_x_y, level, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);

		//glBindImageTexture(3, m_textureTest, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);



		//compWidth = divup(m_texture_width >> level, 16);
		//compHeight = divup(m_texture_height >> level, 16);

		//glDispatchCompute(compWidth, compHeight, 1);

		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		       
	//}              
	               
	                       
	                             
	//if (level == 0)
	//{

	//	 

	//	cv::Mat sxx2 = cv::Mat((m_texture_height >> level), (m_texture_width >> level), CV_32FC2);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
	//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sxx2.data);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	;

	//	cv::Mat image1[2];
	//	cv::split(sxx2, image1); 

	//	cv::imshow("qwe", image1[0]);
	//	cv::imshow("tyu", image1[1]);

	//	cv::Scalar mean0 = cv::mean(image1[1]);
	//	double max, min;
	//	cv::minMaxLoc(image1[1], &min, &max);
	//	std::cout << "y " << mean0[0] << " min : " << min << " max : " << max << std::endl;

	//	cv::minMaxLoc(image1[0], &min, &max);
	//	std::cout << "x " << mean0[0] << " min : " << min << " max : " << max << std::endl;

	//	cv::Mat mag, ang;
	//	cv::Mat hsv_split[3], hsv;
	//	cv::Mat rgb;
	//	cv::cartToPolar(image1[0], image1[1], mag, ang, true);
	//	cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
	//	hsv_split[0] = ang;
	//	hsv_split[1] = mag;
	//	hsv_split[2] = cv::Mat::ones(ang.size(), ang.type());
	//	cv::merge(hsv_split, 3, hsv);
	//	cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
	//	cv::imshow("flowsparse", rgb); 
 

//	}     
	                                        
	          
	if (level == 1)        
	{    
		  
		//cv::Mat tes = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC1);
		// 
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_textureTest);
		//glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, tes.data);
		//glBindTexture(GL_TEXTURE_2D, 0);   
		//; 
		//        
		////cv::Mat tesim[4]; 
		////cv::split(tes, tesim);    
		//  
		////cv::imshow("test1", tesim[0] * 1.0e10); 
		//cv::imshow("test2", tes);  
		//glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//double dmin, dmax;

		//cv::minMaxLoc(tes, &dmin, &dmax);

		//cv::Scalar dmean = cv::mean(tes);
  //
		//std::cout << "mean : " << dmean << " dmin : " << dmin << " dmax : " << dmax << std::endl;
		     
	}         
	      
	    
	return false;  
}   
             
  
bool gDisOptFlow::densification(int level)
{  

	disFlowProg.use(); 

	glUniform1i(m_level_dis_ID, level);

	// DENSIFICATION
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_densificationID);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, m_textureI0); 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureI1); 

	glBindImageTexture(6, m_textureS_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, m_textureU_x_y, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	    
	int compWidth = divup(m_texture_width >> level, 4);
	int compHeight = divup(m_texture_height >> level, 4);
	       
	glDispatchCompute(compWidth, compHeight, 1);  
	         
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);  
	          
	return false;
}   
//
//void gDisOptFlow::variationalRefinement(int level)
//{
//	cv::Mat I0imq = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_8UC4);
//	cv::Mat I1imq = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_8UC4);
//
//	glActiveTexture(GL_TEXTURE0); 
//	glBindTexture(GL_TEXTURE_2D, m_textureI0);
//	glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_UNSIGNED_BYTE, I0imq.data);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	; 
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_textureI1);
//	glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_UNSIGNED_BYTE, I1imq.data);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	;  
//
//	cv::Mat I0C1;
//	cv::cvtColor(I0imq, I0C1, CV_BGRA2GRAY);
//	
//	cv::Mat I1C1;
//	cv::cvtColor(I1imq, I1C1, CV_BGRA2GRAY);
//
//	cv::Mat sxx3 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);
//	cv::Mat sxx4 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, sxx3.data);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	;  
//	    
//	//cv::imshow("dens1", sxx3);  
//	      
//	cv::Mat image2[2];  
//	cv::split(sxx3, image2);  
//
//	 
//	variational_refinement_processors[0]->calcUV(I0C1, I1C1,
//		image2[0], image2[1]);
//
//	//cv::merge(image2, 2, sxx3);
//
//	////glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
//	////glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width >> level, m_texture_height >> level, GL_RG, GL_FLOAT, sxx3.ptr());
//	//  
//
//	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
//	//if (imageArray != NULL)
//	//{
//
//	cv::merge(image2, 2, sxx4);
//
//	glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, m_texture_width >> level, m_texture_height >> level, GL_RG, GL_FLOAT, sxx4.data);
//
//	if (level == 1)
//	{
//		cv::Mat mag, ang;
//		cv::Mat hsv_split[3], hsv;
//		cv::Mat rgb;
//		cv::cartToPolar(image2[0], image2[1], mag, ang, true);
//		cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
//		hsv_split[0] = ang;
//		hsv_split[1] = mag;
//		hsv_split[2] = cv::Mat::ones(ang.size(), ang.type());
//		cv::merge(hsv_split, 3, hsv);
//		cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
//		cv::imshow("flowvar", rgb);
//	}
//
//	 
//
//}

void gDisOptFlow::variRef(int level)
{
	glBindTexture(GL_TEXTURE_2D, m_texture_total_flow);
	glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, m_texture_width >> level, m_texture_height >> level, GL_RG, GL_FLOAT, zeroValues.data());

	glBindTexture(GL_TEXTURE_2D, m_texture_dup_dvp);
	glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, m_texture_width >> level, m_texture_height >> level, GL_RGBA, GL_FLOAT, zeroValues.data());

	
	flipflop = 0; 

	variRefineProg.use();

	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_prepareBuffersID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureI0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureI1); // CHANGE ME TO I1 EXT 

	glBindImageTexture(0, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	//glBindImageTexture(2, m_textureWarp_I1, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	 
	glBindImageTexture(2, m_textureI_mix_diff_warp, level, GL_TRUE, NULL, GL_WRITE_ONLY, GL_RGBA8);
	 
	glUniform1i(m_level_var_ID, level);

	int compWidth = divup(m_texture_width >> level, 32);
	int compHeight = divup(m_texture_height >> level, 32);

	glDispatchCompute(compWidth, compHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	 
	//if (level == 0)
	//{

	//	std::vector<uchar> vari_vec((m_texture_height >> level) * (m_texture_width >> level) * 4 * 3);
	//	cv::Mat vari_Mat = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_8UC4);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureI_mix_diff_warp);
	//	glGetTexImage(GL_TEXTURE_2D_ARRAY, level, GL_RGBA, GL_UNSIGNED_BYTE, vari_vec.data());// this is importnant, you are using GL_RED_INTEGETER!!!!
	//	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	//	;

	//	memcpy_s(vari_Mat.data, vari_vec.size() * sizeof(uchar) / 3, vari_vec.data() + vari_vec.size() * 1 / 3, vari_vec.size() * sizeof(uchar) / 3);

	//	cv::imshow("varima", vari_Mat);
	//} 
	 


	// SPIN UP THE SOBEL FOR ALL THESE LOVELY IMAGES

	sobelProg.use();

	glBindImageTexture(0, m_textureI_mix_diff_warp, level, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(4, m_textureI_grads_mix_diff_x_y, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getGradientsID);
	glUniform1i(m_imageType_cov_ID, 0); // image type 0 = color rgba8ui

	glDispatchCompute(compWidth, compHeight, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);



	glBindImageTexture(0, m_textureI_mix_diff_warp, level, GL_FALSE, 1, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(4, m_textureI_grads_mix_diff_x_y, level, GL_FALSE, 1, GL_WRITE_ONLY, GL_RG32F);

	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getGradientsID);
	glUniform1i(m_imageType_cov_ID, 0); // image type 0 = color rgba8ui

	glDispatchCompute(compWidth, compHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	 


	glBindImageTexture(1, m_textureI_grads_mix_diff_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(5, m_textureI_second_grads_mix_diff_x_y, level, GL_TRUE, NULL, GL_WRITE_ONLY, GL_RG32F);

	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getGradientsID);
	glUniform1i(m_imageType_cov_ID, 2); // image type 2 = color rg32f

	glDispatchCompute(compWidth, compHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	  

	for (int inner_iter = 0; inner_iter < 2; inner_iter++) // this is a problem, < 1 or < level + 1
	{

		sobelProg.use();

		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_getSmoothnessID);

		if (inner_iter == 0)
		{
			glBindImageTexture(1, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW
		}
		else
		{
			glBindImageTexture(1, m_texture_total_flow, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW

		}

		glBindImageTexture(6, m_texture_smoothness_weight, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		glUniform1i(m_imageType_cov_ID, 1); // image type 0 = color rgba8ui

		glDispatchCompute(compWidth, compHeight, 1);


		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


		 
		   

		variRefineProg.use();
		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_computeSmoothnessTermID);

		//	m_texture_smoothness_terms
		glBindImageTexture(5, m_texture_smoothness_weight, level, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		glBindImageTexture(6, m_texture_smoothness_terms, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);

		glDispatchCompute(compWidth, compHeight, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//if (level == 1 && inner_iter == 3) // it doesnt look like total flow is being updated, so on iter != 0, theres nothing
		//{
		//	cv::Mat smooth_0 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC1);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_texture_smoothness_weight);
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, smooth_0.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	;



		//	cv::imshow("smoo_weight", smooth_0);

		//	cv::Mat smooth_1 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_texture_smoothness_terms);
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, smooth_1.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	


		//	cv::Mat image00[2];
		//	cv::split(smooth_1, image00);

		//	//cv::imshow("smx_dif", image00[0] - image00[1]);
		//	cv::imshow("smx_x", image00[0]);
		//	cv::imshow("smx_y", image00[1]);

		//} 


		 
		//if (level == 0 && inner_iter == 1) // it doesnt look like total flow is being updated, so on iter != 0, theres nothing
		//{
		//	cv::Mat smooth_0 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC1);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_texture_smoothness_weight);
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, smooth_0.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	

		//	   
		//  
		//	cv::imshow("smoo_we", smooth_0);

		//	cv::Mat smooth_1 = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_texture_smoothness_terms); 
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, smooth_1.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	


		//	cv::Mat image00[2];
		//	cv::split(smooth_1, image00);

		//	//cv::imshow("smx_dif", image00[0] - image00[1]);
		//	cv::imshow("smx_y", image00[1]);

		//	//cv::Mat totflow = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);

		//	//glActiveTexture(GL_TEXTURE0);
		//	//glBindTexture(GL_TEXTURE_2D, m_texture_total_flow);
		//	//glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, totflow.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
		//	//glBindTexture(GL_TEXTURE_2D, 0);
		//	//

		//	//cv::Mat tofl[2];
		//	//cv::split(totflow, tofl);

		//	//cv::imshow("wwee", tofl[0] - tofl[1]);
		//	//cv::imshow("dwerwev", tofl[1]);

		//	//cv::Mat mag, ang;
		//	//cv::Mat hsv_split[3], hsv;
		//	//cv::Mat rgb;
		//	//cv::cartToPolar(tofl[0], tofl[1], mag, ang, true);
		//	//cv::normalize(mag, mag, 0, 100, cv::NORM_MINMAX);
		//	//hsv_split[0] = ang;
		//	//hsv_split[1] = mag; 
		//	//hsv_split[2] = cv::Mat::ones(ang.size(), ang.type());
		//	//cv::merge(hsv_split, 3, hsv);
		//	//cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
		//	//cv::imshow("flow123", rgb);

		 
		 
		//}   
		//  
		  
		  
		  
		variRefineProg.use();   
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_buffer_refinement_data_terms);

		glUniform1i(m_flipflopID, flipflop);    
		glUniform1i(m_iter_var_ID, inner_iter);  
		  
		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_computeDataTermID);
		//glBindImageTexture(0, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW 
		glBindImageTexture(1, m_texture_dup_dvp, level, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); // SHOULD THIS BE DELTA FLOW 
		//glBindImageTexture(7, m_texture_total_flow, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW
		glBindImageTexture(3, m_textureI_grads_mix_diff_x_y, level, GL_TRUE, NULL, GL_READ_ONLY, GL_RG32F);
		glBindImageTexture(4, m_textureI_second_grads_mix_diff_x_y, level, GL_TRUE, NULL, GL_READ_ONLY, GL_RG32F);
		


		glDispatchCompute(compWidth, compHeight, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);


		 
		   
		  
		variRefineProg.use(); 

		 
		  
		glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_computeSORID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_buffer_refinement_data_terms);

		glUniform1i(m_flipflopID, flipflop);
		glUniform1i(m_iter_var_ID, inner_iter);
		glUniform1i(m_level_var_ID, level);

		//	m_texture_smoothness_terms
		glBindImageTexture(0, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW

		glBindImageTexture(1, m_texture_dup_dvp, level, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(7, m_texture_total_flow, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW
		glBindImageTexture(6, m_texture_smoothness_terms, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
		
		 
		
		//glBindImageTexture(5, m_textureTest, level, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);


		glDispatchCompute(compWidth, compHeight, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		flipflop = !flipflop;
		 
		  

		//if (level == 0 && inner_iter == 0)
		//{

		//	cv::Mat tes = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC1);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_textureTest);
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, tes.data);
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	

		//	cv::imshow("test12", tes);



		//}

		//if (level == 1 && inner_iter == 1)
		//{

		   

		//	cv::Mat deltasuv = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC4);

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, m_texture_dup_dvp);
		//	glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_FLOAT, deltasuv.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
		//	glBindTexture(GL_TEXTURE_2D, 0);
		//	
		//	 
		//	cv::Mat imdel[4]; 
		//	cv::split(deltasuv, imdel); 
		//	  
		//	cv::imshow("dv0", imdel[0]);   
		//	cv::imshow("du0", imdel[1]);

		//	cv::imshow("dv1", imdel[2]); 
		//	cv::imshow("du1", imdel[3]);
		//	cv::waitKey(1);
		//}
		// 
		 
	}
	  
	  
	   
	 

	//// when level == 1 the bottom quater is exactly tyhe same, ie. R == G
	//if (level == 1)
	//{
	//	cv::Mat weight = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC1);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_texture_smoothness_weight);
	//	glGetTexImage(GL_TEXTURE_2D, level, GL_RED, GL_FLOAT, weight.data);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	
	 
	//	cv::imshow("weight", weight);


	//}


	   

	if (level == 0)
	{
		////cv::Mat deltasuv = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC4);


		////glActiveTexture(GL_TEXTURE0);
		////glBindTexture(GL_TEXTURE_2D, m_texture_dup_dvp);
		////glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_FLOAT, deltasuv.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
		////glBindTexture(GL_TEXTURE_2D, 0);
		////

		////cv::Mat imdel[4];
		////cv::split(deltasuv, imdel);

		////cv::imshow("dv", imdel[1]);  
		////cv::imshow("du", imdel[2]);

			//cv::Mat totflow = cv::Mat(m_texture_height >> level, m_texture_width >> level, CV_32FC2);

			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, m_texture_total_flow);
			//glGetTexImage(GL_TEXTURE_2D, level, GL_RG, GL_FLOAT, totflow.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
			//glBindTexture(GL_TEXTURE_2D, 0);
			// 

			//cv::Mat tofl[2];
			//cv::split(totflow, tofl);  

			////cv::imshow("wwee", tofl[0] - tofl[1]);
			////cv::imshow("dwerwev", tofl[1]); 

		 //  
			//cv::Mat mag, ang; 
			//cv::Mat hsv_split[3], hsv;
			//cv::Mat rgb; 
			//cv::cartToPolar(tofl[0], tofl[1], mag, ang, true);
			//cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);
			//hsv_split[0] = ang;
			//hsv_split[1] = mag;
			//hsv_split[2] = cv::Mat::ones(ang.size(), ang.type()); 
			//cv::merge(hsv_split, 3, hsv);
			//cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
			//cv::imshow("totflowrgb", rgb);

	} 
	 
	//if (level > 0)
	//{
	//	variRefineProg.use();
	//	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_resizeID);

	//	glBindImageTexture(7, m_texture_total_flow, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW
	//	glBindImageTexture(0, m_textureU_x_y, level - 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F); // SHOULD THIS BE DELTA FLOW

	//	glDispatchCompute(compWidth, compHeight, 1);
	//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//}
	//else
	//{
	glCopyImageSubData(m_texture_total_flow, GL_TEXTURE_2D, level, 0, 0, 0,
		m_textureU_x_y, GL_TEXTURE_2D, level, 0, 0, 0,
		m_texture_width >> level, m_texture_height >> level , 1);
	//}


	glMemoryBarrier(GL_ALL_BARRIER_BITS);


	 

	  
	    
}      

void gDisOptFlow::swapTextures() 
{
	// SWAP TEXTURES 
	glCopyImageSubData(m_textureI1, GL_TEXTURE_2D, 0, 0, 0, 0,
		m_textureI0, GL_TEXTURE_2D, 0, 0, 0, 0,
		m_texture_width, m_texture_height, 1);

}  
   
//void gDisOptFlow::resizeFlow(int level)
//{
//	disFlowProg.use();
//  
//	glUniform1i(m_level_dis_ID, level);
//  
//	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_resizeID); 
//
//	glBindImageTexture(2, m_textureU_x_y, level, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
//	glBindImageTexture(1, m_textureU_x_y, level - 1, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
//
//   
//	int compWidth = divup(m_texture_width >> level, 4);
//	int compHeight = divup(m_texture_height >> level, 4); 
// 
//	glDispatchCompute(compWidth, compHeight, 1);
//
//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//
//
//}   
    
bool gDisOptFlow::calc(bool useInfrared)
{   

	 
	glBeginQuery(GL_TIME_ELAPSED, timeQuery[0]);   
	  
	for (int level = 0; level <= m_numLevels; level++)
		computeSobel(level, useInfrared);

	glMemoryBarrier(GL_ALL_BARRIER_BITS); 
	   
	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texture_width ,m_texture_height , GL_RG, GL_FLOAT, zeroValues.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	  
	     
	            
	for (int level = m_numLevels - 1; level >= 0; level--) 
	{  

		//glBindTexture(GL_TEXTURE_2D, m_textureS_x_y);
		//glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, (m_texture_width / m_patch_stride) >> level,( m_texture_height / m_patch_stride) >> level, GL_RG, GL_FLOAT, zeroValues.data());
		
		makePatches(level);       
		patchInverseSearch(level, useInfrared);        
		densification(level);     
		if (level >= 0) // dont need to densify finest level?   
		{   
			//variationalRefinement(level); // opencv, slow

			variRef(level);  // mine, broken ish  slower   
			 
			                
		}                         
		              
		                
		                                     
	}                            
	glMemoryBarrier(GL_ALL_BARRIER_BITS); 

	  
	// COPY FLOW TOP LEVEL AND MAKE MIPMAPS FOR INITIAL FLOW
	//for (int level = 0; level < m_numLevels; level++)
	//{

	int level = 0;
		glCopyImageSubData(m_textureU_x_y, GL_TEXTURE_2D, level, 0, 0, 0,
			               m_texture_init_U_x_y, GL_TEXTURE_2D, level, 0, 0, 0,
			               m_texture_width >> level, m_texture_height >> level, 1);

		glBindTexture(GL_TEXTURE_2D, m_texture_init_U_x_y);
		glGenerateMipmap(GL_TEXTURE_2D);
		 

	//}  
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	 
	  

	glEndQuery(GL_TIME_ELAPSED);      

	GLuint available = 0;
	while (!available) {
		glGetQueryObjectuiv(timeQuery[0], GL_QUERY_RESULT_AVAILABLE, &available);
	}
	  
	// elapsed time in nanoseconds
	GLuint64 elapsed;
	glGetQueryObjectui64vEXT(timeQuery[0], GL_QUERY_RESULT, &elapsed);

	// ms 
	m_timeElapsed = elapsed / 1000000.0;
	if (swapCounter % 1 == 0)
		swapTextures(); 
	  
	//I1im.copyTo(I0im);

	swapCounter++; 
	  
	return false; 
}
   
bool gDisOptFlow::track()    
{   
	 
	disFlowProg.use();  
	glUniformSubroutinesuiv(GL_COMPUTE_SHADER, 1, &m_trackID); 
	
	glUniform1i(m_trackWidthID, m_texture_height * 2);
	 
	// bind flow texture 
	glActiveTexture(GL_TEXTURE6);

	glm::ivec2 imageSize(m_texture_width, m_texture_height);
	glUniform2iv(m_texSizeID, 1, glm::value_ptr(imageSize));

	
	glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);
	// bind ssbo for tracked points
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_trackedPointsBuffer);

	int compWidth = divup(m_texture_height, 4);
	int compHeight = divup(m_texture_height, 4);
	glDispatchCompute(compWidth, compHeight, 1);  
	 
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//std::vector<float> outputData(m_texture_height * m_texture_height *2);

	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_trackedPointsBuffer);
	//void *ptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//memcpy_s(outputData.data(), outputData.size() * sizeof(float), ptr, outputData.size() * sizeof(float));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	//cv::Mat blank = cv::Mat(424, 512, CV_8UC4);
	//for (int i = 0; i < m_texture_height * m_texture_height * 2; i += 2)
	//{

	//	cv::circle(blank, cv::Point2f(outputData[i], outputData[i + 1]), 2, cv::Scalar(255, 128, 128, 255));

	//}    

	//cv::imshow("tracked", blank);
	 

	 



	//cv::Mat blank = cv::Mat(1080, 1920, CV_8UC4);  
	  
	//if (swapCounter % 100 == 0)
	//{
	//	for (int i = 0; i < 100; i++)
	//	{
	//		for (int j = 0; j < 100; j++)
	//		{
	//			m_trackedPoints[j * 100 + i].x = (1920 >> 1) - 500 + (i * 10);
	//			m_trackedPoints[j * 100 + i].y = (1080 >> 1) - 500 + (j * 10);

	//		}
	//	}
	//}  
	 
	//cv::Mat totflow = cv::Mat(m_texture_height >> 0, m_texture_width >> 0, CV_32FC2);

	////glActiveTexture(GL_TEXTURE0);
	////glBindTexture(GL_TEXTURE_2D, m_texture_dup_dvp); // there is a problem with totral flow, the x and y comp are the same!!!!!
	////glGetTexImage(GL_TEXTURE_2D, 1, GL_RG, GL_FLOAT, totflow.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
	////glBindTexture(GL_TEXTURE_2D, 0);
	////

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureU_x_y); // there is a problem with totral flow, the x and y comp are the same!!!!!
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, totflow.data);// this is importnant, you are using GL_RED_INTEGETER!!!!
	//glBindTexture(GL_TEXTURE_2D, 0);
	//

	//float readFlow[2];  
	//glBindTexture(GL_TEXTURE_2D, m_textureU_x_y);

	//for (int i = 0; i < 10000; i++)
	//{

	//	cv::Point2f currFlow = totflow.at<cv::Point2f>(int(m_trackedPoints[i].y), int(m_trackedPoints[i].x));
	//	m_trackedPoints[i] += currFlow;

	//	m_trackedPoints[i].x = m_trackedPoints[i].x < 0 ? 0 : m_trackedPoints[i].x;
	//	m_trackedPoints[i].y = m_trackedPoints[i].y < 0 ? 0 : m_trackedPoints[i].y;

	//	m_trackedPoints[i].x = m_trackedPoints[i].x > blank.cols - 10 ? blank.cols - 10 : m_trackedPoints[i].x;
	//	m_trackedPoints[i].y = m_trackedPoints[i].y > blank.rows - 10 ? blank.rows - 10 : m_trackedPoints[i].y;

	//	cv::circle(blank, cv::Point2f(m_trackedPoints[i].x, m_trackedPoints[i].y), 2, cv::Scalar(255, 128, 128, 255));

	//}

	//cv::imshow("tracked", blank);


	return 1;

	 
}

