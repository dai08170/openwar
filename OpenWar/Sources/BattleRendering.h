/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#ifndef BATTLERENDERING_H
#define BATTLERENDERING_H

#include "renderer.h"


struct BattleRendering
{
	struct texture_billboard_vertex
	{
		glm::vec3 _position;
		float _height;
		float _order;
		glm::vec2 _texcoord;
		glm::vec2 _texsize;

		texture_billboard_vertex(glm::vec3 p, float h, glm::vec2 tc, glm::vec2 ts, float b = 0.5) : _position(p), _height(h), _texcoord(tc), _texsize(ts)
		{
		}
	};

	struct texture_billboard_uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
		glm::vec3 _upvector;
		float _viewport_height;
		float _min_point_size;
		float _max_point_size;
	};

	struct color_billboard_vertex
	{
		glm::vec3 _position;
		glm::vec4 _color;
		float _height;

		color_billboard_vertex(glm::vec3 p, glm::vec4 c, float h) : _position(p), _color(c), _height(h)
		{
		}
	};

	struct color_billboard_uniforms
	{
		glm::mat4x4 _transform;
		glm::vec3 _upvector;
		float _viewport_height;
	};

	struct ground_color_uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;
		glm::vec4 _color;

		ground_color_uniforms() : _point_size(1) { }
	};

	struct ground_gradient_uniforms
	{
		glm::mat4x4 _transform;
		GLfloat _point_size;

		ground_gradient_uniforms() : _point_size(1) { }
	};

	struct ground_texture_uniforms
	{
		glm::mat4x4 _transform;
		const texture* _texture;
	};


	static renderer<texture_billboard_vertex, texture_billboard_uniforms>* _texture_billboard_renderer;
	static renderer<color_billboard_vertex, color_billboard_uniforms>* _color_billboard_renderer;

	static renderer<color_vertex3, ground_gradient_uniforms>* _ground_gradient_renderer;
	static renderer<plain_vertex3, ground_color_uniforms>* _ground_plain_renderer;
	static renderer<texture_vertex3, ground_texture_uniforms>* _ground_texture_renderer;
	static renderer<plain_vertex, plain_uniforms>* _ground_shadow_renderer;

	static renderer<plain_vertex, ground_texture_uniforms>* _water_inside_renderer;
	static renderer<plain_vertex, ground_texture_uniforms>* _water_border_renderer;

	static texture* _textureBackgroundLinen;
	static texture* _textureBackgroundTerrain;
	static texture* _textureUnitMarkers;
	static texture* _textureMovementBlue;
	static texture* _textureMovementGray;
	static texture* _textureMovementRed;
	static texture* _textureMissileBlue;
	static texture* _textureMissileGray;
	static texture* _textureMissileRed;
	static texture* _textureBillboards;
	static texture* _textureTouchMarker;

	static void Initialize();
};


#endif
