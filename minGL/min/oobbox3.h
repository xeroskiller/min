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
#ifndef __OOBBOX3__
#define __OOBBOX3__
#include "stdafx.h"
#include "BoundingVolume.h"
#include "Vector3.h"
#include "Vector4.h"
#include "bsphere.h"
#include "aabbox3.h"
#include "oobbox3.h"
#include "Ray3.h"
#include "Matrix.h"
#include "Quaternion.h"
#include <math.h>
#include <cassert>

template <class T> class oobbox3 : public BoundingVolume<T>
{
public:
	friend class bsphere<T>;
	friend class aabbox3<T>;
	oobbox3(const aabbox3<T>& box)
	{
		_center = box.getCenter();
		_axes[0] = Vector3<T>(1,0,0);
		_axes[1] = Vector3<T>(0,1,0);
		_axes[2] = Vector3<T>(0,0,1);
		_halfExtents = box.getExtent();
		_halfExtents/=2;
		_rotation = Quaternion<pfd>(0,0,0,0);
		_type = OOBB;
	}
	~oobbox3() {};
	Vector3<T>& getVertice(size_t n)
	{
		switch (n)
		{
			case 0:
			{
				return _center + _axes[0]*_halfExtents + _axes[1]*_halfExtents + _axes[2]*_halfExtents;
			}
			case 1:
			{
				return _center + _axes[0]*_halfExtents + _axes[1]*_halfExtents - _axes[2]*_halfExtents;
			}
			case 2:
			{
				return _center + _axes[0]*_halfExtents - _axes[1]*_halfExtents + _axes[2]*_halfExtents;
			}
			case 3:
			{
				return _center + _axes[0]*_halfExtents - _axes[1]*_halfExtents - _axes[2]*_halfExtents;
			}
			case 4:
			{
				return _center - _axes[0]*_halfExtents + _axes[1]*_halfExtents + _axes[2]*_halfExtents;
			}
			case 5:
			{
				return _center - _axes[0]*_halfExtents + _axes[1]*_halfExtents - _axes[2]*_halfExtents;
			}
			case 6:
			{
				return _center - _axes[0]*_halfExtents - _axes[1]*_halfExtents + _axes[2]*_halfExtents;
			}
			case 7:
			{
				return _center - _axes[0]*_halfExtents - _axes[1]*_halfExtents - _axes[2]*_halfExtents;
			}
		}
		return Vector3<T>(0,0,0);
	}
	void setScale(const Vector3<T>& scale)
	{
		_halfExtents.X() = _halfExtents.X()*scale.X();
		_halfExtents.Y() = _halfExtents.Y()*scale.Y();
		_halfExtents.Z() = _halfExtents.Z()*scale.Z();
	}
	void setPosition(Vector3<T> pos)
	{
		center = pos;
	}
	void rotate(Vector3<T> vec, pfd rot)
	{
		Quaternion<pfd> rotq(vec, rot);
		Quaternion<pfd> conjq = rotq.conjugate();
		_rotation = rotq*_rotation;
		//_rotation = _rotation * conjq;
		for(size_t i=0; i<3; i++)
		{
			Quaternion<pfd> ver(_axes[i]);
			ver = rotq*ver;
			ver = ver*conjq;
			_axes[i] = Vector3<T>(ver.X(), ver.Y(), ver.Z());
		}
	}
	void setRotation(Vector3<T> vec, pfd rot)
	{
		Quaternion<pfd> rotq(vec, rot);
		Quaternion<pfd> conjq = rotq.conjugate();
		_rotation = rotq;

		for(size_t i=0; i<3; i++)
		{
			Quaternion<pfd> ver(_axes[i]);
			ver = rotq*ver;
			ver = ver*conjq;
			_axes[i] = Vector3<T>(ver.X(), ver.Y(), ver.Z());
		}
	}
	oobbox3(const oobbox3<T>& A)
	{
		if(this != &A)
		{
			memcpy(_axes, A._axes, 3*sizeof(Vector3<T>));
			_center = A._center;
			_rotation = A._rotation;
			_type =OOBB;
		}
	}
	oobbox3<T>& operator=(const oobbox3<T>& A)
	{
		if(this != &A)
		{
			memcpy(_axes, A._axes, 3*sizeof(Vector3<T>));
			_center = A._center;
			_rotation = A._rotation;
			_type = OOBB;
		}
		return *this;
	}
	
	//Interface
	const Vector3<T> getCenter() const
	{
		return _center;
	}
	const Vector3<T> closestPoint(const Vector3<T>& p) const
	{
		Vector3<T> d = p - _center;
		Vector3<T> q = _center;
		Vector3<T> e = _halfExtents*2;
		for(size_t i=0; i < 3; i++)
		{
			T dist = d.dotProduct(_axes[i]);
			if(dist > e._arr[i])
			{
				dist = e._arr[i];
			}
			if(dist < -e._arr[i])
			{
				dist = -e._arr[i];
			}
			q += _axes[i]*dist;
		}
		return q;
	}
	const T sqDistanceBetween(const Vector3<T>& p) const
	{
		Vector3<T> v = p - _center;
		Vector3<T> e = _halfExtents*2;
		T sqDist = (T)0.0;
		for(size_t i=0; i < 3; i++)
		{
			T d = v.dotProduct(_axes[i]), excess = (T)0.0;
			if(d < -e._arr[i])
			{
				excess = d + e._arr[i];
			}
			else if(d > e._arr[i])
			{
				excess = d - e._arr[i];
			}
			sqDist += excess * excess;
		}
		return sqDist;
	}
	bool isPointInside(const Vector3<T>& p) const
	{
		Vector3<pfd> d = p - _center;
		Quaternion<T> inv_rot = _rotation.conjugate();
		Quaternion<T> transformed_p(d);
		transformed_p = inv_rot* transformed_p;
		transformed_p = transformed_p*_rotation;

		Vector3<T> MinEdge = _center - _halfExtents;
		Vector3<T> MaxEdge = _center + _halfExtents;
		return (transformed_p.X() >= MinEdge.X() && transformed_p.X() <= MaxEdge.X() && transformed_p.Y() >= MinEdge.Y() && transformed_p.Y() <= MaxEdge.Y() && transformed_p.Z() >= MinEdge.Z() && transformed_p.Z() <= MaxEdge.Z());
	}
	bool intersectsWithSegment(const Vector3<T>& A, const Vector3<T>& B)
	{
		//Transform points into the OOBB coordinate system and apply the AABB test
		Vector3<T> P = Vector3<T>((A - _center).dotProduct(_axes[0]), (A - _center).dotProduct(_axes[1]), (A - _center).dotProduct(_axes[2]));
		Vector3<T> Q = Vector3<T>((B - _center).dotProduct(_axes[0]), (B - _center).dotProduct(_axes[1]), (B - _center).dotProduct(_axes[2]));
		
		Vector3<T> _minEdge = _center - _halfExtents*2;
		Vector3<T> _maxEdge = _center + _halfExtents*2;
		
		Vector3<T> e = _maxEdge - _minEdge;
		Vector3<T> p1 = P + Q;
		Vector3<T> d = p1 - P;
		Vector3<T> m = P + p1 - _minEdge - _maxEdge;

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
		int parallel = 0;
		bool found = false;
		T DA[3];
		T dA[3];
		T tmax;
		T t;
		Vector3<pfd> e = _halfExtents*2;
		Vector3<T> d = _center - ray._origin;
		for (size_t i=0; i<3; i++)
		{
			DA[i] = ray._direction.dotProduct(_axes[i]);
			dA[i] = d.dotProduct(_axes[i]);

			if (fabs(DA[i]) < 0.001)
				parallel |= 1 << i;
			else
			{
				T es = (DA[i] > (T)0.0) ? e._arr[i] : -e._arr[i];
				T invDA = (T)1.0 / DA[i];
				if (!found)
				{
					t = (dA[i] - es) * invDA;
					tmax = (dA[i] + es) * invDA;
					found = true;
				}
				else
				{
					T s = (dA[i] - es) * invDA;
					if (s > t)
						t = s;
					s = (dA[i] + es) * invDA;
					if (s < tmax)
						tmax = s;
					if (t > tmax)
						return false;
				}
			}
		}
		if (parallel)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (parallel & (1 << i))
				{
					//back calculate extent, if calculated extent > extent than no intersection
					if (fabs(dA[i] - t * DA[i]) > e._arr[i] || fabs(dA[i] - tmax * DA[i]) > e._arr[i])
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	bool intersectsWithRay(const Ray3<T>& ray, Vector3<T>& p, T& t)
	{
		int parallel = 0;
		bool found = false;
		T DA[3];
		T dA[3];
		T tmax;
		Vector3<pfd> e = _halfExtents*2;
		Vector3<T> d = _center - ray._origin;
		for (size_t i=0; i<3; i++)
		{
			DA[i] = ray._direction.dotProduct(_axes[i]);
			dA[i] = d.dotProduct(_axes[i]);

			if (fabs(DA[i]) < 0.001)
				parallel |= 1 << i;
			else
			{
				T es = (DA[i] > (T)0.0) ? e._arr[i] : -e._arr[i];
				T invDA = (T)1.0 / DA[i];
				if (!found)
				{
					t = (dA[i] - es) * invDA;
					tmax = (dA[i] + es) * invDA;
					found = true;
				}
				else
				{
					T s = (dA[i] - es) * invDA;
					if (s > t)
						t = s;
					s = (dA[i] + es) * invDA;
					if (s < tmax)
						tmax = s;
					if (t > tmax)
						return false;
				}
			}
		}
		if (parallel)
		{
			for (int i = 0; i < 3; ++i)
			{
				if (parallel & (1 << i))
				{
					//back calculate extent, if calculated extent > extent than no intersection
					if (fabs(dA[i] - t * DA[i]) > e._arr[i] || fabs(dA[i] - tmax * DA[i]) > e._arr[i])
					{
						return false;
					}
				}
			}
		}
		p = ray._origin + ray._direction * t;
		return true;
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
		oobbox3<T> box(A);
		return box.intersectsWithOOBB(*this);
	}
	bool intersectsWithAABB(const aabbox3<T>& A, Vector3<T>& p) const
	{
		p = A.closestPoint(_center);
		oobbox3<T> box(A);
		return box.intersectsWithOOBB(*this);
	}
	bool intersectsWithOOBB(const oobbox3<T>& b) const
	{
		//This method uses the seperating axis theorem to test if two objects are seperating
		oobbox3<T> a = *this;
		T epsilon = (T)0.001;
		T ahalfExtents[3];
		T bhalfExtents[3];
		T ra,rb;

		ahalfExtents[0] = a._halfExtents.X();
		ahalfExtents[1] = a._halfExtents.Y();
		ahalfExtents[2] = a._halfExtents.Z();
		bhalfExtents[0] = b._halfExtents.X();
		bhalfExtents[1] = b._halfExtents.Y();
		bhalfExtents[2] = b._halfExtents.Z();

		Matrix R, AbsR;

		//Rotation Matrix to bring B into A's coordinate system
		R._mat[0]  = a._axes[0].dotProduct(b._axes[0]);
		R._mat[1]  = a._axes[0].dotProduct(b._axes[1]);
		R._mat[2]  = a._axes[0].dotProduct(b._axes[2]);
		R._mat[4]  = a._axes[1].dotProduct(b._axes[0]);
		R._mat[5]  = a._axes[1].dotProduct(b._axes[1]);
		R._mat[6]  = a._axes[1].dotProduct(b._axes[2]);
		R._mat[8]  = a._axes[2].dotProduct(b._axes[0]);
		R._mat[9]  = a._axes[2].dotProduct(b._axes[1]);
		R._mat[10] = a._axes[2].dotProduct(b._axes[2]);

		//Translation Vector to bring B into A's coordinate system
		Vector3<T> trans = b._center - a._center;
		trans = Vector3<T>(trans.dotProduct(a._axes[0]),trans.dotProduct(a._axes[1]),trans.dotProduct(a._axes[2]));
		T t[3] = {trans.X(),trans.Y(),trans.Z()};

		//Add a small bit of error to avoid 0 > 0 fallacy
		AbsR._mat[0]  = abs(R._mat[0]) + epsilon;
		AbsR._mat[1]  = abs(R._mat[1]) + epsilon;
		AbsR._mat[2]  = abs(R._mat[2]) + epsilon;
		AbsR._mat[4]  = abs(R._mat[4]) + epsilon;
		AbsR._mat[5]  = abs(R._mat[5]) + epsilon;
		AbsR._mat[6]  = abs(R._mat[6]) + epsilon;
		AbsR._mat[8]  = abs(R._mat[8]) + epsilon;
		AbsR._mat[9]  = abs(R._mat[9]) + epsilon;
		AbsR._mat[10] = abs(R._mat[10]) + epsilon;

		//Test Axis L=A0, L=A1, L=A2
		for(int i=0;i<3;i++)
		{
			ra = ahalfExtents[i];
			rb = bhalfExtents[0] * AbsR._mat[4*i] + bhalfExtents[1] * AbsR._mat[4*i+1] + bhalfExtents[2] * AbsR._mat[4*i+2];
			if (abs(t[i]) > ra + rb) {return 0;}
		}

		//Test Axis L=B0, L=B1, L=B2
	   for(int i=0;i<3;i++)
		{
			ra = ahalfExtents[0] * AbsR._mat[i] + ahalfExtents[1] * AbsR._mat[i+4] + ahalfExtents[2] * AbsR._mat[i+8];
			rb = bhalfExtents[i];
			if (abs(t[0] * R._mat[i] + t[1] * R._mat[i+4] + t[2]*R._mat[i+8]) > ra + rb) {return 0;}
		}

		//Test axis L = A0 x B0
		ra = ahalfExtents[1] * AbsR._mat[8] + ahalfExtents[2] * AbsR._mat[4];
		rb = bhalfExtents[1] * AbsR._mat[2] + bhalfExtents[2] * AbsR._mat[1];
		if (abs(t[2] * R._mat[4] - t[1] * R._mat[8]) > ra + rb) {return 0;}

		//Test Axis L = A0 x B1
		ra = ahalfExtents[1] * AbsR._mat[9] + ahalfExtents[2] * AbsR._mat[5];
		rb = bhalfExtents[0] * AbsR._mat[2] + bhalfExtents[2] * AbsR._mat[0];
		if (abs(t[2] * R._mat[5] - t[1] * R._mat[9]) > ra + rb) {return 0;}

		 //Test Axis L = A0 x B2
		ra = ahalfExtents[1] * AbsR._mat[10] + ahalfExtents[2] * AbsR._mat[6];
		rb = bhalfExtents[0] * AbsR._mat[1] + bhalfExtents[1] * AbsR._mat[0];
		if (abs(t[2] * R._mat[6] - t[1] * R._mat[10]) > ra + rb) {return 0;}

		 //Test Axis L = A1 x B0
		ra = ahalfExtents[0] * AbsR._mat[8] + ahalfExtents[2] * AbsR._mat[0];
		rb = bhalfExtents[1] * AbsR._mat[6] + bhalfExtents[2] * AbsR._mat[5];
		if (abs(t[0] * R._mat[8] - t[2] * R._mat[0]) > ra + rb) {return 0;}

		 //Test Axis L = A1 x B1
		ra = ahalfExtents[0] * AbsR._mat[9] + ahalfExtents[2] * AbsR._mat[1];
		rb = bhalfExtents[0] * AbsR._mat[6] + bhalfExtents[2] * AbsR._mat[4];
		if (abs(t[0] * R._mat[9] - t[2] * R._mat[1]) > ra + rb) {return 0;}

		//Test Axis L = A1 x B2
		ra = ahalfExtents[0] * AbsR._mat[10] + ahalfExtents[2] * AbsR._mat[2];
		rb = bhalfExtents[0] * AbsR._mat[5] + bhalfExtents[1] * AbsR._mat[4];
		if (abs(t[0] * R._mat[10] - t[2] * R._mat[2]) > ra + rb) {return 0;}

		//Test Axis L = A2 x B0
		ra = ahalfExtents[0] * AbsR._mat[4] + ahalfExtents[1] * AbsR._mat[0];
		rb = bhalfExtents[1] * AbsR._mat[10] + bhalfExtents[2] * AbsR._mat[9];
		if (abs(t[1] * R._mat[0] - t[0] * R._mat[4]) > ra + rb) {return 0;}

		//Test Axis L = A2 x B1
		ra = ahalfExtents[0] * AbsR._mat[5] + ahalfExtents[1] * AbsR._mat[1];
		rb = bhalfExtents[0] * AbsR._mat[10] + bhalfExtents[2] * AbsR._mat[8];
		if (abs(t[1] * R._mat[1] - t[0] * R._mat[5]) > ra + rb) {return 0;}

		//Test Axis L = A2 x B2
		ra = ahalfExtents[0] * AbsR._mat[6] + ahalfExtents[1] * AbsR._mat[2];
		rb = bhalfExtents[0] * AbsR._mat[9] + bhalfExtents[1] * AbsR._mat[8];
		if (abs(t[1] * R._mat[2] - t[0] * R._mat[6]) > ra + rb) {return 0;}

		//Since no seperating axis is found
		return 1;
	}
	bool intersectsWithOOBB(const oobbox3<T>& b, Vector3<T>& p) const
	{
		p = closestPoint(b._center);
		return intersectsWithOOBB(b);
	}
	void setPosition(const Vector3<pfd>& pos)
	{
		_center = pos;
	}
protected:
	Vector3<T> _axes[3]; //object space verts
	Vector3<T> _halfExtents;
	Vector3<T> _center;
	Quaternion<T> _rotation;
};

#endif