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
#include "UIObject.h"
#include "Matrix3.h"
#include "GLUIObject.h"
#include "glLocationManager.h"
#include "OpenGLContext.h"
#include "minutils.h"

UIObject::UIObject(OpenGLContext& glContext)
{
	_glContext = &glContext;
	_programIndex = 0;
	_objectIndex = 0;
	_progId = 0;
	_mat = Matrix3<pfd>(Matrix3<pfd>::IDENTITY_MATRIX);
	_scale = Vector2<pfd>(1,1);
	_isScaled = false;
	_glUIObject = 0;
	_isUpdated = false;
}

UIObject::~UIObject() {}

void UIObject::createObject(GLUIObject* glUIObject)
{
	if(glUIObject->isCreated())
	{
		_glUIObject = glUIObject;
	}
}

const Matrix2<pfd>& UIObject::getRotation()
{
	return _rotation;
}

void UIObject::rotateObj(const Matrix2<pfd>& mat)
{
	_isUpdated = false;
	_rotation.MultiplyMatrices(&mat);
}

void UIObject::setRotation(const Matrix2<pfd>& mat)
{
	_isUpdated = false;
	_rotation = mat;
}

const Vector2<pfd>& UIObject::getPosition()
{
	return _position;
}

void UIObject::setPosition(const Vector2<pfd>& pos)
{
	_isUpdated = false;
	Vector2<pfd> temp = pos;
	size_t w = _glContext->windowWidth;
	size_t h = _glContext->windowHeight;

	temp._arr[0] = ((pfd)2.0/w)*temp._arr[0] - 1;
	temp._arr[1] = ((pfd)2.0/h)*temp._arr[1] - 1;

	_position = temp;
}

const Vector2<pfd>& UIObject::getScale()
{
	return _scale;
}

void UIObject::setScale(const Vector2<pfd>& scale)
{
	_isUpdated = false;
	_scale = scale;
}

void UIObject::setModel()
{
	if(!_isUpdated)
	{
		if(_isScaled)
		{
			_mat = Matrix3<pfd>(_position, _rotation, _scale);
		}
		else
		{
			_mat = Matrix3<pfd>(_position, _rotation);
		}
		size_t mm = _glUIObject->getGLLocManager()->getULocation(glLocationManager::MODEL_MATRIX_LOCATION);
		glUniformMatrix3fv(mm, 1 , false, _mat._mat);
		_isUpdated = true;
	}
}

bool UIObject::isUpdated()
{
	return _isUpdated;
}

void UIObject::update()
{
	setModel();
}

const Matrix3<pfd>* UIObject::getModelMatrix()
{
	return &(_mat);
}

void UIObject::setProgram(GLuint progId, size_t progIdIndex)
{
	_progId = progId;
	_programIndex = progIdIndex;
	GLuint ret;
	ret = glGetUniformLocation(progId, "ModelMatrix");
	_glUIObject->getGLLocManager()->assignULocation(glLocationManager::MODEL_MATRIX_LOCATION, ret);
	if(ret == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get ModelMatrix Uniform.");
	}
	if(_glUIObject->isTexture())
	{
		ret = glGetUniformLocation(progId, "Texture");
		_glUIObject->getGLLocManager()->assignULocation(glLocationManager::TEXTURE1_LOCATION, ret);
		if(ret == -1)
		{
			fatal_error(_glContext, L"OpenGL error", L"Error: Could not get Texture Uniform.");
		}
	}
	check_glError(_glContext, L"OpenGL error", L"Error: Set program in object error.");
}

size_t UIObject::getProgramIndex()
{
	return _programIndex;
}

size_t UIObject::getObjectIndex()
{
	return _objectIndex;
}

GLUIObject* UIObject::getGLUIObject()
{
	return _glUIObject;
}