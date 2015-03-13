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
#include "TextureManager.h"

size_t TextureManager::maxTexture = 80;

TextureManager::TextureManager()
{
	_activeTexture = 0;
}

TextureManager::~TextureManager()
{

}

size_t TextureManager::requestNewTextureLayer()
{
	size_t temp = _activeTexture;
	if(_activeTexture + 1 != maxTexture)
	{
		_activeTexture++;
		return temp;
	}
	else
	{
		return -1;
	}
}

void TextureManager::switchTexture(size_t layer, size_t textLocation, GLuint textId)
{
	glActiveTexture(GL_TEXTURE0 + layer);
	glBindTexture(GL_TEXTURE_2D, textId);
	glUniform1i(textLocation, layer); //GL_TEXTURE... 0,1,2,3
}

void TextureManager::disableTexture(size_t layer)
{
	glActiveTexture(GL_TEXTURE0 + layer);
	glBindTexture(GL_TEXTURE_2D, 0);
}