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
#ifndef __UIOBJECT__
#define __UIOBJECT__

#include "stdafx.h"
class GLUIObject;
class OpenGLContext;
template <class T> class Vector2;
#include "Vector2.h"
template <class T> class Matrix2;
#include "Matrix2.h"
template <class T> class Matrix3;
#include "Matrix3.h"

class UIObject
{
public:
	friend class SceneManager;
	~UIObject(void);
	void createObject(GLUIObject* glObject);
	void rotateObj(const Matrix2<pfd>& mat);
	void setRotation(const Matrix2<pfd>& mat);
	void setPosition(const Vector2<pfd>& pos);
	void setScale(const Vector2<pfd>& scale);
	const Matrix2<pfd>& getRotation();
	const Vector2<pfd>& getPosition();
	const Vector2<pfd>& getScale();
	void update();
	void setModel();
	const Matrix3<pfd>* getModelMatrix();
	void setProgram(GLuint progId, size_t progIdIndex);
	size_t getProgramIndex();
	size_t getObjectIndex();
	GLUIObject* getGLUIObject();
	bool isUpdated();
private:
	UIObject(OpenGLContext&);
	OpenGLContext* _glContext;
	GLUIObject* _glUIObject;

	Matrix3<pfd> _mat;
	Matrix2<pfd> _rotation;
	Vector2<pfd> _position;
	Vector2<pfd> _scale;
	bool _isScaled;
	
	GLuint _progId;
	size_t _programIndex;
	size_t _objectIndex;

	bool _isUpdated;
};

#endif