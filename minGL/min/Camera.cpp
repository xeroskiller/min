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
#include "Camera.h"
#include "minutils.h"

Camera::Camera(OpenGLContext* context, bool is_fps) : _frustum(static_cast<pfd>(90.0), context->getAspectRatio(), static_cast<pfd>(0.01), static_cast<pfd>(100.0))
{
	_eye = Vector3<pfd>(0,0,0);
	_center = Vector3<pfd>(0,0,-1);
	_dir = _center - _eye;
	_dir.normalize();
	_right = Vector3<pfd>(1,0,0);
	_up = Vector3<pfd>(0,1,0);
	_fps = true;
	_glContext=context;
	ShowCursor(_fps);
	_sensitivity =  1;
	_projectionMatrix = Matrix(IDENTITY_MATRIX);
	_viewMatrix = Matrix(IDENTITY_MATRIX);
	_hasMoved = true;
}

Camera::Camera(OpenGLContext* context, bool is_fps, const Vector3<pfd>& eye, const Vector3<pfd>& center, const Vector3<pfd>& up) : _frustum(static_cast<pfd>(90.0), context->getAspectRatio(), static_cast<pfd>(0.01), static_cast<pfd>(500.0))
{
	_eye = eye;
	_center = center;
	_dir = center - eye;
	_dir.normalize();
	_right =_dir.crossProduct(up);
	_up = up;
	_fps = is_fps;
	_glContext=context;
	ShowCursor(!_fps);
	_sensitivity =  1;
	_projectionMatrix = Matrix(IDENTITY_MATRIX);
	_viewMatrix = Matrix(IDENTITY_MATRIX);
	_hasMoved = true;
}


Camera::~Camera(void)
{
}

void Camera::rotateCamera(Vector3<pfd> vec, pfd angle)
{
	_hasMoved = -1;
	Vector3<pfd> diff = _center - _eye;
	pfd mag = diff.magnitude();

	Quaternion<pfd> temp(vec, angle);
	Quaternion<pfd> view(_dir);
	Quaternion<pfd> result;
	Quaternion<pfd> conj = temp.conjugate();
	_rotation = temp*_rotation;

	result = temp*view;
	result = result*conj;

	_dir = Vector3<pfd>(result.X(), result.Y(), result.Z());
	_center = _dir*mag;
	_center += _eye;
}

Vector3<pfd>& Camera::getPosition()
{
	return _eye;
}

void Camera::setPosition(Vector3<pfd> pos)
{
	_hasMoved = -1;
	Vector3<pfd> temp = _center - _eye;
	pfd mag = temp.magnitude();
	_eye = pos;
	_center = _dir*mag;
	_center += _eye;
}

void Camera::lookAt(const Vector3<pfd>& lookat)
{
	_hasMoved = -1;
	Vector3<pfd> diff = _center - _eye;
	pfd mag = diff.magnitude();
	Quaternion<pfd> q(_dir, lookat);
	_rotation = q*_rotation;
	_dir = lookat;
	_center = _dir*mag;
	_center += _eye;
}

Vector3<pfd>& Camera::getLookAt()
{
	return _center;
}

Vector3<pfd>& Camera::getDir()
{
	return _dir;
}

Vector3<pfd>& Camera::getUp()
{
	return _up;
}

Vector3<pfd>& Camera::getRight()
{
	return _right;
}
void Camera::getMouseInput()
{
	if(_fps)
	{
		POINT p;
		BOOL ret = GetCursorPos(&p);
		if(ret != TRUE)
		{
			fatal_error(_glContext, L"WINAPI error", L"Error: WINAPI failed getting global mouse coordinates.");
		}
		else
		{
			ScreenToClient(_glContext->hwnd, &p);
			if(ret != TRUE)
			{
				fatal_error(_glContext, L"WINAPI error", L"Error: WINAPI failed getting local mouse coordinates.");
			}
		}
		pfd middleX = static_cast<pfd>(_glContext->windowWidth/2);
		pfd middleY = static_cast<pfd>(_glContext->windowHeight/2);

		pfd mouseX = static_cast<pfd>(middleX - p.x);
		pfd mouseY = static_cast<pfd>(middleY - p.y);

		if(abs(mouseX) <= 0 && abs(mouseY) <= 0)
		{	
			ClientToScreen(_glContext->hwnd, &p); //i think this is necessary because of GetMessage() should be PeekMessage()
			SetCursorPos(p.x,p.y);
			if(_hasMoved != -1)
				_hasMoved = false;
			else
				_hasMoved = true;
			return;
		}
		else
		{
			mouseX *= static_cast<pfd>(_sensitivity/middleX);
			mouseY *= static_cast<pfd>(_sensitivity/middleY);
		}

		_hasMoved = true;

		//Done with p send it back to window
		p.x = static_cast<long>(middleX);
		p.y = static_cast<long>(middleY);
		ClientToScreen(_glContext->hwnd, &p);
		SetCursorPos(p.x,p.y);

		rotateCamera(_right, mouseY);
		rotateCamera(_up, mouseX);
		CalcVectors();
	}
}

void Camera::CalcVectors()
{
	_dir = _center - _eye;
	_dir.normalize();
	_right =_dir.crossProduct(_up);
}

void Camera::setSensitivity(pfd sens)
{
	_sensitivity = sens;
}

const Quaternion<pfd>& Camera::getRotation()
{
	return _rotation;
}

const Quaternion<pfd> Camera::getInverseRotation()
{
	return _rotation.conjugate();
}

const Matrix& Camera::createProjectionMatrix()
{
	if(_hasMoved)
	{
		_frustum.glhPerspectivef2(&_projectionMatrix);
	}
	return _projectionMatrix;
}

const Matrix& Camera::createViewMatrix()
{
	if(_hasMoved)
	{
		_frustum.glhLookAtf1(&_viewMatrix, _eye, _center, _up);
	}
	return _viewMatrix;
}

void Camera::update()
{
	this->createProjectionMatrix();
	this->createViewMatrix();
}

bool Camera::hasMoved()
{
	return _hasMoved;
}

const Matrix& Camera::getProjectionMatrix()
{
	return _projectionMatrix;
}

const Matrix& Camera::getViewMatrix()
{
	return _viewMatrix;
}