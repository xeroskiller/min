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
#ifndef __QUATERNION__
#define __QUATERNION__
#include "stdafx.h"
#include <math.h>

template <class T> class Vector3;
#include "Vector3.h"

template <class T> class Quaternion
{
public:
	Quaternion(void)
	{
		_arr[0] = 1;
		_arr[1] = 0;
		_arr[2] = 0;
		_arr[3] = 0;
	}
	Quaternion(const T x, const T y, const T z)
	{
			_arr[0] = 1;
			_arr[1] = x;
			_arr[2] = y;
			_arr[3] = z;
	}
	Quaternion(const T w, const T x, const T y, const T z)
	{
			T sina_2 = sin(w/2);
			_arr[0] = cos(w/2);
			_arr[1] = x*sina_2;
			_arr[2] = y*sina_2;
			_arr[3] = z*sina_2;
	}
	Quaternion(const Vector3<T>& vector)
	{
		_arr[0] = 1;
		_arr[1] = vector.X();
		_arr[2] = vector.Y();
		_arr[3] = vector.Z();
	}
	Quaternion(const Vector3<T>& vector, T angle)
	{
		T sina_2 = sin(angle/2);
		_arr[0] = cos(angle/2);
		_arr[1] = vector.X()*sina_2;
		_arr[2] = vector.Y()*sina_2;
		_arr[3] = vector.Z()*sina_2;
	}
	//must be normalized vectors
	Quaternion(const Vector3<T>& v1, const Vector3<T>& v2)
	{
		Vector3<pfd> x_axis(1,0,0);
		Vector3<pfd> y_axis(1,0,0);

		T cos_theta = v1.dotProduct(v2);
		if(cos_theta < -0.999999)
		{
			Vector3<pfd> q = y_axis.crossProduct(v1);
			if(q.magnitude() < 0.000001)
			{
				q = x_axis.crossProduct(v1);
			}
			q.normalize();
			_arr[0] = 0;
			_arr[1] = q.X();
			_arr[2] = q.Y();
			_arr[3] = q.Z();
		}
		else if(cos_theta > 0.999999)
		{
			_arr[0] = 1;
			_arr[1] = 0;
			_arr[2] = 0;
			_arr[3] = 0;
		}
		else
		{
			Vector3<pfd> q = v1.crossProduct(v2);
			_arr[0] = cos_theta + 1;
			_arr[1] = q.X();
			_arr[2] = q.Y();
			_arr[3] = q.Z();
			this->normalize();
		}
	}
	Quaternion(const Quaternion<T>& copy)
	{
		_arr[0] = copy._arr[0];
		_arr[1] = copy._arr[1];
		_arr[2] = copy._arr[2];
		_arr[3] = copy._arr[3];
	}
	~Quaternion(void) {}

	void calculateW()
	{
		T t = (T)1.0 - ( _arr[1] * _arr[1] ) - ( _arr[2] * _arr[2] ) - ( _arr[3] * _arr[3] );
		if ( t < (T)0.0 )
		{
			_arr[0] = (T)0.0;
		}
		else
		{
			_arr[0] = -sqrt(t);
		}
	}

	T magnitude() const
	{
		return sqrt(_arr[0]*_arr[0] + _arr[1]*_arr[1] + _arr[2]*_arr[2] + _arr[3]*_arr[3]);
	}
	void normalize()
	{
		T mag = magnitude();
		_arr[0] /= mag;
		_arr[1] /= mag;
		_arr[2] /= mag;
		_arr[3] /= mag;
	}
	const Quaternion<T> conjugate() const
	{
		Quaternion<T> temp= *this;
		temp._arr[1] = -1*temp._arr[1];
		temp._arr[2] = -1*temp._arr[2];
		temp._arr[3] = -1*temp._arr[3];
		return temp;
	}
	static const Quaternion<T> lerp(const Quaternion<T>& A, const Quaternion<T>& B, T interpolation)
	{
		T w = A._arr[0] + (B._arr[0] - A._arr[0]) * interpolation;
		T x = A._arr[1] + (B._arr[1] - A._arr[1]) * interpolation;
		T y = A._arr[2] + (B._arr[2] - A._arr[2]) * interpolation;
		T z = A._arr[3] + (B._arr[3] - A._arr[3]) * interpolation;
		Quaternion<T> temp(w, x, y, z);
		temp.normalize();
		return temp;
	}
	const Vector3<T> transform(const Vector3<pfd> A) const
	{
		Quaternion<pfd> tq =  *this * A;
		tq = tq * this->conjugate();
		Vector3<pfd> tv(tq.X(), tq.Y(), tq.Z());
		return tv;
	}
	Quaternion<T>& operator=(const Quaternion<T>& rhs)
	{
		if(this == &rhs)
			return *this;
		else
		{
			_arr[0] = rhs._arr[0];
			_arr[1] = rhs._arr[1];
			_arr[2] = rhs._arr[2];
			_arr[3] = rhs._arr[3];
		}
		return *this;
	}
	const Quaternion<T> operator*(const Quaternion<T>& b) const
	{
		Quaternion a = *this;
		a._arr[0] = b._arr[0]*this->_arr[0] - b._arr[1]*this->_arr[1] - b._arr[2]*this->_arr[2] - b._arr[3]*this->_arr[3];
		a._arr[1] = b._arr[0]*this->_arr[1] + b._arr[1]*this->_arr[0] - b._arr[2]*this->_arr[3] + b._arr[3]*this->_arr[2];
		a._arr[2] = b._arr[0]*this->_arr[2] + b._arr[1]*this->_arr[3] + b._arr[2]*this->_arr[0] - b._arr[3]*this->_arr[1];
		a._arr[3] = b._arr[0]*this->_arr[3] - b._arr[1]*this->_arr[2] + b._arr[2]*this->_arr[1] + b._arr[3]*this->_arr[0];
		return a;
	}
	const T X() const
	{
		return _arr[1];
	}
	const T Y() const 
	{
		return _arr[2];
	}
	const T Z() const 
	{
		return _arr[3];
	}
	T _arr[4];
private:
};

#endif

