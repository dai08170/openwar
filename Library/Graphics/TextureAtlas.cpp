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

	glBindTexture(GL_TEXTURE_2D, id);
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


TextureImage* TextureAtlas::AddTextureImage(const Image& image, TextureImageType textureImageType)
{
	if (_textureAtlasImage == nullptr)
	{
		_textureAtlasImage = new Image(512, 512);
		_permamentPos = glm::ivec2(0, 0);
		_permanentHeight = 0;
		_discardablePos = glm::ivec2(0, _textureAtlasImage->GetHeight());
		_discardableHeight = 0;
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
			_discardablePos = glm::ivec2(0, atlasSize.y - _discardableHeight);

		position = glm::ivec2(_discardablePos.x, _discardablePos.y - imageSize.y);

		_discardablePos.x += imageSize.x;
		_discardableHeight = glm::max(_discardableHeight, atlasSize.y - position.y);
	}

	bounds2i bounds = bounds2i(position, position + imageSize);
	_textureAtlasImage->Copy(image, position.x, position.y);
	_dirty = true;

	return NewTextureImage(discardable, bounds, bounds);
}


TextureSheet TextureAtlas::AddTextureSheet(const Image& image)
{
	TextureImage* textureImage = AddTextureImage(image, TextureImageType::Permanent);
	return TextureSheet(this, textureImage->GetOuterBounds());
}


TextureSheet TextureAtlas::GetTextureSheet(const bounds2f& bounds)
{
	return TextureSheet(this, bounds);
}


TextureImage* TextureAtlas::NewTextureImage(bool discardable, const bounds2f& inner, const bounds2f& outer)
{
	TextureImage* result = new TextureImage();

	result->_textureAtlas = this;
	result->_discardable = discardable;
	result->_inner = inner;
	result->_outer = outer;

	_textureImages.push_back(result);

	return result;
}


/***/


TextureImage::TextureImage() :
	_textureAtlas(nullptr),
	_discardable(false),
	_discarded(false)
{
}


TextureImage::~TextureImage()
{

}


bool TextureImage::IsDiscarded() const
{
	return _discarded;
}


bounds2f TextureImage::GetInnerBounds() const
{
	return _inner;
}


bounds2f TextureImage::GetOuterBounds() const
{
	return _outer;
}


bounds2f TextureImage::GetInnerUV() const
{
	return _inner / glm::vec2(_textureAtlas->_textureAtlasImage->GetWidth(), _textureAtlas->_textureAtlasImage->GetHeight());
}


bounds2f TextureImage::GetOuterUV() const
{
	return _outer / glm::vec2(_textureAtlas->_textureAtlasImage->GetWidth(), _textureAtlas->_textureAtlasImage->GetHeight());
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


TextureImage* TextureSheet::NewTextureImage(int u0, int v0, int size_u, int size_v)
{
	bounds2f bounds = bounds2f(u0, v0, u0 + size_u, v0 + size_v) + _sheetBounds.min;

	return _textureAtlas->NewTextureImage(false, bounds, bounds);
}


TextureImage* TextureSheet::NewTexturePatch(int u0, int v0, int size_u, int size_v, int inset_u, int inset_v)
{
	bounds2f outer = bounds2f(u0, v0, u0 + size_u, v0 + size_v) + _sheetBounds.min;
	bounds2f inner = outer.grow(-(float)inset_u, -(float)inset_v);

	return _textureAtlas->NewTextureImage(false, inner, outer);
}
