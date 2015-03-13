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
#ifndef __CAMERA__
#define __CAMERA__
#include "stdafx.h"

#include <math.h>

class OpenGLContext;
#include "OpenGLContext.h"

class Matrix;
#include "Matrix.h"

class Frustum;
#include "Frustum.h"

template <class T> class Vector3;
#include "Vector3.h"

template <class T> class Quaternion;
#include "Quaternion.h"

class Camera
{
public:
	Camera(OpenGLContext* context, bool is_fps);
	Camera(OpenGLContext* context, bool is_fps, const Vector3<pfd>& eye, const Vector3<pfd>& center, const Vector3<pfd>& up);
	~Camera(void);
	void rotateCamera(Vector3<pfd> vec, pfd angle);
	void setPosition(Vector3<pfd> pos);
	Vector3<pfd>& getPosition();
	void lookAt(const Vector3<pfd>& lookat);
	Vector3<pfd>& getLookAt();
	Vector3<pfd>& getDir();
	Vector3<pfd>& getUp();
	Vector3<pfd>& getRight();
	void getMouseInput();
	void CalcVectors();
	void setSensitivity(pfd sens);
	const Quaternion<pfd>& getRotation();
	const Quaternion<pfd> getInverseRotation();
	const Matrix& createProjectionMatrix();
	const Matrix& createViewMatrix();
	const Matrix& getProjectionMatrix();
	const Matrix& getViewMatrix();
	void update();
	bool hasMoved();
	Frustum _frustum;
private:
	OpenGLContext* _glContext;
	Vector3<pfd> _eye;
	Vector3<pfd> _center;
	Vector3<pfd> _dir;
	Vector3<pfd> _right;
	Vector3<pfd> _up;
	bool _fps;
	pfd _sensitivity;
	Quaternion<pfd> _rotation;
	Matrix _projectionMatrix;
	Matrix _viewMatrix;
	char _hasMoved;
};

#endif

