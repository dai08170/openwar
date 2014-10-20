// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef PatchShape_H
#define PatchShape_H

#include "VertexBuffer.h"
#include "Shape.h"


struct TexturePatch
{
	bounds2f outer;
	bounds2f inner;

	TexturePatch();
	TexturePatch(bounds2f o, bounds2f i);
};

struct TexturePatchFactory
{
	glm::ivec2 size;

	TexturePatchFactory(int size_u, int size_v);
	TexturePatch GetTexturePatch(int u0, int v0, int size_u, int size_v, int insert_u, int inset_v);
};


class PatchShape : public Shape
{
	VertexGlyph<Vertex_2f_2f> _glyph;

public:
	bounds2f outer_xy;
	bounds2f inner_xy;
	bounds2f outer_uv;
	bounds2f inner_uv;

	PatchShape() { }
	PatchShape(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	void Reset();
	void Reset(TexturePatch tile, bounds2f bounds, glm::vec2 inset);

	VertexGlyph<Vertex_2f_2f>* GetGlyph();

protected:
	void generate(std::vector<Vertex_2f_2f>& vertices);
	void rectangle(std::vector<Vertex_2f_2f>& vertices, bounds2f xy, bounds2f uv);

private:
	PatchShape(const PatchShape& x) : Shape(x) { }
	PatchShape& operator=(const PatchShape&) { return *this; }
};


#endif
