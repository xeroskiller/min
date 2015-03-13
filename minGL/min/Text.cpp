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
#include "Text.h"
#include "OpenGLContext.h"
#include "TextManager.h"
#include "minutils.h"

Text::Text(OpenGLContext* context, TextManager* textManager)
{
	_glContext = context;
	_textManager = textManager;
	TEXTURE_LOCATION = 0;
	_progIdIndex = 0;
	_vertices = 0;
	_uv = 0;
	_indices = 0;
	_vbVertId = 0;
	_vbIndexId = 0;
	_vbUVID = 0;
	textureLayer = 0;
	_created = false;
}

Text::~Text(void)
{
	delete [] _vertices;
	delete [] _uv;
	delete [] _indices;
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &_vbVertId);
	glDeleteBuffers(1, &_vbIndexId);
	glDeleteBuffers(1, &_vbUVID);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_vaoId);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not delete a VBO");
}

void Text::createText(const std::string& str, size_t x, size_t y, size_t spacing)
{
	if(!_created)
	{
		size_t size = str.size();
		_vertices = new Vector2<pfd>[4*size];
		_uv = new Vector2<pfd>[4*size];
		_indices = new GLIndex[6*size];
		_indexCount = 6*size;
		char ch;
		pfd uv_x;
		pfd uv_y;
		size_t ivert=0;
		size_t iuv=0;
		size_t iind=0;
		size_t w = _glContext->windowWidth;
		size_t h = _glContext->windowHeight;

		for(size_t i=0; i<str.size(); i++)
		{
			_indices[iind]   = static_cast<GLIndex>(ivert);
			_indices[iind+1] = static_cast<GLIndex>(ivert + 3);
			_indices[iind+2] = static_cast<GLIndex>(ivert + 1);

			_indices[iind+3] = static_cast<GLIndex>(ivert + 2);
			_indices[iind+4] = static_cast<GLIndex>(ivert + 1);
			_indices[iind+5] = static_cast<GLIndex>(ivert + 3);
			iind += 6;

			_vertices[ivert]   = Vector2<pfd>(static_cast<pfd>(x + i*spacing)*(2.0f/w) - 1.0f,           static_cast<pfd>(y+spacing)*(2.0f/h) - 1.0f);
			_vertices[ivert+1] = Vector2<pfd>(static_cast<pfd>(x + i*spacing + spacing)*(2.0f/w) - 1.0f, static_cast<pfd>(y+spacing)*(2.0f/h) - 1.0f);
			_vertices[ivert+2] = Vector2<pfd>(static_cast<pfd>(x + i*spacing + spacing)*(2.0f/w) - 1.0f, static_cast<pfd>(y)*(2.0f/h) - 1.0f);
			_vertices[ivert+3] = Vector2<pfd>(static_cast<pfd>(x + i*spacing)*(2.0f/w) - 1.0f,           static_cast<pfd>(y)*(2.0f/h) - 1.0f);
	
			ivert +=4;
			ch = str[i];
			uv_x = (ch%16)/16.0f;
			uv_y = (ch/16)/16.0f;

			if(!_textManager->isInvertNormal())
			{
				_uv[iuv]   = Vector2<pfd>(uv_x,                1.0f-uv_y);
				_uv[iuv+1] = Vector2<pfd>(uv_x + (1.0f/16.0f), 1.0f-uv_y);
				_uv[iuv+2] = Vector2<pfd>(uv_x + (1.0f/16.0f), 1.0f- (uv_y + 1.0f/16.0f));
				_uv[iuv+3] = Vector2<pfd>(uv_x,                1.0f- (uv_y + 1.0f/16.0f));
			}
			else
			{
				_uv[iuv]   = Vector2<pfd>(uv_x,              uv_y);
				_uv[iuv+1] = Vector2<pfd>(uv_x + 1.0f/16.0f, uv_y);
				_uv[iuv+2] = Vector2<pfd>(uv_x + 1.0f/16.0f, uv_y + 1.0f/16.0f);
				_uv[iuv+3] = Vector2<pfd>(uv_x,              uv_y + 1.0f/16.0f);
			}
			iuv += 4;
		}
		glGenVertexArrays(1, &_vaoId);
		glBindVertexArray(_vaoId);

		glGenBuffers(1, &_vbVertId);
		glBindBuffer(GL_ARRAY_BUFFER, _vbVertId);
		glBufferData(GL_ARRAY_BUFFER, 8 * size * sizeof(GLfloat), _vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); //GL_MAX_VERTEX_ATTRIBS is LIMIT
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &_vbUVID);
		glBindBuffer(GL_ARRAY_BUFFER, _vbUVID);
		glBufferData(GL_ARRAY_BUFFER, 8 * size * sizeof(GLfloat), _uv, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &_vbIndexId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbIndexId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6* size * sizeof(GLIndex), _indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
		check_glError(_glContext, L"OpenGL error", L"Error: Could not create Text 2D");
		_created  = true;
	}
}

void Text::editText(const std::string& str, size_t x, size_t y, size_t spacing)
{
	size_t size = str.size();
	delete [] _vertices;
	delete [] _uv;
	delete [] _indices;
	_vertices = new Vector2<pfd>[4*size];
	_uv = new Vector2<pfd>[4*size];
	_indices = new GLIndex[6*size];
	_indexCount = 6*size;
	char ch;
	pfd uv_x;
	pfd uv_y;
	size_t ivert=0;
	size_t iuv=0;
	size_t iind=0;
	size_t w = _glContext->windowWidth;
	size_t h = _glContext->windowHeight;

	for(size_t i=0; i<str.size(); i++)
	{
		_indices[iind]   = static_cast<GLIndex>(ivert);
		_indices[iind+1] = static_cast<GLIndex>(ivert + 3);
		_indices[iind+2] = static_cast<GLIndex>(ivert + 1);

		_indices[iind+3] = static_cast<GLIndex>(ivert + 2);
		_indices[iind+4] = static_cast<GLIndex>(ivert + 1);
		_indices[iind+5] = static_cast<GLIndex>(ivert + 3);
		iind += 6;

		_vertices[ivert]   = Vector2<pfd>(static_cast<pfd>(x + i*spacing)*(2.0f/w) - 1.0f,           static_cast<pfd>(y+spacing)*(2.0f/h) - 1.0f);
		_vertices[ivert+1] = Vector2<pfd>(static_cast<pfd>(x + i*spacing + spacing)*(2.0f/w) - 1.0f, static_cast<pfd>(y+spacing)*(2.0f/h) - 1.0f);
		_vertices[ivert+2] = Vector2<pfd>(static_cast<pfd>(x + i*spacing + spacing)*(2.0f/w) - 1.0f, static_cast<pfd>(y)*(2.0f/h) - 1.0f);
		_vertices[ivert+3] = Vector2<pfd>(static_cast<pfd>(x + i*spacing)*(2.0f/w) - 1.0f,           static_cast<pfd>(y)*(2.0f/h) - 1.0f);
	
		ivert +=4;
		ch = str[i];
		uv_x = (ch%16)/16.0f;
		uv_y = (ch/16)/16.0f;

		if(!_textManager->isInvertNormal())
		{
			_uv[iuv]   = Vector2<pfd>(uv_x,                1.0f-uv_y);
			_uv[iuv+1] = Vector2<pfd>(uv_x + (1.0f/16.0f), 1.0f-uv_y);
			_uv[iuv+2] = Vector2<pfd>(uv_x + (1.0f/16.0f), 1.0f- (uv_y + 1.0f/16.0f));
			_uv[iuv+3] = Vector2<pfd>(uv_x,                1.0f- (uv_y + 1.0f/16.0f));
		}
		else
		{
			_uv[iuv]   = Vector2<pfd>(uv_x,              uv_y);
			_uv[iuv+1] = Vector2<pfd>(uv_x + 1.0f/16.0f, uv_y);
			_uv[iuv+2] = Vector2<pfd>(uv_x + 1.0f/16.0f, uv_y + 1.0f/16.0f);
			_uv[iuv+3] = Vector2<pfd>(uv_x,              uv_y + 1.0f/16.0f);
		}
		iuv += 4;
	}
	glBindVertexArray(_vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, _vbVertId);
	glBufferData(GL_ARRAY_BUFFER, 8 * size * sizeof(GLfloat), _vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); //GL_MAX_VERTEX_ATTRIBS is LIMIT
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbUVID);
	glBufferData(GL_ARRAY_BUFFER, 8 * size * sizeof(GLfloat), _uv, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbIndexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6* size * sizeof(GLIndex), _indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not create Text 2D");
}

void Text::setProgram(GLuint progId, size_t progIdIndex)
{
	_progIdIndex = progIdIndex;
	TEXTURE_LOCATION = glGetUniformLocation(progId, "TextTable2D");
	if(TEXTURE_LOCATION == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get Light Uniform.");
	}
	check_glError(_glContext, L"OpenGL error", L"Error: Set program in object error.");
}

GLuint Text::getVaoId()
{
	return _vaoId;
}

size_t Text::getIndexCount()
{
	return _indexCount;
}

GLuint Text::getProgramIndex()
{
	return _progIdIndex;
}