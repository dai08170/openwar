// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef TERRAINVIEW_H
#define TERRAINVIEW_H

#include "BattleModel/GroundMap.h"
#include "BattleModel/HeightMap.h"
#include "Algebra/geometry.h"
#include "Shapes/VertexShape.h"
#include "Surface/View.h"

class EditorModel;
class EditorHotspot;
class GraphicsContext;
class TerrainHotspot;
class TerrainViewport;
class Touch;


class TerrainView : public View
{
	TerrainViewport* _terrainViewport{};

	std::shared_ptr<EditorHotspot> _editorHotspot;
	std::shared_ptr<TerrainHotspot> _terrainHotspot;

protected:
	HeightMap* _heightMap{};

public:
	TerrainView(Surface* surface);
	virtual ~TerrainView();

	glm::vec2 GetScreenTop() const;
	glm::vec2 GetScreenLeft() const;
	glm::vec2 GetScreenBottom() const;
	glm::vec2 GetScreenRight() const;

	TerrainViewport* GetViewport() const;

	void SetEditorHotspot(std::shared_ptr<EditorHotspot> hotspot);

	virtual void OnTouchEnter(Touch* touch);
	virtual void OnTouchBegin(Touch* touch);
	virtual void Render() = 0;

	void RenderMouseHint(VertexShape_3f& vertices);

	void SetHeightMap(HeightMap* heightMap);

	ray GetCameraRay(glm::vec2 screenPosition) const;
	glm::vec3 GetTerrainPosition2(glm::vec2 screenPosition, float height = 0) const;
	glm::vec3 GetTerrainPosition3(glm::vec2 screenPosition) const;

	void Move(glm::vec3 originalContentPosition, glm::vec2 currentScreenPosition);
	void Zoom(std::pair<glm::vec3, glm::vec3> originalContentPositions, std::pair<glm::vec2, glm::vec2> currentScreenPositions);
	void Orbit(glm::vec3 anchor, float angle);

	void MoveCamera(glm::vec3 position);
	void ClampCameraPosition();

	glm::vec2 ContentToScreen(glm::vec3 value) const;

	glm::vec3 GetTerrainPosition(glm::vec2 p, float h = 1) { return _heightMap->GetPosition(p, h); };

};


#endif
