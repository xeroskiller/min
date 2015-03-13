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
#ifndef __FRUSTUM__
#define __FRUSTUM__
#include "stdafx.h"

#include "Plane.h"
#include "Matrix.h"
#include "Vector3.h"
#include "bsphere.h"
#include "aabbox3.h"
#include <math.h>

class Frustum
{
public:
	static const pfd PI;
	enum FrustumPlaneEnum: char {TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP};
	enum FrustumIntersectionEnum: char {OUTSIDE=0, INTERSECT, INSIDE};
	Frustum(pfd fovy, pfd aspectRatio, pfd nearPlane, pfd farPlane);
	~Frustum();
	pfd Cotangent(pfd angle);
	pfd DegreesToRadians(pfd degrees);
	pfd RadiansToDegrees(pfd radians);
	void glhPerspectivef2(Matrix* matrix);
	void glhFrustumf2(Matrix* matrix, pfd left, pfd right, pfd bottom, pfd top, pfd znear, pfd zfar);
	void glhLookAtf1( Matrix *matrix, const Vector3<pfd>& eyePosition3D, const Vector3<pfd>& center3D, const Vector3<pfd>& upVector3D );
	void glhLookAtf2(Matrix *matrix, const Vector3<pfd>& eyePosition3D, const Vector3<pfd>& center3D, const Vector3<pfd>& upVector3D );
	FrustumIntersectionEnum isPointInFrustum(const Vector3<pfd>& p);
	FrustumIntersectionEnum isSphereInFrustum(const bsphere<pfd>& s);
	FrustumIntersectionEnum isAABBInFrustum(const aabbox3<pfd>& b);
	void setFovy(pfd fovy);
	void setNearPlane(pfd np);
	void setFarPlane(pfd fp);
	const bsphere<pfd>& getSphere()
	{
		return _sphere;
	}
protected:
	pfd _fovy; //in degrees
	pfd _aspectRatio;
	pfd _nearPlane;
	pfd _farPlane;
	pfd _nearHeight;
	pfd _nearWidth;
	pfd _farHeight;
	pfd _farWidth;
	Vector3<pfd> _nearCenter;
	Vector3<pfd> _farCenter;

	Vector3<pfd> _corners[8];
	Plane<pfd> _plane[6];
	bsphere<pfd> _sphere;
};

#endif