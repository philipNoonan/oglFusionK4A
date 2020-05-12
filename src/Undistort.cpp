#include "Undistort.h"

namespace rgbd
{
	Undistort::Undistort(
		const gl::Shader::Ptr prog
	) : prog(prog)
	{
	}

	void Undistort::execute(
		gl::Texture::Ptr vertexMap,
		gl::Texture::Ptr rawDepthMap,
		float depthScale,
		glm::vec4 cam
	)
	{
		prog->setUniform("depthScale", depthScale);
		prog->setUniform("cam", cam);

		prog->use();
		vertexMap->bindImage(0, 0, GL_READ_ONLY);
		rawDepthMap->bindImage(1, 0, GL_WRITE_ONLY);
		glDispatchCompute(GLHelper::divup(vertexMap->getWidth(), 32), GLHelper::divup(vertexMap->getHeight(), 32), 1);
		prog->disuse();
	}
}