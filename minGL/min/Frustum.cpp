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
#include "Frustum.h"

const pfd Frustum::PI = 3.14159265358979323846f;

Frustum::Frustum(pfd fovy, pfd aspectRatio, pfd nearPlane, pfd farPlane) : _sphere(Vector3<pfd>(0,0,0), 1)
{
	_fovy = fovy;
	_aspectRatio = aspectRatio;
	_nearPlane = nearPlane;
	_farPlane = farPlane;
}

Frustum::~Frustum()
{

}

pfd Frustum::Cotangent(pfd angle)
{
	return (pfd)(1.0 / tan(angle));
}

pfd Frustum::DegreesToRadians(pfd degrees)
{
	return degrees*(pfd)(PI/180);
}

pfd Frustum::RadiansToDegrees(pfd radians)
{
	return radians * (pfd)(180/PI);
}

void Frustum::glhPerspectivef2(Matrix* matrix)
{
	pfd tang = tanf(_fovy * PI / 360.0f);
    _nearHeight = _nearPlane * tang; //ymax
    _nearWidth  = _nearHeight * _aspectRatio; //xmax
    _farHeight = _farPlane * tang;
	_farWidth = _farHeight * _aspectRatio; 
    glhFrustumf2(matrix, -_nearWidth, _nearWidth, -_nearHeight, _nearHeight, _nearPlane, _farPlane);
}

void Frustum::glhFrustumf2(Matrix* matrix, pfd left, pfd right, pfd bottom, pfd top, pfd znear, pfd zfar)
{
    pfd temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix->_mat[0] = temp / temp2;
    matrix->_mat[1] = 0.0;
    matrix->_mat[2] = 0.0;
    matrix->_mat[3] = 0.0;
    matrix->_mat[4] = 0.0;
    matrix->_mat[5] = temp / temp3;
    matrix->_mat[6] = 0.0;
    matrix->_mat[7] = 0.0;
    matrix->_mat[8] = (right + left) / temp2;
    matrix->_mat[9] = (top + bottom) / temp3;
    matrix->_mat[10] = (-zfar - znear) / temp4;
    matrix->_mat[11] = -1.0;
    matrix->_mat[12] = 0.0;
    matrix->_mat[13] = 0.0;
    matrix->_mat[14] = (-temp * zfar) / temp4;
    matrix->_mat[15] = 0.0;
}


void Frustum::glhLookAtf1( Matrix *matrix, const Vector3<pfd>& eyePosition3D, const Vector3<pfd>& center3D, const Vector3<pfd>& upVector3D )
{
   Vector3<pfd> forward, side, up;

   //------------------
   forward = center3D - eyePosition3D;
   forward.normalize();
   //------------------
   //Side = forward x up
   side = forward.crossProduct(upVector3D);
   side.normalize();
   //------------------
   //Recompute up as: up = side x forward
   up = side.crossProduct(forward);
   //------------------

   _nearCenter = eyePosition3D + forward * _nearPlane;
   _farCenter = eyePosition3D + forward * _farPlane;

	_corners[0] = _nearCenter + up * _nearHeight - side * _nearWidth; //TOP LEFT
	_corners[1] = _nearCenter + up * _nearHeight + side * _nearWidth; //TOP RIGHT
	_corners[2] = _nearCenter - up * _nearHeight - side * _nearWidth; //BOTTOM LEFT
	_corners[3] = _nearCenter - up * _nearHeight + side * _nearWidth; //BOTTOM RIGHT

	_corners[4] = _farCenter + up * _farHeight - side * _farWidth; //FAR TOP LEFT
	_corners[5] = _farCenter + up * _farHeight + side * _farWidth; //FAR TOP RIGHT
	_corners[6] = _farCenter - up * _farHeight - side * _farWidth; //FAR BOTTOM LEFT
	_corners[7] = _farCenter - up * _farHeight + side * _farWidth; //FAR BOTTOM RIGHT

	Vector3<pfd> center = (_nearCenter + _farCenter)/2;
	_sphere = bsphere<pfd>(center, (_farPlane - _nearPlane)/2);

	_plane[TOP]    = Plane<pfd>(_corners[1], _corners[0], _corners[4]);
	_plane[BOTTOM] = Plane<pfd>(_corners[2], _corners[3], _corners[7]);
	_plane[LEFT]   = Plane<pfd>(_corners[0], _corners[2], _corners[6]);

	_plane[RIGHT]  = Plane<pfd>(_corners[3], _corners[1], _corners[7]);
	_plane[NEARP]  = Plane<pfd>(_corners[0], _corners[1], _corners[3]);
	_plane[FARP]   = Plane<pfd>(_corners[5], _corners[4], _corners[6]);

   matrix->_mat[0] = side._arr[0];
   matrix->_mat[4] = side._arr[1];
   matrix->_mat[8] = side._arr[2];
   matrix->_mat[12] = 0.0;
   //------------------
   matrix->_mat[1] = up._arr[0];
   matrix->_mat[5] = up._arr[1];
   matrix->_mat[9] = up._arr[2];
   matrix->_mat[13] = 0.0;
   //------------------
   matrix->_mat[2] = -forward._arr[0];
   matrix->_mat[6] = -forward._arr[1];
   matrix->_mat[10] = -forward._arr[2];
   matrix->_mat[14] = 0.0;
   //------------------
   matrix->_mat[3] = matrix->_mat[7] = matrix->_mat[11] = 0.0;
   matrix->_mat[15] = 1.0;
   //------------------
   matrix->TranslateMatrix(Vector3<pfd>(0,0,0) - eyePosition3D); //translate object to origin
}

void Frustum::glhLookAtf2(Matrix *matrix, const Vector3<pfd>& eyePosition3D, const Vector3<pfd>& center3D, const Vector3<pfd>& upVector3D )
{	
   Vector3<pfd> forward, side, up;
   Matrix matrix2;
   *matrix = Matrix(IDENTITY_MATRIX);
   //------------------
   forward = center3D - eyePosition3D;
   forward.normalize();
   //------------------
   //Side = forward x up
   side = forward.crossProduct(upVector3D);
   side.normalize();
   //------------------
   //Recompute up as: up = side x forward
   up = side.crossProduct(forward);
   //------------------
   matrix2._mat[0] = side._arr[0];
   matrix2._mat[4] = side._arr[1];
   matrix2._mat[8] = side._arr[2];
   matrix2._mat[12] = -eyePosition3D._arr[0];
   //------------------
   matrix2._mat[1] = up._arr[0];
   matrix2._mat[5] = up._arr[1];
   matrix2._mat[9] = up._arr[2];
   matrix2._mat[13] = -eyePosition3D._arr[1];
   //------------------
   matrix2._mat[2] = -forward._arr[0];
   matrix2._mat[6] = -forward._arr[1];
   matrix2._mat[10] = -forward._arr[2];
   matrix2._mat[14] = -eyePosition3D._arr[2];
   //------------------
   matrix2._mat[3] = matrix2._mat[7] = matrix2._mat[11] = 0.0;
   matrix2._mat[15] = 1.0;
   //------------------
   matrix->_mat[12] = eyePosition3D._arr[0];
   matrix->_mat[13] = eyePosition3D._arr[1];
   matrix->_mat[14] = eyePosition3D._arr[2];

   matrix->MultiplyMatrices(&matrix2);
}

Frustum::FrustumIntersectionEnum Frustum::isPointInFrustum(const Vector3<pfd>& p)
{
	FrustumIntersectionEnum result = INSIDE;
	for (size_t i = 0; i < 6; i++)
	{
		if(_plane[i].distance(p) < 0)
		{
			return OUTSIDE;
		}
	}
	return result;
}

Frustum::FrustumIntersectionEnum Frustum::isSphereInFrustum(const bsphere<pfd>& s)
{
	pfd distance;
	FrustumIntersectionEnum result = INSIDE;
	for(size_t i=0; i < 6; i++)
	{
		distance = _plane[i].distance(s._center);
		if (distance < -s._radius)
		{
			return OUTSIDE;
		}
		else if (distance < s._radius)
		{
			result =  INTERSECT;
		}
	}
	return(result);
}

Frustum::FrustumIntersectionEnum Frustum::isAABBInFrustum(const aabbox3<pfd>& b)
{
	if(_sphere.intersectsWithAABB(b))
		return INTERSECT;

	Vector3<pfd> min = b.getMin();
	Vector3<pfd> max = b.getMax();

	Vector3<pfd> normal;
	Vector3<pfd> p = min;
	Vector3<pfd> n = max;

	FrustumIntersectionEnum result = INSIDE;
	//for each plane do ...
	for(int i=0; i < 6; i++)
	{
		normal = _plane[i].getNormal();
		p = min;
		n = max;
		if(normal.X() >= 0)
		{
			p._arr[0] = max.X();
			n._arr[0] = min.X();
		}
		if(normal.Y() >= 0)
		{
			p._arr[1] = max.Y();
			n._arr[1] = min.Y();
		}
		if(normal.Z() >= 0)
		{
			p._arr[2] = max.Z();
			n._arr[2] = min.Z();
		}
		// is the positive vertex outside?
		if (_plane[i].distance(p) < 0)
			return OUTSIDE;
		// is the negative vertex outside?	
		else if (_plane[i].distance(n) < 0)
			result =  INTERSECT;
	}
	return(result);
	
}

void Frustum::setFovy(pfd fovy)
{
	_fovy = fovy;
}

void Frustum::setNearPlane(pfd np)
{
	_nearPlane = np;
}

void Frustum::setFarPlane(pfd fp)
{
	_farPlane = fp;
}