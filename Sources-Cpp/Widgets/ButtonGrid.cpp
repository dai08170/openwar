// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonGrid.h"
#include "Surface/Surface.h"
#include "Surface/Touch.h"
#include "Graphics/Viewport.h"
#include "Graphics/TextureFont.h"


const ButtonAlignment ButtonAlignment::TopLeft(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::TopCenter(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::TopRight(ButtonAlignment::Vertical::Top, ButtonAlignment::Horizontal::Right);
const ButtonAlignment ButtonAlignment::CenterLeft(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::Center(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::CenterRight(ButtonAlignment::Vertical::Center, ButtonAlignment::Horizontal::Right);
const ButtonAlignment ButtonAlignment::BottomLeft(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Left);
const ButtonAlignment ButtonAlignment::BottomCenter(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Center);
const ButtonAlignment ButtonAlignment::BottomRight(ButtonAlignment::Vertical::Bottom, ButtonAlignment::Horizontal::Right);


/***/


ButtonItem::ButtonItem(ButtonArea* buttonArea, const char* text) :
	_hotspot(),
	_buttonArea(buttonArea),
	_buttonIcon(nullptr),
	_keyboardShortcut('\0'),
	_selected(false),
	_disabled(false),
	selectedImage(buttonArea->GetButtonGrid()),
	buttonImage(buttonArea->GetButtonGrid()),
	highlightImage(buttonArea->GetButtonGrid()),
	buttonString(buttonArea->GetButtonGrid())
{
	_hotspot.SetDistance([this](glm::vec2 position) {
		return _bounds.distance(_buttonArea->GetButtonGrid()->GetViewport2D().ScreenToContent(position));
	});

	buttonString.SetString(text);
}



ButtonItem::ButtonItem(ButtonArea* buttonArea, std::shared_ptr<TextureImage> icon) :
	_hotspot(),
	_buttonArea(buttonArea),
	_buttonIcon(icon),
	_keyboardShortcut('\0'),
	_selected(false),
	_disabled(false),
	selectedImage(buttonArea->GetButtonGrid()),
	buttonImage(buttonArea->GetButtonGrid()),
	highlightImage(buttonArea->GetButtonGrid()),
	buttonString(buttonArea->GetButtonGrid())
{
	_hotspot.SetDistance([this](glm::vec2 position) {
		return _bounds.distance(_buttonArea->GetButtonGrid()->GetViewport2D().ScreenToContent(position));
	});
}



ButtonItem::~ButtonItem()
{
}


void ButtonItem::SetButtonText(const char* value)
{
	buttonString.SetString(value ?: "");
	_buttonArea->GetButtonGrid()->UpdateLayout();
}


glm::vec2 ButtonItem::CalculateSize() const
{
	if (buttonString.GetString()[0] != '\0')
	{
		glm::vec2 size = buttonString.MeasureSize();
		return glm::vec2(38 + size.x, fmaxf(44, size.y));
	}

	if (_buttonIcon)
	{
		glm::vec2 size = _buttonIcon->GetBounds().outer.size();
		if (size.x > 44 || size.y > 44)
			return size;
	}

	return glm::vec2(44, 44);
}



/***/



ButtonArea::ButtonArea(ButtonGrid* buttonView, int numberOfColumns) :
	_buttonView(buttonView),
	noaction([](){}),
	backgroundImage(buttonView)
{
	if (numberOfColumns > 0)
		columns.assign(numberOfColumns, 0);
}



ButtonArea::~ButtonArea()
{
	for (ButtonItem* buttonItem : buttonItems)
		delete buttonItem;
}



ButtonItem* ButtonArea::AddButtonItem(const char* buttonText)
{
	ButtonItem* buttonItem = new ButtonItem(this, buttonText);

	buttonItems.push_back(buttonItem);

	_buttonView->UpdateLayout();

	return buttonItem;
}



ButtonItem* ButtonArea::AddButtonItem(std::shared_ptr<TextureImage> buttonIcon)
{
	ButtonItem* buttonItem = new ButtonItem(this, buttonIcon);
	buttonItems.push_back(buttonItem);

	_buttonView->UpdateLayout();

	return buttonItem;
}



glm::vec2 ButtonArea::CalculateSize() const
{
	glm::vec2 result;

	for (float width : columns)
		result.x += width;

	for (float height : rows)
		result.y += height;

	return result;
}



void ButtonArea::UpdateColumnsAndRows()
{
	if (buttonItems.empty())
		return;

	int numberOfColumns = columns.empty() ? 1 : (int)columns.size();
	int numberOfRows = (int)(buttonItems.size() + columns.size() - 1) / (int)columns.size();

	columns.assign(numberOfColumns, 0);
	rows.assign(numberOfRows, 0);

	for (size_t i = 0; i < rows.size(); ++i)
		for (size_t j = 0; j < columns.size(); ++j)
		{
			size_t index = i * columns.size() + j;
			if (index < buttonItems.size())
			{
				glm::vec2 size = buttonItems[index]->CalculateSize();
				columns[j] = fmaxf(columns[j], size.x);
				rows[i] = fmaxf(rows[i], size.y);
			}
		}

	if (buttonItems.size() == 1 && buttonItems.front()->GetButtonText())
	{
		columns.front() = fmaxf(172, columns.front());
	}
}



void ButtonArea::UpdateBounds(bounds2f bounds)
{
	_bounds = bounds.add_radius(_margin);

	float y = 0;
	for (size_t i = 0; i < rows.size(); ++i)
	{
		float h = rows[i];
		float x = 0;
		for (size_t j = 0; j < columns.size(); ++j)
		{
			float w = columns[j];
			size_t index = i * columns.size() + j;
			if (index < buttonItems.size())
			{
				buttonItems[index]->SetBounds(bounds2f(x, -y - h, x + w, -y) + bounds.fix<0,1>());
			}
			x += w;
		}
		y += h;
	}
}



/***/




ButtonGrid::ButtonGrid(Surface* surface, std::shared_ptr<ScrollerViewport> viewport, ButtonGridTextureSheet* textureSheet, ButtonAlignment alignment) : WidgetView(surface, viewport),
	_gc(surface->GetGraphicsContext()),
	_alignment(alignment),
	_textureSheet(textureSheet)
{
}



bool ButtonGrid::HasButtons() const
{
	return !_buttonAreas.empty() && !_buttonAreas.front()->buttonItems.empty();
}



void ButtonGrid::Reset()
{
	_obsolete.insert(_obsolete.end(), _buttonAreas.begin(), _buttonAreas.end());
	_buttonAreas.clear();
}



ButtonArea* ButtonGrid::AddButtonArea(int numberOfColumns)
{
	ButtonArea* buttonArea = new ButtonArea(this, numberOfColumns);
	_buttonAreas.push_back(buttonArea);
	return buttonArea;
}



void ButtonGrid::UpdateLayout()
{
	bounds2f contentBounds = GetViewport2D().GetVisibleBounds();
	glm::vec2 contentCenter = contentBounds.mid();
	float margin = 3;
	float spacing = 20;
	std::vector<bounds2f> bounds;
	glm::vec2 position(-spacing, spacing);

	for (ButtonArea* buttonArea : _buttonAreas)
	{
		buttonArea->UpdateColumnsAndRows();
		glm::vec2 size = buttonArea->CalculateSize();
		position += glm::vec2(spacing, -spacing - size.y);
		bounds.push_back(bounds2f(position, position + size));
		position += glm::vec2(size.x, 0);
	}

	if (_alignment.horizontal == ButtonAlignment::Horizontal::Center)
	{
		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(contentCenter.x - b.x().mid(), contentBounds.max.y - margin);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentCenter.x - b.x().mid(), contentCenter.y - position.y / 2);
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentCenter.x - b.x().mid(), contentBounds.min.y + margin - position.y);
				break;
		}
	}
	else
	{
		switch (_alignment.horizontal)
		{
			case ButtonAlignment::Horizontal::Left:
				for (bounds2f& b : bounds)
					b += glm::vec2(contentBounds.min.x + margin, 0);
				break;

			case ButtonAlignment::Horizontal::Right:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(contentBounds.max.x - margin - position.x, 0);
				break;

			default:
				break;
		}

		switch (_alignment.vertical)
		{
			case ButtonAlignment::Vertical::Top:
				for (bounds2f& b : bounds)
					b += glm::vec2(0, contentBounds.max.y - margin - b.max.y);
				break;

			case ButtonAlignment::Vertical::Center:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, contentCenter.y - b.y().mid());
				break;

			case ButtonAlignment::Vertical::Bottom:
		        for (bounds2f& b : bounds)
	                b += glm::vec2(0, contentBounds.min.y + margin - b.min.y);
				break;
		}
	}

	for (size_t i = 0; i < _buttonAreas.size(); ++i)
		_buttonAreas[i]->UpdateBounds(bounds[i]);
}


void ButtonGrid::Animate(double secondsSinceLastUpdate)
{
	UpdateLayout();
	if (!_obsolete.empty())
	{
		for (ButtonArea* buttonArea : _obsolete)
			delete buttonArea;
		_obsolete.clear();
	}
}


void ButtonGrid::OnTouchBegin(Touch* touch)
{
	glm::vec2 position = GetViewport2D().ScreenToContent(touch->GetOriginalPosition());

	for (ButtonArea* buttonArea : _buttonAreas)
		for (ButtonItem* buttonItem : buttonArea->buttonItems)
			if (buttonItem->HasAction()
				&& !buttonItem->IsDisabled()
				&& buttonItem->GetBounds().contains(position))
			{
				buttonItem->GetHotspot()->SubscribeTouch(touch);
			}
}


void ButtonGrid::Render()
{
	for (ButtonArea* buttonArea : _buttonAreas)
	{
		buttonArea->backgroundImage.SetBounds(buttonArea->_bounds.add_radius(10));
		buttonArea->backgroundImage.SetInset(BorderInset(32));
		buttonArea->backgroundImage.SetTextureImage(_textureSheet->buttonBackground);

		for (ButtonItem* buttonItem : buttonArea->buttonItems)
		{
			buttonItem->selectedImage.SetBounds(buttonItem->GetBounds().add_radius(10));
			buttonItem->selectedImage.SetInset(BorderInset(32));
			buttonItem->selectedImage.SetTextureImage(buttonItem->IsSelected() ? _textureSheet->buttonSelected : nullptr);

			if (buttonItem->GetButtonIcon())
			{
				bounds2f bounds_xy = buttonItem->GetButtonIcon()->GetBounds().outer;
				bounds_xy -= bounds_xy.mid();
				bounds_xy += buttonItem->GetBounds().mid();
				buttonItem->buttonImage.SetBounds(bounds_xy);
			}

			buttonItem->buttonImage.SetTextureImage(buttonItem->GetButtonIcon());
			buttonItem->buttonImage.SetAlpha(buttonItem->IsDisabled() ? 0.5f : 1.0f);

			buttonItem->highlightImage.SetBounds(buttonItem->GetBounds());
			buttonItem->highlightImage.SetInset(BorderInset(32));
			buttonItem->highlightImage.SetTextureImage(buttonItem->IsHighlight() ? _textureSheet->buttonHighlight : nullptr);

			buttonItem->buttonString.SetString(buttonItem->GetButtonText() ?: "");
			buttonItem->buttonString.SetPosition(buttonItem->GetBounds().mid() - 0.5f * buttonItem->buttonString.MeasureSize());
		}
	}

	WidgetView::Render();
}
