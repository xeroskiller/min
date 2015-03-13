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
#ifndef __BOUNDINGVOLUME__
#define __BOUNDINGVOLUME__
#include "stdafx.h"

template <class T> class aabbox3;
template <class T> class oobbox3;
template <class T> class Vector3;
template <class T> class bsphere;
template <class T> class Ray3;

template <class T> class BoundingVolume
{
public:
	enum BoundingVolumeEnum: size_t {SPHERE=0, AABB, OOBB};
	BoundingVolume() {};
	~BoundingVolume() {};
	virtual const Vector3<T> getCenter() const = 0;
	virtual const Vector3<T> closestPoint(const Vector3<T>& p) const = 0;
	virtual const T sqDistanceBetween(const Vector3<T>& p) const = 0;
	virtual bool isPointInside(const Vector3<T>& p) const = 0;
	virtual bool intersectsWithSegment(const Vector3<T>& A, const Vector3<T>& B)=0;
	virtual bool intersectsWithRay(const Ray3<T>& ray) = 0;
	virtual bool intersectsWithRay(const Ray3<T>& ray, Vector3<T>& p, T& t) = 0;
	virtual bool intersectsWithSphere(const bsphere<T>& A) const = 0;
	virtual bool intersectsWithSphere(const bsphere<T>& A, Vector3<T>& p) const = 0;
	virtual bool intersectsWithAABB(const aabbox3<T>& A) const = 0;
	virtual bool intersectsWithAABB(const aabbox3<T>& A, Vector3<T>& p) const = 0;
	virtual bool intersectsWithOOBB(const oobbox3<T>& b) const = 0;
	virtual bool intersectsWithOOBB(const oobbox3<T>& b, Vector3<T>& p) const = 0;
	bool intersects(const BoundingVolume& bv) const
	{
		switch (bv._type)
		{
			case SPHERE:
			{
				const bsphere<T> & s = static_cast<const bsphere<T>&>(bv);
				return intersectsWithSphere(s);
				break;
			}
			case AABB:
			{
				const aabbox3<T> & a = static_cast<const aabbox3<T>&>(bv);
				return intersectsWithAABB(a);
				break;
			}
			case OOBB:
			{
				const oobbox3<T> & o = static_cast<const oobbox3<T>&>(bv);
				return intersectsWithOOBB(o);
				break;
			}
			default:
				return false;
		}
	}
	bool intersects(const BoundingVolume& bv, Vector3<T>& p) const
	{
		switch (bv._type)
		{
			case SPHERE:
			{
				const bsphere<T> & s = static_cast<const bsphere<T>&>(bv);
				return intersectsWithSphere(s, p);
				break;
			}
			case AABB:
			{
				const aabbox3<T> & a = static_cast<const aabbox3<T>&>(bv);
				return intersectsWithAABB(a, p);
				break;
			}
			case OOBB:
			{
				const oobbox3<T> & o = static_cast<const oobbox3<T>&>(bv);
				return intersectsWithOOBB(o, p);
				break;
			}
			default:
				return false;
		}
	}
	const Vector3<T> distanceBetweenCenters(const BoundingVolume& bv) const
	{
		return bv.getCenter() - this->getCenter();
	}
	BoundingVolumeEnum getType() const
	{
		return _type;
	}
protected:
	BoundingVolumeEnum _type;
};

#endif