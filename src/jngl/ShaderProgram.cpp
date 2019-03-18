// Copyright 2018 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "ShaderProgram.hpp"

#include "../Shader_Impl.hpp"

#include <cassert>
#include <string>

namespace jngl {

int ShaderProgram::Context::referenceCount = 0;
const ShaderProgram::Impl* ShaderProgram::Context::activeImpl = nullptr;

struct ShaderProgram::Impl {
	GLuint id;
};

ShaderProgram::ShaderProgram(const Shader& vertex, const Shader& fragment)
: impl(std::make_unique<Impl>()) {
	impl->id = glCreateProgram();
	glAttachShader(impl->id, vertex.impl->id);
	glAttachShader(impl->id, fragment.impl->id);
	glLinkProgram(impl->id);
	GLint status = GL_FALSE;
	glGetProgramiv(impl->id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[2048];
		glGetProgramInfoLog(impl->id, sizeof(buffer), nullptr, buffer);
		throw std::runtime_error(buffer);
	}
	const auto tmp = use();
	glUniformMatrix4fv(getUniformLocation("projection"), 1, GL_TRUE, &opengl::projection.a[0][0]);
}

ShaderProgram::Context ShaderProgram::use() const {
	return Context(*impl);
}

int ShaderProgram::getAttribLocation(const std::string& name) const {
	int location = glGetAttribLocation(impl->id, name.c_str());
	assert(location != -1);
	return location;
}

int ShaderProgram::getUniformLocation(const std::string& name) const {
	int location = glGetUniformLocation(impl->id, name.c_str());
	assert(location != -1);
	return location;
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(impl->id);
}

ShaderProgram::Context::Context(const ShaderProgram::Impl& impl) {
	if (referenceCount > 0) {
		if (activeImpl != &impl) {
			throw std::runtime_error("A different ShaderProgram is already in use.");
		}
	} else {
		glUseProgram(impl.id);
	}
	++referenceCount;
	activeImpl = &impl;
}

ShaderProgram::Context::Context(Context&&) {
	++referenceCount;
}

ShaderProgram::Context::~Context() {
	--referenceCount;
	assert(referenceCount >= 0);
}

void ShaderProgram::Context::setUniform(const int location, const float v0, const float v1) {
	glUniform2f(location, v0, v1);
}

} // namespace jngl
