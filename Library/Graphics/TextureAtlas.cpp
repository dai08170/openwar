#include "TextureAtlas.h"
#include "GraphicsContext.h"
#include "TextureFont.h"



TextureAtlas::TextureAtlas(GraphicsContext* gc) : Texture(gc),
	_gc(gc),
	_textureAtlasImage(nullptr),
	_permanentHeight(0),
	_discardableHeight(0),
	_dirty(false)
{
}


GraphicsContext* TextureAtlas::GetGraphicsContext() const
{
	return _gc;
}


TextureFont* TextureAtlas::GetTextureFont(const FontDescriptor& fontDescriptor)
{
	FontAdapter* fontAdapter = _gc->GetFontAdapter(fontDescriptor);

	auto i = _textureFonts.find(fontAdapter);
	if (i != _textureFonts.end())
		return i->second;

	TextureFont* result = new TextureFont(this, fontAdapter);
	_textureFonts[fontAdapter] = result;
	return result;
}


void TextureAtlas::LoadAtlasFromResource(const resource& r)
{
	_textureAtlasImage = new Image();
	_textureAtlasImage->LoadFromResource(r);
	LoadTextureFromImage(*_textureAtlasImage);
}


void TextureAtlas::LoadTextureFromImage(const Image& image)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();
}



#ifdef OPENWAR_IMAGE_USE_SDL
void TextureAtlas::LoadTextureFromSurface(SDL_Surface* surface)
{
	SDL_Surface* tmp = nullptr;
	if (surface->format->format != SDL_PIXELFORMAT_ABGR8888)
	{
		tmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
		surface = tmp;
	}

	SDL_LockSurface(surface);

	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

	SDL_UnlockSurface(surface);

	if (tmp != nullptr)
		SDL_FreeSurface(tmp);
}
#endif


void TextureAtlas::UpdateTexture()
{
	if (_dirty)
	{
		if (_textureAtlasImage != nullptr)
			LoadTextureFromImage(*_textureAtlasImage);
		_dirty = false;
	}
}


std::shared_ptr<TextureImage> TextureAtlas::AddTextureImage(const Image& image, TextureImageType textureImageType)
{
	if (_textureAtlasImage == nullptr)
		_textureAtlasImage = new Image(glm::max(512, image.GetWidth()), glm::max(256, image.GetHeight()));

	if (_permanentHeight + _discardableHeight + image.GetHeight() > _textureAtlasImage->GetHeight())
	{
		DiscardTextureImages();
		if (_textureAtlasImage->GetHeight() < 2048)
		{
			Image* textureAtlasImage = new Image(_textureAtlasImage->GetWidth(), _textureAtlasImage->GetHeight() * 2);
			textureAtlasImage->Copy(*_textureAtlasImage, 0, 0);
			delete _textureAtlasImage;
			_textureAtlasImage = textureAtlasImage;
		}
	}

	glm::ivec2 atlasSize(_textureAtlasImage->GetWidth(), _textureAtlasImage->GetHeight());
	glm::ivec2 imageSize(image.GetWidth(), image.GetHeight());

	bool discardable = textureImageType == TextureImageType::Discardable;
	glm::ivec2 position;

	if (!discardable)
	{
		if (_permamentPos.x + imageSize.x > atlasSize.x)
			_discardablePos = glm::ivec2(0, _permanentHeight);

		position = _permamentPos;

		_permamentPos.x += imageSize.x;
		_permanentHeight = glm::max(_permanentHeight, _permamentPos.y + imageSize.y);
	}
	else
	{
		if (_discardablePos.x + imageSize.x > atlasSize.x)
			_discardablePos = glm::ivec2(0, _discardableHeight);

		position = glm::ivec2(_discardablePos.x, atlasSize.y - _discardablePos.y - imageSize.y);

		_discardablePos.x += imageSize.x;
		_discardableHeight = glm::max(_discardableHeight, _discardablePos.y + imageSize.y);
	}

	bounds2i bounds = bounds2i(position, position + imageSize);
	_textureAtlasImage->Copy(image, position.x, position.y);
	_dirty = true;

	return NewTextureImage(BorderBounds(bounds), discardable);
}


TextureSheet TextureAtlas::AddTextureSheet(const Image& image)
{
	std::shared_ptr<TextureImage> textureImage = AddTextureImage(image, TextureImageType::Permanent);
	return TextureSheet(this, textureImage->GetBounds().outer);
}


TextureSheet TextureAtlas::GetTextureSheet(const bounds2f& bounds)
{
	return TextureSheet(this, bounds);
}


std::shared_ptr<TextureImage> TextureAtlas::NewTextureImage(const BorderBounds& bounds, bool discardable)
{
	std::shared_ptr<TextureImage> result = std::make_shared<TextureImage>();

	result->_textureAtlas = this;
	result->_bounds = bounds;
	result->_discardable = discardable;

	_textureImages.push_back(result);

	return result;
}


void TextureAtlas::DiscardTextureImages()
{
	for (std::shared_ptr<TextureImage> textureImage : _textureImages)
		if (textureImage->_discardable)
			textureImage->_discarded = true;

	_textureImages.erase(
		std::remove_if(_textureImages.begin(), _textureImages.end(), [](std::shared_ptr<TextureImage> textureImage) {
			return textureImage->_discarded;
		}),
		_textureImages.end());


	_discardablePos = glm::ivec2(0, 0);
	_discardableHeight = 0;
}


/***/


TextureImage::TextureImage() :
	_textureAtlas(nullptr),
	_discardable(false),
	_discarded(false)
{
}


bool TextureImage::IsDiscarded() const
{
	return _discarded;
}


void TextureImage::SetBounds(const BorderBounds& value)
{
	_bounds = value;

}


BorderBounds TextureImage::GetBounds() const
{
	return _bounds;
}


BorderBounds TextureImage::GetCoords() const
{
	glm::vec2 size = glm::vec2(_textureAtlas->_textureAtlasImage->GetWidth(), _textureAtlas->_textureAtlasImage->GetHeight());
	BorderBounds result;
	result.inner = _bounds.inner / size;
	result.outer = _bounds.outer / size;
	return result;
}


/***/


TextureSheet::TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v) :
	_textureAtlas(textureAtlas),
	_sheetBounds(0, 0, size_u, size_v)
{
}


TextureSheet::TextureSheet(TextureAtlas* textureAtlas, const bounds2f& bounds) :
	_textureAtlas(textureAtlas),
	_sheetBounds(bounds)
{
}


glm::vec2 TextureSheet::MapCoord(int u, int v) const
{
	return (_sheetBounds.min + glm::vec2(u, v)) / (glm::vec2)_textureAtlas->_textureAtlasImage->size();
}


std::shared_ptr<TextureImage> TextureSheet::NewTextureImage(const BorderBounds& bounds)
{
	BorderBounds offsetBounds;
	offsetBounds.inner = bounds.inner + _sheetBounds.min;
	offsetBounds.outer = bounds.outer + _sheetBounds.min;
	return _textureAtlas->NewTextureImage(offsetBounds, false);
}
