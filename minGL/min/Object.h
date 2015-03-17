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
#ifndef __OBJECT__
#define __OBJECT__
#include "stdafx.h"
class OpenGLContext;
#include "OpenGLContext.h"
class SceneManager;
#include "SceneManager.h"
class Matrix;
#include "Matrix.h"
class GLObject;
#include "glLocationManager.h"

template <class T> class aabbox3;
#include "bsphere.h"
#include <string>
#include <deque>
#include <vector>
class Object
{
public:
	friend class SceneManager;
	virtual ~Object(void);
	virtual void createObject(std::vector<GLObject*>& glObjList);
	void rotateObj(const Quaternion<pfd>& q);
	virtual void setRotation(const Quaternion<pfd>& q);
	virtual void setPosition(const Vector3<pfd>& pos);
	virtual void setScale(const Vector3<pfd>& scale);
	virtual std::deque<Object*>::iterator Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k);
	const Quaternion<pfd>& getRotation();
	const Vector3<pfd>& getPosition();
	const Vector3<pfd>& getScale();
	void ApplyMatrix(Matrix*, const glLocationManager::glLocationEnum);
	void setMaterialProperties(const Vector3<pfd>& mat);
	void setProjection(Camera& cam);
	void setView(Camera& cam);
	void setModel();
	const Matrix* getModelMatrix();
	void setModelMatrix(Matrix& mat);
	void setProgram(GLuint progId, size_t progIdIndex);
	size_t getProgramIndex();
	size_t getObjectIndex();
	void setTimeToLive(size_t seconds);
	GLObject& getGLObject(size_t n);
	bool isUpdated();
	bool testFrustumIntersection(size_t n, Frustum* frustum);
	void updateDrawTree(Frustum* frustum, const Vector3<pfd>& worldPos);
protected:
	Object(OpenGLContext&);
	OpenGLContext* _glContext;
	std::vector<GLObject*> _glObjects;
	size_t _numGLObjects;
	Matrix _mat;
	Quaternion<pfd> _rotation;
	Vector3<pfd> _position;
	Vector3<pfd> _scale;
	bool _isScaled;
	double _time_to_live;
	double _reference_time;
	Vector3<pfd> _material_property;
	GLuint _progId;
	size_t _programIndex;
	size_t _objectIndex;
	bool _isUpdated;
	bsphere<pfd>* _bSphere;

};

#endif