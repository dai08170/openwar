// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef BILLBOARDTEXTURE_H
#define BILLBOARDTEXTURE_H

#include "affine2.h"
#include "image.h"
#include "texture.h"


class BillboardTexture
{
	struct item
	{
		int shape;
		float angle; // degrees
		affine2 texcoords;
		item(int s, float a, affine2 t) : shape(s), angle(a), texcoords(t) { }
	};

	texture* _texture;
	std::vector<item> _items;
	int _shapeCount;

public:
	BillboardTexture();
	~BillboardTexture();

	texture* GetTexture() const { return _texture; }

	int AddSheet(const image& img);
	int AddShape(int sheet);

	void SetTexCoords(int shape, float angle, const affine2& texcoords);

	affine2 GetTexCoords(int shape, float angle);
};


#endif
