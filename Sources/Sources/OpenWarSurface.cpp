// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "../BattleModel/BattleScenario.h"
#include "ButtonGrid.h"
#include "ButtonGridGesture.h"
#include "../Library/Shapes/GradientShape3.h"
#include "BattleView/BattleGesture.h"
#include "BattleView/BattleView.h"
#include "TerrainView/EditorGesture.h"
#include "OpenWarSurface.h"
#include <glm/gtc/matrix_transform.hpp>


OpenWarSurface::OpenWarSurface(glm::vec2 size, float pixelDensity) : BattleSurface(size, pixelDensity),
_buttonRendering(nullptr),
_editorModel(nullptr),
_editorGesture(nullptr),
_buttonsTopLeft(nullptr),
_buttonsTopRight(nullptr),
_buttonGesture(nullptr),
_buttonItemHand(nullptr),
_buttonItemPaint(nullptr),
_buttonItemErase(nullptr),
_buttonItemSmear(nullptr),
_buttonItemHills(nullptr),
_buttonItemTrees(nullptr),
_buttonItemWater(nullptr),
_buttonItemFords(nullptr),
_scriptHintRenderer(nullptr)
{
	_buttonRendering = new ButtonRendering(_renderers, pixelDensity);

	_buttonsTopLeft = new ButtonGrid(_buttonRendering, ButtonAlignment::TopLeft);
	_buttonsTopRight = new ButtonGrid(_buttonRendering, ButtonAlignment::TopRight);

	_buttonsTopLeft->SetContainer(this);
	_buttonsTopRight->SetContainer(this);

	_buttonGesture = new ButtonGridGesture();
	_buttonGesture->buttonViews.push_back(_buttonsTopLeft);
	_buttonGesture->buttonViews.push_back(_buttonsTopRight);

	ButtonArea* toolButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHand = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolHand);
	_buttonItemSmear = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolSmear);
	_buttonItemPaint = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolPaint);
	_buttonItemErase = toolButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolErase);

	ButtonArea* featureButtonArea = _buttonsTopLeft->AddButtonArea(4);
	_buttonItemHills = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolHills);
	_buttonItemTrees = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolTrees);
	_buttonItemWater = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolWater);
	_buttonItemFords = featureButtonArea->AddButtonItem(_buttonRendering->buttonEditorToolFords);

	_buttonItemHand->SetAction([this](){ SetEditorMode(EditorMode::Hand); });
	_buttonItemPaint->SetAction([this](){ SetEditorMode(EditorMode::Paint); });
	_buttonItemErase->SetAction([this](){ SetEditorMode(EditorMode::Erase); });
	_buttonItemSmear->SetAction([this](){ SetEditorMode(EditorMode::Smear); });
	_buttonItemHills->SetAction([this](){ SetEditorFeature(TerrainFeature::Hills); });
	_buttonItemTrees->SetAction([this](){ SetEditorFeature(TerrainFeature::Trees); });
	_buttonItemWater->SetAction([this](){ SetEditorFeature(TerrainFeature::Water); });
	_buttonItemFords->SetAction([this](){ SetEditorFeature(TerrainFeature::Fords); });

	_buttonItemHand->SetKeyboardShortcut('1');
	_buttonItemSmear->SetKeyboardShortcut('2');
	_buttonItemPaint->SetKeyboardShortcut('3');
	_buttonItemErase->SetKeyboardShortcut('4');
	_buttonItemHills->SetKeyboardShortcut('5');
	_buttonItemTrees->SetKeyboardShortcut('6');
	_buttonItemWater->SetKeyboardShortcut('7');
	_buttonItemFords->SetKeyboardShortcut('8');

	UpdateButtonsAndGestures();

	_scriptHintRenderer = new GradientLineShape3(GetGraphicsContext());
}


OpenWarSurface::~OpenWarSurface()
{

}


void OpenWarSurface::ResetBattleViews(BattleScenario* scenario, const std::vector<BattleCommander*>& commanders)
{
	delete _editorGesture;
	_editorGesture = nullptr;

	delete _editorModel;
	_editorModel = nullptr;

	BattleSurface::ResetBattleViews(scenario, commanders);

	const std::vector<BattleView*>& battleViews = GetBattleViews();
	if (!battleViews.empty())
	{
		BattleView* battleView = battleViews.front();
		_editorModel = new EditorModel(battleView, battleView->GetSmoothTerrainRenderer());
		_editorGesture = new EditorGesture(battleView, _editorModel);
	}

	SetPlaying(false);
	UpdateButtonsAndGestures();
}


void OpenWarSurface::OnFrameChanged()
{
	BattleSurface::OnFrameChanged();
	bounds2f viewport = bounds2f(0, 0, GetSize());
	_buttonsTopLeft->SetFrame(viewport);
	_buttonsTopRight->SetFrame(viewport);
}


void OpenWarSurface::Update(double secondsSinceLastUpdate)
{
	BattleSurface::Update(secondsSinceLastUpdate);

	if (_scenario != nullptr && _playing)
		_scenario->GetSimulator()->AdvanceTime((float)secondsSinceLastUpdate);
}


void OpenWarSurface::RenderSurface()
{
	glm::vec2 size = GetSize();
	glm::vec2 translate = -size / 2.0f;
	glm::vec2 scale = 2.0f / size;
	glm::mat4 transform = glm::translate(glm::scale(glm::mat4x4(), glm::vec3(scale, 1.0f)), glm::vec3(translate, 0.0f));

	glClearColor(0.9137f, 0.8666f, 0.7647f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);

	BattleSurface::RenderSurface();

	_buttonsTopLeft->Render(transform* _buttonsTopLeft->GetContentTransform());
	_buttonsTopRight->Render(transform* _buttonsTopRight->GetContentTransform());
}


void OpenWarSurface::MouseEnter(glm::vec2 position)
{
	//_battleView->ShowMouseHint(position);
}


void OpenWarSurface::MouseHover(glm::vec2 position)
{
	//_battleView->ShowMouseHint(position);
}


void OpenWarSurface::MouseLeave(glm::vec2 position)
{
	//_battleView->HideMouseHint();
}


void OpenWarSurface::ClickedPlay()
{
	SetPlaying(true);
	UpdateButtonsAndGestures();
}


void OpenWarSurface::ClickedPause()
{
	SetPlaying(false);
	UpdateButtonsAndGestures();
}


void OpenWarSurface::SetEditorMode(EditorMode editorMode)
{
	if (_editorModel != nullptr)
	{
		_editorModel->SetEditorMode(editorMode);
		UpdateButtonsAndGestures();
	}
}


void OpenWarSurface::SetEditorFeature(TerrainFeature terrainFeature)
{
	if (_editorModel != nullptr)
	{
		_editorModel->SetTerrainFeature(terrainFeature);
		UpdateButtonsAndGestures();
	}
}


void OpenWarSurface::UpdateButtonsAndGestures()
{
	bool playing = IsPlaying();
	bool editing = _editorModel != nullptr && _editorModel->GetEditorMode() != EditorMode::Hand;
	SetEditing(editing);

	_buttonItemHand->SetDisabled (playing);
	_buttonItemPaint->SetDisabled(playing);
	_buttonItemErase->SetDisabled(playing);
	_buttonItemSmear->SetDisabled(playing);
	_buttonItemHills->SetDisabled(playing);
	_buttonItemTrees->SetDisabled(playing);
	_buttonItemWater->SetDisabled(playing);
	_buttonItemFords->SetDisabled(playing);

	if (_editorModel != nullptr)
	{
		_buttonItemHand->SetSelected(_editorModel->GetEditorMode() == EditorMode::Hand);
		_buttonItemPaint->SetSelected(_editorModel->GetEditorMode() == EditorMode::Paint);
		_buttonItemErase->SetSelected(_editorModel->GetEditorMode() == EditorMode::Erase);
		_buttonItemSmear->SetSelected(_editorModel->GetEditorMode() == EditorMode::Smear);
		_buttonItemHills->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Hills);
		_buttonItemTrees->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Trees);
		_buttonItemWater->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Water);
		_buttonItemFords->SetSelected(_editorModel->GetTerrainFeature() == TerrainFeature::Fords);
	}

	if (_editorGesture != nullptr)
	{
		_editorGesture->SetEnabled(editing);
	}

	_buttonsTopRight->Reset();
	if (playing)
		_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPause)->SetAction([this](){ ClickedPause(); });
	else
		_buttonsTopRight->AddButtonArea()->AddButtonItem(_buttonRendering->buttonIconPlay)->SetAction([this](){ ClickedPlay(); });
}
