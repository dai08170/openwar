// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TextureBillboardRenderer_H
#define TextureBillboardRenderer_H

#include <map>
#include <vector>

#include "Algebra/affine2.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/Image.h"
#include "VertexShape.h"

class Viewport;


class BillboardTextureSheet
{
	struct item
	{
		int shape;
		float facing; // degrees
		affine2 texcoords;
		item(int s, float f, affine2 t) : shape(s), facing(f), texcoords(t) { }
	};

	TextureAtlas* _texture;
	std::map<int, std::vector<item>> _items;
	int _shapeCount;

public:
	BillboardTextureSheet(GraphicsContext* gc);
	~BillboardTextureSheet();

	Texture* GetTexture() const { return _texture; }

	int AddSheet(const Image& image);
	int AddShape();

	void SetTexCoords(int shape, float facing, const affine2& texcoords);

	affine2 GetTexCoords(int shape, float facing);
};


struct Billboard
{
	glm::vec3 position;
	float facing;
	float height;
	int shape;

	Billboard() {}
	Billboard(glm::vec3 p, float f, float h, int s) : position(p), facing(f), height(h), shape(s) {}
};


struct BillboardModel
{
	BillboardTextureSheet* texture;
	std::vector<Billboard> staticBillboards;
	std::vector<Billboard> dynamicBillboards;

	int _billboardTreeShapes[16];
	int _billboardShapeCasualtyAsh[8];
	int _billboardShapeCasualtySam[8];
	int _billboardShapeCasualtyCav[16];
	int _billboardShapeFighterSamBlue;
	int _billboardShapeFighterSamRed;
	int _billboardShapeFighterAshBlue;
	int _billboardShapeFighterAshRed;
	int _billboardShapeFighterCavBlue;
	int _billboardShapeFighterCavRed;
	int _billboardShapeSmoke[8];
};


class BillboardTextureShape
{
public:
	VertexShape_3f_1f_2f_2f _vertices;

	BillboardTextureShape();
	~BillboardTextureShape();

	void Reset();
	void AddBillboard(glm::vec3 position, float height, affine2 texcoords);

	void Draw(Viewport* viewport,GraphicsContext* gc, Texture* tex, const glm::mat4& transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bool depthTest, bounds1f sizeLimit = bounds1f(0, 1024));

	void Render(Viewport* viewport, GraphicsContext* gc, BillboardModel* billboardModel, const glm::mat4& transform, const glm::vec3& cameraUp, float viewportHeight, float cameraFacingDegrees, bool flip);
};


#endif
