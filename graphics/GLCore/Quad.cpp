#include "Quad.h"

namespace gl
{
	Quad::Quad()
	{
		std::vector<GLfloat> vert =
		{
			+1.0f, +1.0f, 0.0f, /* Upper right */ -1.0f, +1.0f, 0.0f, /* Upper left */
			-1.0f, -1.0f, 0.0f, /* Lower left */  +1.0f, -1.0f, 0.0f, /* Lower right */
			1.0f, 1.0f, /* Upper right */ 0.0f, 1.0f, /* Upper left */
			0.0f, 0.0f, /* Lower left */  1.0f, 0.0f  /* Lower right */
		};
		std::vector<GLshort> index =
		{
			0, 1, 2, 3
		};

		vertices.create(vert.data(), (int)vert.size(), GL_STATIC_READ);
		vao.addVertexAttrib(0, 3, vertices, 0, 0);
		vao.addVertexAttrib(1, 2, vertices, 0, (void *)(sizeof(GLfloat) * 3 * 4));

		indices.create(index.data(), (int)index.size(), GL_STATIC_READ);
		vao.bind();
		indices.bind();
		vao.unbind();
	}

	Quad::~Quad()
	{
	}

	void Quad::loadShaders(std::string path) {
		progs.insert(std::make_pair("ScreenQuad", std::make_shared<gl::Shader>(path + "ScreenQuad.vert", path + "ScreenQuad.frag")));

		progs["ScreenQuad"]->setUniform("isYFlip", 1);
		progs["ScreenQuad"]->setUniform("maxDepth", 3);

		progs["ScreenQuad"]->setUniform("maxDepth", 10.0f);
		progs["ScreenQuad"]->setUniform("renderType", 0);
		progs["ScreenQuad"]->setUniform("flowType", 0);

	}

	void Quad::setParams(int renderOpts, glm::vec2 depthRange, int level, float magMulti) {
		progs["ScreenQuad"]->setUniform("renderOptions", renderOpts);
		progs["ScreenQuad"]->setUniform("depthRange", depthRange);
		progs["ScreenQuad"]->setUniform("level", level);
		progs["ScreenQuad"]->setUniform("magMulti", magMulti);

	}

	void Quad::renderMulti(gl::Texture::Ptr depthTex, gl::Texture::Ptr normalTex, gl::Texture::Ptr colorTex, gl::Texture::Ptr infraTex, gl::Texture::Ptr mappingTex, gl::Texture::Ptr flowTex)
	{
		glDisable(GL_DEPTH_TEST);

		progs["ScreenQuad"]->use();

		vao.bind();
		depthTex->use(0);
		normalTex->use(1);
		colorTex->use(2);
		infraTex->use(3);
		mappingTex->use(4);
		flowTex->use(5);

		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);
		vao.unbind();

		progs["ScreenQuad"]->disuse();
		glEnable(GL_DEPTH_TEST);

	}

	void Quad::render(gl::Texture::Ptr tex)
	{
		glDisable(GL_DEPTH_TEST);

		progs["ScreenQuad"]->use();

		vao.bind();
		tex->use();
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);
		vao.unbind();
		progs["ScreenQuad"]->disuse();
		glEnable(GL_DEPTH_TEST);

	}
}