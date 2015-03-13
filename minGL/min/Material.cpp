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
#include "Material.h"
#include <cassert>

Material::Material()
{
	_colors = 0;
	_colorSize = 0;
	for( size_t i=0; i < MAX_TEXTURE; i++)
	{
		_texture[i] = 0;
	}
}

Material::Material(Texture* text)
{
	_colors = 0;
	_colorSize = 0;
	for( size_t i=0; i < MAX_TEXTURE; i++)
	{
		_texture[i] = 0;
	}
	_texture[0] = text;
}

Material::Material(Vector4<pfd>* color, size_t colorsize)
{
	_colors = color;
	_colorSize = colorsize;
	for( size_t i=0; i < MAX_TEXTURE; i++)
	{
		_texture[i] = 0;
	}
}

Material::~Material()
{
	for( size_t i=0; i < MAX_TEXTURE; i++)
	{
		if(_texture[i] != 0)
		{
			delete _texture[i];
		}
	}
	if(_colors)
		delete _colors;
}

const Vector4<pfd>* Material::GetColor() const
{
	return _colors;
}

const size_t Material::GetColorSize() const 
{
	return _colorSize;
}

void Material::SetColor(Vector4<pfd>* color, size_t size)
{
	_colors = color;
	_colorSize = size;
}

Texture* Material::GetTexture(size_t n)
{
	assert(n < MAX_TEXTURE);
	return _texture[n];
}

void Material::SetTexture(Texture* text, size_t n)
{
	assert(n < MAX_TEXTURE);
	if(_texture[n] != 0)
	{
		delete _texture[n];
	}
	_texture[n] = text;
}