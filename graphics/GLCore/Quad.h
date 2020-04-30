#pragma once

#include <iostream>
#include <map>

#include "ComputeShader.h"


#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GLCore/Buffer.h"
#include "GLCore/Texture.h"
#include "GLCore/VertexArray.h"

namespace gl
{
	class Quad
	{
	private:
		gl::VertexBuffer<GLfloat> vertices;
		gl::IndexBuffer<GLuint> indices;
		gl::VertexArray vao;

		std::map<std::string, const gl::Shader::Ptr> progs;


	public:
		Quad();
		~Quad();

		void loadShaders(std::string pathToShaders);
		void setParams(int renderOpts, glm::vec2 depthRange, int level, float magMulti);

		void render(gl::Texture::Ptr tex);
		void renderMulti(
			gl::Texture::Ptr depthTex, 
			gl::Texture::Ptr normalTex, 
			gl::Texture::Ptr colorTex, 
			gl::Texture::Ptr infraTex,
			gl::Texture::Ptr mappingTex,
			gl::Texture::Ptr flowTex
		);

	};
}