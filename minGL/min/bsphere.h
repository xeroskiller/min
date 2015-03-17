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
#ifndef __BSPHERE__
#define __BSPHERE__
#include "stdafx.h"

#include "BoundingVolume.h"
#include "Vector3.h"
#include "Vector4.h"
#include "bsphere.h"
#include "aabbox3.h"
#include "oobbox3.h"
#include "Ray3.h"

template <class T> class bsphere : public BoundingVolume<T>
{
public:
	friend class aabbox3<T>;
	friend class oobbox3<T>;
	friend class Frustum;
	bsphere()
	{
		_radius = 0.0;
		_type = SPHERE;
	}
	bsphere(const Vector3<T>& center, const T& radius)
	{
		_center = center;
		_radius = radius;
		_type = SPHERE;
	}
	bsphere(Vector3<T>* vertices, size_t size)
	{
		aabbox3<T> box = aabbox3<T>::MostSeparatedPoints(vertices, size);
		_center = box.getCenter();
		_radius = (box._maxEdge - _center).dotProduct(box._maxEdge - _center);
		_radius = sqrt(_radius);

		add(vertices, size);
		
		_type = SPHERE;
	}
	bsphere(Vector4<T>* vertices, size_t size)
	{
		aabbox3<T> box = aabbox3<T>::MostSeparatedPoints(vertices, size);
		_center = box.getCenter();
		_radius = (box._maxEdge - _center).dotProduct(box._maxEdge - _center);
		_radius = sqrt(_radius);

		add(vertices, size);

		_type = SPHERE;
	}
	bsphere(const aabbox3<T>& box)
	{
		_center = box.getCenter();
		_radius = (box._maxEdge - _center).dotProduct(box._maxEdge - _center);
		_radius = sqrt(_radius);
		_type = SPHERE;
	}
	void add(Vector3<T>* vertices, size_t size)
	{
		for(size_t i=0; i<size; i++)
		{
			Vector3<T> d = vertices[i] - _center;
			T dist2 = d.dotProduct(d);

			if(dist2 > _radius * _radius)
			{
				T dist = sqrt(dist2);
				T newRadius = (_radius + dist) * (T)0.5;
				T k = (newRadius - _radius) / dist;
				_radius = newRadius;
				_center += d*k;
			}
		}
	}
	void add(Vector4<T>* vertices, size_t size)
	{
		for(size_t i=0; i<size; i++)
		{
			Vector3<T> d = vertices[i] - _center;
			T dist2 = d.dotProduct(d);

			if(dist2 > _radius * _radius)
			{
				T dist = sqrt(dist2);
				T newRadius = (_radius + dist) * (T)0.5;
				T k = (newRadius - _radius) / dist;
				_radius = newRadius;
				_center += d*k;
			}
		}
	}
	~bsphere() {};
	bsphere(const bsphere& A)
	{
		if(this != &A)
		{
			_radius = A._radius;
			_center = A._center;
			_type = A._type;
		}
	}
	const bsphere& operator=(const bsphere& A)
	{
		if(this != &A)
		{
			_radius = A._radius;
			_center = A._center;
			_type = A._type;
		}
		return *this;
	}
	
	//INTERFACE
	const Vector3<T> closestPoint(const Vector3<T>& p) const
	{
		Vector3<T> direction = (p - _center);
		direction.normalize();
		return (_center + direction*_radius);

	}
	const T sqDistanceBetween(const Vector3<T>& p) const
	{
		Vector3<T> direction = (p - _center);
		direction.normalize();
		Vector3<T> d = (p - _center + direction*_radius);
		return d.dotProduct(d);
	}
	bool isPointInside(const Vector3<T>& p) const
	{
		Vector3<T> d = p - _center; 
		return (d.dotProduct(d) <= _radius*_radius);
	}
	bool intersectsWithSegment(const Vector3<T>& A, const Vector3<T>& B)
	{
		Vector3<T> m = A - _center;
		Vector3<T> d = (B-A); 
		T b_a_mag = d.magnitude();
		d = d/b_a_mag;

		T b = m.dotProduct(d);
		T c = m.dotProduct(m) - _radius*_radius;
		if(c > (T)0.0 && b > (T)0.0)
		{
			return 0;
		}
		T discr = b*b - c;
		if(discr < (T)0.0)
		{
			return 0;
		}
		T t = -b - sqrt(discr);
		if(t < (T)0.0)
		{
			t = (T)0.0;
		}
		if(t <= b_a_mag)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	bool intersectsWithRay(const Ray3<T>& ray)
	{
		Vector3<T> m = ray._origin - _center;
		T c = m.dotProduct(m) - _radius* _radius;
		if(c <= (T)0.0)
		{
			return 1;
		}
		T b = m.dotProduct(ray._direction);
		if(b > (T)0.0)
		{
			return 0;
		}
		T disc = b*b -c;
		if(disc < (T)0.0)
		{
			return 0;
		}
		return 1;
	}
	bool intersectsWithRay(const Ray3<T>& ray, Vector3<T>& p, T& t)
	{
		Vector3<T> m = ray._origin - _center;
		T b = m.dotProduct(ray._direction);
		T c = m.dotProduct(m) - _radius*_radius;
		if(c > (T)0.0 && b > (T)0.0)
		{
			return 0;
		}
		T discr = b*b - c;
		if(discr < (T)0.0)
		{
			return 0;
		}
		t = -b - sqrt(discr);
		if(t < (T)0.0)
		{
			t = (T)0.0;
		}
		p = ray._origin + ray._direction * t;
		return 1;
	}
	bool intersectsWithSphere(const bsphere<T>& A) const
	{
		Vector3<T> d = A._center - _center; 
		T dist2 = d.dotProduct(d);
		T radiusSum = _radius + A._radius;
		return (dist2 <= radiusSum * radiusSum);
	}
	bool intersectsWithSphere(const bsphere<T>& A, Vector3<T>& p) const
	{
		p = closestPoint(A._center);
		Vector3<T> v = p - A._center;
		return (v.dotProduct(v) <= A._radius* A._radius);
	}
	bool intersectsWithAABB(const aabbox3<T>& A) const
	{
		T sqDist = A.sqDistanceBetween(_center);
		return (sqDist <= _radius * _radius);
	}
	bool intersectsWithAABB(const aabbox3<T>& A, Vector3<T>& p) const
	{
		p = A.closestPoint(_center);
		Vector3<T> v = p - _center;
		return (v.dotProduct(v) <= _radius* _radius);
	}
	bool intersectsWithOOBB(const oobbox3<T>& A) const
	{
		T sqDist = A.sqDistanceBetween(_center);
		return (sqDist <= _radius * _radius);
	}
	bool intersectsWithOOBB(const oobbox3<T>& A, Vector3<T>& p) const
	{
		p = A.closestPoint(_center);
		Vector3<T> v = p - _center;
		return (v.dotProduct(v) <= _radius* _radius);
	}
	const Vector3<T> getCenter() const
	{
		return _center;
	}
	void setPosition(const Vector3<pfd>& pos)
	{
		_center = pos;
	}
	const pfd& getRadius() const 
	{ 
		return _radius;
	}
	void setRadius(const pfd& r)
	{
		_radius = r;
	}
protected:
	T _radius;
	Vector3<T> _center;
};

#endif