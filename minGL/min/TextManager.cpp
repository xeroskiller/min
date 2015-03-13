/* Copyright (c) <2013-2015> <Aaron Springstroh, Min() Game Engine>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

This file is part of Min() Game Engine.

Min() Game Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Min() Game Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Min() Game Engine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"
#include "TextManager.h"
#include "OpenGLContext.h"
#include "SceneManager.h"
#include "Texture.h"
#include "TextureManager.h"

TextManager::TextManager(OpenGLContext* context)
{
	_glContext = context;
	_texture = 0;
	_Text2DId = 0;
	_textureLayer = 0;
}


TextManager::~TextManager()
{
	if(_texture != 0)
	{
		delete _texture;
	}
	std::vector<Text*>::iterator k = _textBuffer.begin();
	for(; k!= _textBuffer.end();)
	{
		delete (*k);
		k = _textBuffer.erase(k);
	}
}

void TextManager::Initialize(const std::string& textTablePath)
{
	_texture = new Texture();
	size_t dot;
	char ch;
	for(size_t i=0; i< textTablePath.size(); i++)
	{
		ch = textTablePath[i];
		if(ch == '.')
		{				
			dot = i;
			dot++;
			break;
		}
	}
	std::string ext;
	for(size_t i = dot; i<textTablePath.size(); i++)
	{
		ext += textTablePath[i];
	}
	if(ext == "bmp" || ext == "BMP")
	{
		_invertNormal = false;
		_texture->LoadBMPTexture(textTablePath);
	}
	else if(ext == "dds" || ext == "DDS")
	{
		_invertNormal = true;
		_texture->LoadDDSTexture(textTablePath);
	}

	_textureLayer = _glContext->tlmgr->requestNewTextureLayer();
	glGenTextures(1, &_Text2DId);
	glBindTexture(GL_TEXTURE_2D, _Text2DId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	if(_texture->Format() == GL_RGB8)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, _texture->Format(), _texture->Width(), _texture->Height(), 0, GL_BGR, GL_UNSIGNED_BYTE, _texture->Data());
	}
	else
	{
		unsigned int blockSize = (_texture->Format() == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;
		//load all mipmaps
		unsigned int width = _texture->Width();
		unsigned int height = _texture->Height();
		for(unsigned int level = 0; level < _texture->MipMapCount() &&  (width || height); ++level)
		{
			unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, _texture->Format(), width, height, 0, size, _texture->Data() + offset);
				
			offset += size;
			width  /= 2;
			height /= 2;
		}
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	check_glError(_glContext, L"OpenGL error", L"Error: TextureManager could not initialize.");
}

Text* TextManager::addText()
{
	Text* text = new Text(_glContext, this);
	_textBuffer.push_back(text);
	return text;
}

Text* TextManager::GetText(size_t n)
{
	return _textBuffer[n];
}

void TextManager::drawText()
{
	std::vector<Text*>::iterator k = _textBuffer.begin();
	Text* current = 0;
	for(; k != _textBuffer.end(); k++)
	{
		current = (*k);
		glBindVertexArray(current->getVaoId());
		GLuint progId = _glContext->smgr->getProgramId(current->getProgramIndex());
		glUseProgram(progId);

		_glContext->tlmgr->switchTexture(_textureLayer, current->TEXTURE_LOCATION, _Text2DId);

		glDrawElements(GL_TRIANGLES, current->getIndexCount(), GL_UNSIGNED_SHORT, (void*)0);
	}
	glUseProgram(0);
	glBindVertexArray(0);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not draw text objects.");
}

bool TextManager::isInvertNormal()
{
	return _invertNormal;
}

