// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonRendering_H
#define ButtonRendering_H

#include "Algebra/bounds.h"
#include "Texture.h"
#include "WidgetShape.h"
#include "CommonShaders.h"


struct ButtonIcon;

struct ButtonRendering
{
	GraphicsContext* _gc;

	TextureFont* _string_font;
	WidgetShape* _string_shape;

	Texture* _textureButtonBackground;
	Texture* _textureButtonHighlight;
	Texture* _textureButtonSelected;
	Texture* _textureButtonIcons;
	Texture* _textureEditorTools;

	ButtonIcon* buttonIconPlay;
	ButtonIcon* buttonIconPause;
	ButtonIcon* buttonIconRewind;
	ButtonIcon* buttonIconHelp;
	ButtonIcon* buttonIconChat;

	ButtonIcon* buttonEditorToolHand;
	ButtonIcon* buttonEditorToolPaint;
	ButtonIcon* buttonEditorToolErase;
	ButtonIcon* buttonEditorToolSmear;
	ButtonIcon* buttonEditorToolHills;
	ButtonIcon* buttonEditorToolTrees;
	ButtonIcon* buttonEditorToolWater;
	ButtonIcon* buttonEditorToolFords;

	ButtonRendering(GraphicsContext* gc);

	void RenderBackground(const glm::mat4& transform, bounds2f bounds);
	void RenderHighlight(const glm::mat4& transform, bounds2f bounds);
	void RenderSelected(const glm::mat4& transform, bounds2f bounds);
	void RenderButtonText(const glm::mat4& transform, glm::vec2 position, const char* text);

	void RenderCornerButton(const glm::mat4& transform, Texture* texture, bounds2f bounds, float radius);
	void RenderButtonIcon(const glm::mat4& transform, glm::vec2 position, ButtonIcon* buttonIcon, bool disabled);
	void RenderTextureRect(const glm::mat4& transform, Texture* texture, bounds2f b, bounds2f t, float alpha = 1);

};


#endif
