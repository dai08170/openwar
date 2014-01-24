// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"
#include "Gesture.h"
#include <glm/gtc/matrix_transform.hpp>



Surface::Surface(glm::vec2 size, float pixelDensity) :
_gc(nullptr)
{
	_gc = new graphicscontext(pixelDensity);
	SetSize(size);
}


Surface::~Surface()
{
}


void Surface::UseViewport()
{
	glm::vec2 size = GetSize() * _gc->get_pixeldensity();
	glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
}


void Surface::RenderSurface()
{
	glm::vec2 size = GetSize();
	glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);

	glClearColor(1, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 transform;
	transform = glm::translate(transform, glm::vec3(-1, -1, 0));
	transform = glm::scale(transform, glm::vec3(2.0f / size, 1.0f));

	Container::Render(transform);
}


bool Surface::ShowContextualMenu(glm::vec2 position)
{
	return false;
}


void Surface::MouseEnter(glm::vec2 position)
{
}


void Surface::MouseHover(glm::vec2 position)
{
}


void Surface::MouseLeave(glm::vec2 position)
{
}
