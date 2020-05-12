#pragma once

#include "ComputeShader.h"
#include "glhelper.h"

namespace rgbd
{
	class Undistort : public ComputeShader
	{
	private:
		const gl::Shader::Ptr prog;

	public:
		Undistort(
			const gl::Shader::Ptr prog
		);
		void execute(
			gl::Texture::Ptr vertexMap,
			gl::Texture::Ptr dstDepthMap,
			float depthScale,
			glm::vec4 cam
		);
	};
}