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
#include "StdAfx.h"
#include "Object.h"
#include "SystemClock.h"
#include "GLObject.h"
#include "minutils.h"

Object::Object(OpenGLContext& glContext)
{
	_glContext = &glContext;
	_programIndex = 0;
	_objectIndex = 0;
	_progId = 0;
	_mat = Matrix(IDENTITY_MATRIX);
	_scale = Vector3<pfd>(1,1,1);
	_isScaled = false;
	_time_to_live = -1;
	_reference_time = -1;
	_isUpdated = false;
	_numGLObjects = 0;
}

Object::~Object(void) 
{
	if (_bSphere)
		delete [] _bSphere;
}

void Object::createObject(std::vector<GLObject*>& glObjList)
{
	_numGLObjects = glObjList.size();
	if(_numGLObjects > 0)
	{
		GLObject* glObj = 0;
		_glObjects = glObjList;
		_bSphere = new bsphere<pfd>[_numGLObjects];
		for (size_t i = 0; i < _numGLObjects; i++)
		{
			glObj = (glObjList)[i];
			_bSphere[i] = glObj->getBaseFrustumSphere();
		}
	}
}

const Quaternion<pfd>& Object::getRotation()
{
	return _rotation;
}

void Object::rotateObj(const Quaternion<pfd>& q)
{
	_isUpdated = false;
	_rotation = _rotation*q;
}

void Object::setRotation(const Quaternion<pfd>& q)
{
	_isUpdated = false;
	_rotation = q;
}

const Vector3<pfd>& Object::getPosition()
{
	return _position;
}

void Object::setPosition(const Vector3<pfd>& pos)
{
	_isUpdated = false;
	for (size_t i = 0; i< _numGLObjects; i++)
	{
		_bSphere[i].setPosition((_bSphere[i].getCenter() - this->getPosition()) + pos);
	}
	_position = pos;
}

const Vector3<pfd>& Object::getScale()
{
	return _scale;
}

void Object::setScale(const Vector3<pfd>& scale)
{
	_isUpdated = false;
	_isScaled = true;
	_scale = scale;
}

void Object::ApplyMatrix(Matrix* mat, const glLocationManager::glLocationEnum mode)
{
	GLObject* glObj = 0;
	for(size_t i=0; i < _numGLObjects; i++)
	{
		glObj = _glObjects[i];
		size_t loc = glObj->getGLLocManager()->getULocation(mode);
		glUniformMatrix4fv(loc, 1 , false, mat->_mat);
	}
}

void Object::setMaterialProperties(const Vector3<pfd>& mat)
{
	glUseProgram(_progId);
	_material_property = mat;
	GLObject* glObj = 0;
	for(size_t i=0; i < _numGLObjects; i++)
	{
		glObj = _glObjects[i];
		size_t mp = glObj->getGLLocManager()->getULocation(glLocationManager::MATERIAL_LOCATION);
		glUniform3fv(mp, 1 , _material_property._arr);
	}
	glUseProgram(0);
}

void Object::setProjection(Camera& cam)
{
	const Matrix& persp = cam.getProjectionMatrix();
	GLObject* glObj = 0;
	for(size_t i=0; i < _numGLObjects; i++)
	{
		glObj = _glObjects[i];
		size_t pm = glObj->getGLLocManager()->getULocation(glLocationManager::PROJECTION_MATRIX_LOCATION);
		glUniformMatrix4fv(pm, 1 , false, persp._mat);
	}
}

void Object::setView(Camera& cam)
{
	const Matrix& view = cam.getViewMatrix();
	GLObject* glObj = 0;
	for(size_t i=0; i < _numGLObjects; i++)
	{
		glObj = _glObjects[i];
		size_t vm = glObj->getGLLocManager()->getULocation(glLocationManager::VIEW_MATRIX_LOCATION);
		glUniformMatrix4fv(vm, 1 , false, view._mat);
	}
}

void Object::setModel()
{
	if(!_isUpdated)
	{
		if(_isScaled)
		{
			_mat = Matrix(_rotation, _position, _scale);
		}
		else
		{
			_mat = Matrix(_rotation, _position);
		}
		GLObject* glObj = 0;
		for(size_t i=0; i < _numGLObjects; i++)
		{
			glObj = _glObjects[i];
			size_t mm = glObj->getGLLocManager()->getULocation(glLocationManager::MODEL_MATRIX_LOCATION);
			glUniformMatrix4fv(mm, 1 , false, _mat._mat);
		}
		_isUpdated = true;
	}
}

const Matrix* Object::getModelMatrix()
{
	return &(_mat);
}

void Object::setModelMatrix(Matrix& mat)
{
	_isUpdated = false;
	memcpy(_mat._mat, mat._mat, 16*sizeof(pfd));
}


void Object::setProgram(GLuint progId, size_t progIdIndex)
{
	_progId = progId;
	_programIndex = progIdIndex;
	GLObject* glObj = 0;
	GLuint ret;
	for(size_t i =0; i < _numGLObjects; i++)
	{
		glObj = _glObjects[i];
		ret = glGetUniformLocation(progId, "ProjectionMatrix");
		glObj->getGLLocManager()->assignULocation(glLocationManager::PROJECTION_MATRIX_LOCATION, ret);
		if(ret == -1)
		{
			fatal_error(_glContext, L"OpenGL error", L"Error: Could not get ProjectionMatrix Uniform.");
		}
		ret = glGetUniformLocation(progId, "ModelMatrix");
		glObj->getGLLocManager()->assignULocation(glLocationManager::MODEL_MATRIX_LOCATION, ret);
		if(ret == -1)
		{
			fatal_error(_glContext, L"OpenGL error", L"Error: Could not get ModelMatrix Uniform.");
		}
		ret = glGetUniformLocation(progId, "ViewMatrix");
		glObj->getGLLocManager()->assignULocation(glLocationManager::VIEW_MATRIX_LOCATION, ret);
		if(ret == -1)
		{
			fatal_error(_glContext, L"OpenGL error", L"Error: Could not get ViewMatrix Uniform.");
		}
		ret = glGetUniformLocation(progId, "MaterialProperty");
		glObj->getGLLocManager()->assignULocation(glLocationManager::MATERIAL_LOCATION, ret);
		if(ret == -1)
		{
			fatal_error(_glContext, L"OpenGL error", L"Error: Could not get Material Property Uniform.");
		}

		for(size_t i=0; i < GLObject::MAX_TEXTURE; i++)
		{
			if(glObj->isTexture(i))
			{
				std::string str = "Texture" + std::to_string(i + 1);
				ret = glGetUniformLocation(progId, str.c_str());
				glObj->getGLLocManager()->assignULocation(static_cast<glLocationManager::glLocationEnum>(glLocationManager::TEXTURE1_LOCATION + i), ret);
				if(ret == -1)
				{
					fatal_error(_glContext, L"OpenGL error", L"Error: Could not get Texture Uniform.");
				}
			}
		}
	}
	check_glError(_glContext, L"OpenGL error", L"Error: Set program in object error.");
}

size_t Object::getProgramIndex()
{
	return _programIndex;
}
size_t Object::getObjectIndex()
{
	return _objectIndex;
}

std::deque<Object*>::iterator Object::Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k)
{
	setProjection(cam);
	setView(cam);
	setModel();
	if(_time_to_live > 0)
	{
		_time_to_live -= (ref_time - _reference_time);
		_reference_time = ref_time;
		if(_time_to_live < 0)
		{
			return _glContext->smgr->removeFromScene(this);
		}
	}
	k++;
	return k;
}

void Object::setTimeToLive(size_t seconds)
{
	_time_to_live = static_cast<double>(seconds);
	_reference_time = _glContext->SysClock->GetCurrentTime();
}

GLObject& Object::getGLObject(size_t n)
{
	if (n < _numGLObjects)
	{
		return *_glObjects[n];
	}
	else
	{
		fatal_error(_glContext, L"GLObject Buffer Overrun", L"Improper index passed to getGLObject()");
	}
}

bool Object::isUpdated()
{
	return _isUpdated;
}

bool Object::testFrustumIntersection(size_t n, Frustum* frustum)
{
	return frustum->isSphereInFrustum(_bSphere[n]) != Frustum::FrustumIntersectionEnum::OUTSIDE;
}

void Object::updateDrawTree(Frustum* frustum, const Vector3<pfd>& worldPos)
{
	for (size_t i = 0; i < _numGLObjects; i++)
	{
		GLObject& glObj = getGLObject(i);
		glObj.updateDrawTree(frustum, worldPos);
	}
}