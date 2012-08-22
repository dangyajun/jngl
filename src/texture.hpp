/*
Copyright 2010-2012 Jan Niklas Hasse <jhasse@gmail.com>
For conditions of distribution and use, see copyright notice in LICENSE.txt
*/

#pragma once

#include "opengl.hpp"

#include <vector>
#include <boost/noncopyable.hpp>

class Texture : boost::noncopyable {
public:
	Texture(int imgWidth, int imgHeight, GLubyte** rowPointers, GLenum format = GL_RGBA, int channels = 4);
	~Texture();
	void Bind() const;
	void Draw() const;
	void drawClipped(float xstart, float xend, float ystart, float yend) const;
	GLuint GetID() const;
private:
	static bool useVBO_;
	GLuint texture_;
	GLuint vertexBuffer_;
	std::vector<opengl::CoordType> texCoords_;
	std::vector<GLfloat> vertexes_;
};