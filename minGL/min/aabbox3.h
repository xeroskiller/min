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
#ifndef __AABBOX3D__
#define __AABBOX3D__
#include "stdafx.h"

#include "BoundingVolume.h"
#include "Vector3.h"
#include "Vector4.h"
#include "bsphere.h"
#include "aabbox3.h"
#include "oobbox3.h"
#include "Ray3.h"
#include <limits>

template <class T> class aabbox3 : public BoundingVolume<T>
{
public:
	friend class bsphere<T>;
	friend class oobbox3<T>;
	friend class Frustum;
	aabbox3(Vector3<T>* vertices, size_t size)
	{
		if (vertices != 0)
		{
			if (size >= 2)
			{
				add(vertices, size);
				_type = AABB;
			}
			else if (size == 1)
			{
				aabbox3<T> temp(Vector3<T>(0, 0, 0), Vector3<T>(1, 1, 1));
				temp.setPosition(vertices[0]);
				_minEdge = temp._minEdge;
				_maxEdge = temp._maxEdge;
				_type = AABB;
			}
			else
			{
				_minEdge = Vector3<T>(0, 0, 0);
				_maxEdge = Vector3<T>(0, 0, 0);
				_type = AABB;
			}
		}
		else
		{
			_minEdge = Vector3<T>(0, 0, 0);
			_maxEdge = Vector3<T>(0, 0, 0);
			_type = AABB;
		}
	}
	aabbox3(Vector4<T>* vertices, size_t size)
	{
		if (vertices != 0)
		{
			if (size >= 2 && vertices != 0)
			{
				add(vertices, size);
				_type = AABB;
			}
			else if (size == 1 && vertices != 0)
			{
				aabbox3<T> temp(Vector3<T>(0, 0, 0), Vector3<T>(1, 1, 1));
				temp.setPosition(vertices[0]);
				_minEdge = temp._minEdge;
				_maxEdge = temp._maxEdge;
				_type = AABB;
			}
			else
			{
				_minEdge = Vector3<T>(0, 0, 0);
				_maxEdge = Vector3<T>(0, 0, 0);
				_type = AABB;
			}
		}
		else
		{
			_minEdge = Vector3<T>(0, 0, 0);
			_maxEdge = Vector3<T>(0, 0, 0);
			_type = AABB;
		}
	}
	void add(Vector3<T>* vertices, size_t size)
	{
		for(size_t i=0; i<size; i++)
		{

			if(vertices[i].X() > _maxEdge._arr[0])
			{
				_maxEdge._arr[0] = vertices[i].X();
			}
			if(vertices[i].X() < _minEdge._arr[0])
			{
				_minEdge._arr[0] = vertices[i].X();
			}
			if(vertices[i].Y() > _maxEdge._arr[1])
			{
				_maxEdge._arr[1] = vertices[i].Y();
			}
			if(vertices[i].Y() < _minEdge._arr[1])
			{
				_minEdge._arr[1] = vertices[i].Y();
			}
			if(vertices[i].Z() > _maxEdge._arr[2])
			{
				_maxEdge._arr[2] = vertices[i].Z();
			}
			if(vertices[i].Z() < _minEdge._arr[2])
			{
				_minEdge._arr[2] = vertices[i].Z();
			}
		}
	}
	void add(Vector4<T>* vertices, size_t size)
	{
		for(size_t i=0; i<size; i++)
		{
			if(vertices[i].X() > _maxEdge._arr[0])
			{
				_maxEdge._arr[0] = vertices[i].X();
			}
			if(vertices[i].X() < _minEdge._arr[0])
			{
				_minEdge._arr[0] = vertices[i].X();
			}
			if(vertices[i].Y() > _maxEdge._arr[1])
			{
				_maxEdge._arr[1] = vertices[i].Y();
			}
			if(vertices[i].Y() < _minEdge._arr[1])
			{
				_minEdge._arr[1] = vertices[i].Y();
			}
			if(vertices[i].Z() > _maxEdge._arr[2])
			{
				_maxEdge._arr[2] = vertices[i].Z();
			}
			if(vertices[i].Z() < _minEdge._arr[2])
			{
				_minEdge._arr[2] = vertices[i].Z();
			}
		}
	}
	aabbox3(const Vector3<T>& min, const Vector3<T>& max)
	{
		_minEdge = min;
		_maxEdge = max;
		_type = AABB;
	}
	~aabbox3() {}
	const Vector3<T> getVertice(size_t n) const
	{
		Vector3<T> _halfExtents = getExtent() * (T)0.5;
		switch (n)
		{
			case 0:
			{
				return _center + _halfExtents;
			}
			case 1:
			{
				return _center + Vector3<T>(_halfExtents.X(), _halfExtents.Y(), -_halfExtents.Z());
			}
			case 2:
			{
				return _center + Vector3<T>(_halfExtents.X(), -_halfExtents.Y(), _halfExtents.Z());
			}
			case 3:
			{
				return _center + Vector3<T>(_halfExtents.X(), -_halfExtents.Y(), -_halfExtents.Z());
			}
			case 4:
			{
				return _center + Vector3<T>(-_halfExtents.X(), _halfExtents.Y(), _halfExtents.Z());
			}
			case 5:
			{
				return _center + Vector3<T>(-_halfExtents.X(), _halfExtents.Y(), -_halfExtents.Z());
			}
			case 6:
			{
				return _center + Vector3<T>(-_halfExtents.X(), -_halfExtents.Y(), _halfExtents.Z());
			}
			case 7:
			{
				return _center - _halfExtents;
			}
		}
		return Vector3<T>(0,0,0);
	}
	const Vector3<T> getExtent() const
	{
		 return _maxEdge - _minEdge;
	}
	aabbox3(const aabbox3& A)
	{
		if(this != &A)
		{
			_minEdge = A._minEdge;
			_maxEdge = A._maxEdge;
			_type = AABB;
		}
	}
	aabbox3& operator=(const aabbox3<T>& A)
	{
		if(this != &A)
		{
			_minEdge = A._minEdge;
			_maxEdge = A._maxEdge;
			_type = AABB;
		}
		return *this;
	}
	static const aabbox3<T> MostSeparatedPoints(Vector3<T>* vertices, size_t size)
	{
		if (size >= 2 && vertices != 0)
		{
			size_t minx = 0;
			size_t maxx = 0;
			size_t miny = 0;
			size_t maxy = 0;
			size_t minz = 0;
			size_t maxz = 0;
			for (size_t i = 0; i < size; i++)
			{
				if (vertices[i].X() > vertices[maxx].X())
				{
					maxx = i;
				}
				if (vertices[i].X() < vertices[minx].X())
				{
					minx = i;
				}
				if (vertices[i].Y() > vertices[maxy].Y())
				{
					maxy = i;
				}
				if (vertices[i].Y() < vertices[miny].Y())
				{
					miny = i;
				}
				if (vertices[i].Z() > vertices[maxz].Z())
				{
					maxz = i;
				}
				if (vertices[i].Z() < vertices[minz].Z())
				{
					minz = i;
				}
			}

			T dist2x = (vertices[maxx] - vertices[minx]).dotProduct(vertices[maxx] - vertices[minx]);
			T dist2y = (vertices[maxy] - vertices[miny]).dotProduct(vertices[maxy] - vertices[miny]);
			T dist2z = (vertices[maxz] - vertices[minz]).dotProduct(vertices[maxz] - vertices[minz]);

			size_t min = minx;
			size_t max = maxx;

			if (dist2y > dist2x && dist2y > dist2z)
			{
				max = maxy;
				min = miny;
			}
			if (dist2z > dist2x && dist2z > dist2y)
			{
				max = maxz;
				min = minz;
			}
			return aabbox3<T>(vertices[min], vertices[max]);
		}
		else
		{
			return aabbox3<T>(vertices, size);
		}
	}
	static const aabbox3<T> MostSeparatedPoints(Vector4<T>* vertices, size_t size)
	{
		if (size >= 2 && vertices != 0)
		{

			size_t minx = 0;
			size_t maxx = 0;
			size_t miny = 0;
			size_t maxy = 0;
			size_t minz = 0;
			size_t maxz = 0;
			for (size_t i = 0; i < size; i++)
			{
				if (vertices[i].X() > vertices[maxx].X())
				{
					maxx = i;
				}
				if (vertices[i].X() < vertices[minx].X())
				{
					minx = i;
				}
				if (vertices[i].Y() > vertices[maxy].Y())
				{
					maxy = i;
				}
				if (vertices[i].Y() < vertices[miny].Y())
				{
					miny = i;
				}
				if (vertices[i].Z() > vertices[maxz].Z())
				{
					maxz = i;
				}
				if (vertices[i].Z() < vertices[minz].Z())
				{
					minz = i;
				}
			}

			T dist2x = (vertices[maxx] - vertices[minx]).dotProduct(vertices[maxx] - vertices[minx]);
			T dist2y = (vertices[maxy] - vertices[miny]).dotProduct(vertices[maxy] - vertices[miny]);
			T dist2z = (vertices[maxz] - vertices[minz]).dotProduct(vertices[maxz] - vertices[minz]);

			size_t min = minx;
			size_t max = maxx;

			if (dist2y > dist2x && dist2y > dist2z)
			{
				max = maxy;
				min = miny;
			}
			if (dist2z > dist2x && dist2z > dist2y)
			{
				max = maxz;
				min = minz;
			}
			return aabbox3<T>(vertices[min], vertices[max]);
		}
		else
		{
			return aabbox3<T>(vertices, size);
		}
	}
	const Vector3<T>& getMin() const
	{
		return _minEdge;
	}
	const Vector3<T>& getMax() const
	{
		return _maxEdge;
	}
	//INTERFACE
	const Vector3<T> getCenter() const
	{
		return (_minEdge + _maxEdge) * (T)0.5;
	}
	const Vector3<T> closestPoint(const Vector3<T>& p) const
	{
		Vector3<T> Q;
		for(size_t i=0; i < 3; i++)
		{
			T v = p._arr[i];
			if(v < _minEdge._arr[i])
			{
				v = _minEdge._arr[i];
			}
			if(v > _maxEdge._arr[i])
			{
				v = _maxEdge._arr[i];
			}
			Q._arr[i] = v;
		}
		return Q;
	}
	const T sqDistanceBetween(const Vector3<T>& p) const
	{
		T sqDist = (T)0.0;
		for(size_t i=0; i<3;i++)
		{
			T v = p._arr[i];
			if(v < _minEdge._arr[i])
			{
				sqDist += (_minEdge._arr[i] - v) * (_minEdge._arr[i] - v);
			}
			if(v > _maxEdge._arr[i])
			{
				sqDist += (v - _maxEdge._arr[i]) * (v - _maxEdge._arr[i]);
			}
		}
		return sqDist;
	}
	bool isPointInside(const Vector3<T>& p) const
	{
		return (p.X() >= _minEdge.X() && p.X() <= _maxEdge.X() && p.Y() >= _minEdge.Y() && p.Y() <= _maxEdge.Y() && p.Z() >= _minEdge.Z() && p.Z() <= _maxEdge.Z());
	}
	bool intersectsWithSegment(const Vector3<T>& A, const Vector3<T>& B)
	{
		Vector3<T> e = _maxEdge - _minEdge;
		Vector3<T> p1 = A + B;
		Vector3<T> d = p1 - A;
		Vector3<T> m = A + p1 - _minEdge - _maxEdge;

		T adx = fabs(d._arr[0]);
		if(fabs(m._arr[0]) > e._arr[0] + adx)
		{
			return 0;
		}
		T ady = fabs(d._arr[1]);
		if(fabs(m._arr[1]) > e._arr[1] + ady)
		{
			return 0;
		}
		T adz = fabs(d._arr[2]);
		if(fabs(m._arr[2]) > e._arr[2] + adz)
		{
			return 0;
		}

		adx += (T)0.001; ady += (T)0.001; adz += (T)0.001;
		if(fabs(m._arr[1] * d._arr[2] - m._arr[2] * d._arr[1]) > e._arr[1] * adz + e._arr[2] * ady)
		{
			return 0;
		}
		if(fabs(m._arr[2] * d._arr[0] - m._arr[0] * d._arr[2]) > e._arr[0] * adz + e._arr[2] * adx)
		{
			return 0;
		}
		if(fabs(m._arr[0] * d._arr[1] - m._arr[1] * d._arr[0]) > e._arr[0] * ady + e._arr[1] * adx)
		{
			return 0;
		}
		return 1;
	}
	bool intersectsWithRay(const Ray3<T>& ray)
	{
		T t = (T)0.0;
		#undef max
		T tmax = std::numeric_limits<T>::max();

		for(size_t i=0; i < 3; i++)
		{
			//if parallel and not in the slab exit (no intersection)
			if(fabs(ray._direction._arr[i]) < 0.001)
			{
				if(ray._origin._arr[i] < _minEdge._arr[i] || ray._origin._arr[i] > _maxEdge._arr[i])
				{
					return 0;
				}
			}
			else
			{
				T t1 = (_minEdge._arr[i] - ray._origin._arr[i]) * ray._inv_direction._arr[i];
				T t2 = (_maxEdge._arr[i] - ray._origin._arr[i]) * ray._inv_direction._arr[i];

				if(t1 > t2)
				{
					T w = t1;
					t1 = t2;
					t2 = w;
				}

				if(t1> t)
				{
					t = t1;
				}
				if(t2 > tmax)
				{
					tmax = t2;
				}
				if(t > tmax)
				{
					return 0;
				}
			}
		}
		return 1;
	}
	bool intersectsWithRay(const Ray3<T>& ray, Vector3<T>& p, T& t)
	{
		t = (T)0.0;
		#undef max
		T tmax = std::numeric_limits<T>::max();

		for(size_t i=0; i < 3; i++)
		{
			//if parallel and not in the slab exit (no intersection)
			if(fabs(ray._direction._arr[i]) < 0.001)
			{
				if(ray._origin._arr[i] < _minEdge._arr[i] || ray._origin._arr[i] > _maxEdge._arr[i])
				{
					return 0;
				}
			}
			else
			{
				T t1 = (_minEdge._arr[i] - ray._origin._arr[i]) * ray._inv_direction._arr[i];
				T t2 = (_maxEdge._arr[i] - ray._origin._arr[i]) * ray._inv_direction._arr[i];

				if(t1 > t2)
				{
					T w = t1;
					t1 = t2;
					t2 = w;
				}

				if(t1> t)
				{
					t = t1;
				}
				if(t2 > tmax)
				{
					tmax = t2;
				}
				if(t > tmax)
				{
					return 0;
				}
			}
		}
		p = ray._origin + ray._direction * t;
		return 1;
	}
	bool intersectsWithSphere(const bsphere<T>& A) const
	{
		T sqDist = sqDistanceBetween(A._center);
		return (sqDist <= A._radius * A._radius);
	}
	bool intersectsWithSphere(const bsphere<T>& A, Vector3<T>& p) const
	{
		p = closestPoint(A._center);
		Vector3<T> v = p - A._center;
		return (v.dotProduct(v) <= A._radius* A._radius);
	}
	bool intersectsWithAABB(const aabbox3<T>& A) const 
	{
		return (_minEdge.X() <= A._maxEdge.X() && _minEdge.Y() <= A._maxEdge.Y() && _minEdge.Z() <= A._maxEdge.Z() && _maxEdge.X() >= A._minEdge.X() && _maxEdge.Y() >= A._minEdge.Y() && _maxEdge.Z() >= A._minEdge.Z());
	}
	bool intersectsWithAABB(const aabbox3<T>& A, Vector3<T>& p) const
	{
		p = closestPoint(A.getCenter());
		return (_minEdge.X() <= A._maxEdge.X() && _minEdge.Y() <= A._maxEdge.Y() && _minEdge.Z() <= A._maxEdge.Z() && _maxEdge.X() >= A._minEdge.X() && _maxEdge.Y() >= A._minEdge.Y() && _maxEdge.Z() >= A._minEdge.Z());
	}
	bool intersectsWithOOBB(const oobbox3<T>& b) const
	{
		oobbox3<T> box(*this);
		return box.intersectsWithOOBB(b);
	}
	bool intersectsWithOOBB(const oobbox3<T>& A, Vector3<T>& p) const
	{
		p = A.closestPoint(getCenter());
		oobbox3<T> box(*this);
		return box.intersectsWithOOBB(A);
	}
	void setPosition(const Vector3<pfd>& pos)
	{
		Vector3<T> center = getCenter();
		Vector3<T> relMinEdge = _minEdge - center;
		Vector3<T> relMaxEdge = _maxEdge - center;
		_minEdge = relMinEdge + pos;
		_maxEdge = relMaxEdge + pos;
	}
protected:
	Vector3<T> _minEdge;
	Vector3<T> _maxEdge;
};

#endif