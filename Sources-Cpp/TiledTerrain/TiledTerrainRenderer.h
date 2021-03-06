// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TiledTerrainSurface_H
#define TiledTerrainSurface_H

class GraphicsContext;

#include <map>
#include "BattleMap/TiledGroundMap.h"
#include "Algebra/bounds.h"
#include "Graphics/Image.h"
#include "Algorithms/bspline_patch.h"
#include "Graphics/Texture.h"

class Viewport;


class TiledTerrainRenderer
{
	GraphicsContext* _gc;
	const TiledGroundMap* _tiledGroundMap;
	std::map<std::string, Texture*> _textures;

public:
	TiledTerrainRenderer(GraphicsContext* gc, const TiledGroundMap* tiledGroundMap);
	~TiledTerrainRenderer();

	void Render(Viewport* viewport, const glm::mat4& transform, const glm::vec3& lightNormal);
};


#endif
