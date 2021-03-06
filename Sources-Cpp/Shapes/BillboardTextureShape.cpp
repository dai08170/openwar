// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <algorithm>
#include "BillboardTextureShape.h"
#include "BillboardTextureShader.h"
#include "Graphics/RenderCall.h"
#include "Algebra/geometry.h"



BillboardTextureSheet::BillboardTextureSheet(GraphicsContext* gc) :
	_texture(nullptr),
	_shapeCount(0)
{
	_texture = new TextureAtlas(gc);
}


BillboardTextureSheet::~BillboardTextureSheet()
{
	delete _texture;
}


int BillboardTextureSheet::AddSheet(const Image& image)
{
	_texture->LoadTextureFromImage(image);

	return 1;
}


int BillboardTextureSheet::AddShape()
{
	return ++_shapeCount;
}


void BillboardTextureSheet::SetTexCoords(int shape, float facing, affine2 const & texcoords)
{
	if (_items.find(shape) != _items.end())
		_items[shape].push_back(item(shape, facing, texcoords));
	else
		_items[shape] = std::vector<item>(1, item(shape, facing, texcoords));
}


affine2 BillboardTextureSheet::GetTexCoords(int shape, float facing)
{
	affine2 result;
	float diff = 360;

	auto i = _items.find(shape);
	if (i != _items.end())
	{
		const std::vector<item>& items = (*i).second;
		for (auto j = items.begin(); j != items.end(); ++j)
		{
			if ((*j).shape == shape)
			{
				float d = glm::abs(diff_degrees((*j).facing, facing));
				if (d < diff)
				{
					diff = d;
					result = (*j).texcoords;
				}
			}
		}
	}

	return result;
}


BillboardTextureShape::BillboardTextureShape()
{
}


BillboardTextureShape::~BillboardTextureShape()
{
}


void BillboardTextureShape::Reset()
{
	_vertices._mode = GL_POINTS;
	_vertices.Clear();
}


void BillboardTextureShape::AddBillboard(glm::vec3 position, float height, affine2 texcoords)
{
	glm::vec2 texpos = texcoords.transform(glm::vec2(0, 0));
	glm::vec2 texsize = texcoords.transform(glm::vec2(1, 1)) - texpos;

	_vertices.AddVertex(Vertex_3f_1f_2f_2f(position, height, texpos, texsize));
}



struct billboard_index
{
	int index;
	float order;
};

void BillboardTextureShape::Draw(
	Viewport* viewport,
	GraphicsContext* gc,
	Texture* tex,
	const glm::mat4& transform,
	const glm::vec3& cameraUp,
	float cameraFacingDegrees,
	float viewportHeight,
	bool depthTest,
	bounds1f sizeLimit)
{
	static std::vector<Vertex_3f_1f_2f_2f> vertices;
	static std::vector<billboard_index> indices;

	vertices.insert(vertices.end(), _vertices.GetVertices().begin(), _vertices.GetVertices().end());

	float a = -glm::radians(cameraFacingDegrees);
	float cos_a = cosf(a);
	float sin_a = sinf(a);

	int index = 0;
	for (const Vertex_3f_1f_2f_2f& v : _vertices.GetVertices())
	{
		glm::vec3 p = GetVertexAttribute<0>(v);
		billboard_index i;
		i.index = index++;
		i.order = cos_a * p.x - sin_a * p.y;
		indices.push_back(i);
	}

	std::sort(indices.begin(), indices.end(), [](const billboard_index& a, const billboard_index& b) -> bool {
		float diff = a.order - b.order;
		return diff == 0 ? a.index < b.index : diff > 0;
	});

	_vertices.Clear();
	for (const billboard_index& i : indices)
		_vertices.AddVertex(vertices[i.index]);

	vertices.clear();
	indices.clear();

	RenderCall<BillboardTextureShader>(gc)
		.SetVertices(&_vertices, "position", "height", "texcoord", "texsize")
		.SetUniform("transform", transform)
		.SetTexture("texture", tex)
		.SetUniform("upvector", cameraUp)
		.SetUniform("viewport_height", gc->GetCombinedScaling() * viewportHeight)
		.SetUniform("min_point_size", sizeLimit.min)
		.SetUniform("max_point_size", sizeLimit.max)
		.SetDepthTest(depthTest)
		.Render(*viewport);
}


static affine2 FlipY(const affine2& texcoords)
{
	glm::vec2 v0 = texcoords.transform(glm::vec2(0, 0));
	glm::vec2 v1 = texcoords.transform(glm::vec2(1, 1));
	return affine2(glm::vec2(v0.x, v1.y), glm::vec2(v1.x, v0.y));
}


void BillboardTextureShape::Render(Viewport* viewport, GraphicsContext* gc, BillboardModel* billboardModel, glm::mat4 const & transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bool flip)
{
	Reset();

	for (const Billboard& billboard : billboardModel->staticBillboards)
	{
		float facing = billboard.facing - cameraFacingDegrees + 180;
		affine2 texcoords = billboardModel->texture->GetTexCoords(billboard.shape, flip ? -facing : facing);
		if (flip)
			texcoords = FlipY(texcoords);
		AddBillboard(billboard.position, billboard.height, texcoords);
	}

	for (const Billboard& billboard : billboardModel->dynamicBillboards)
	{
		float facing = billboard.facing - cameraFacingDegrees + 180;
		affine2 texcoords = billboardModel->texture->GetTexCoords(billboard.shape, flip ? -facing : facing);
		if (flip)
			texcoords = FlipY(texcoords);
		AddBillboard(billboard.position, billboard.height, texcoords);
	}

	Draw(viewport, gc, billboardModel->texture->GetTexture(), transform, cameraUp, cameraFacingDegrees, viewportHeight, true);
}
