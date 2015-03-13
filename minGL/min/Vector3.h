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
#ifndef __VECTOR3__
#define __VECTOR3__

#include "stdafx.h"
#include <math.h>
template <class T> class Vector4;
#include "Vector4.h"
#define _USE_MATH_DEFINES
template <class T> class Vector3
{
public:
	Vector3(void)
	{
		_arr[0]=0;
		_arr[1]=0;
		_arr[2]=0;
	}
	Vector3(T x, T y, T z)
	{
		_arr[0]=x;
		_arr[1]=y;
		_arr[2]=z;
	}
	Vector3(const Vector4<T>& vec4)
	{
		_arr[0]=vec4._arr[0];
		_arr[1]=vec4._arr[1];
		_arr[2]=vec4._arr[2];
	}
	Vector3(const Vector3<T>& copy)
	{
		_arr[0]=copy._arr[0];
		_arr[1]=copy._arr[1];
		_arr[2]=copy._arr[2];
	}
	~Vector3(void){}
	T _arr[3];
	T X() const 
	{
		return _arr[0];
	}
	T Y() const 
	{
		return _arr[1];
	}
	T Z() const
	{
		return _arr[2];
	}
	T magnitude() const
	{
		return sqrt(_arr[0]*_arr[0] + _arr[1]*_arr[1] + _arr[2]*_arr[2]);
	}
	void normalize()
	{
		T mag = magnitude();
		_arr[0] /= mag;
		_arr[1] /= mag;
		_arr[2] /= mag;
	}
	const T dotProduct(const Vector3<T>& b) const
	{
		return this->_arr[0]*b._arr[0] + this->_arr[1]*b._arr[1] + this->_arr[2]*b._arr[2];
	}
	const Vector3<T> crossProduct(const Vector3<T>& b) const
	{
		Vector3<T> a = *this;
		a._arr[0] = this->_arr[1]*b._arr[2] - this->_arr[2]*b._arr[1];
		a._arr[1] = this->_arr[2]*b._arr[0] - this->_arr[0]*b._arr[2];
		a._arr[2] = this->_arr[0]*b._arr[1] - this->_arr[1]*b._arr[0];
		return a;
	}
	static const Vector3<T> lerp(const Vector3<T>& A, const Vector3<T>& B, T interpolation)
	{
		//interpolation goes from 1 to 0
		// y = y0 + (y1-y0) (x-0)/(1-0)
		return (A + (B - A) * (interpolation));
	}
	const Vector3<T>& operator=(const Vector3<T>& rhs)
	{
		if(this == & rhs)
			return *this;
		else
		{
			_arr[0]=rhs._arr[0];
			_arr[1]=rhs._arr[1];
			_arr[2]=rhs._arr[2];
		}
		return *this;
	}
	void operator+=(const Vector3<T>& rhs)
	{
		_arr[0]+=rhs._arr[0];
		_arr[1]+=rhs._arr[1];
		_arr[2]+=rhs._arr[2];
	}
	void operator-=(const Vector3<T>& rhs)
	{
		_arr[0]-=rhs._arr[0];
		_arr[1]-=rhs._arr[1];
		_arr[2]-=rhs._arr[2];
	}
	const Vector3<T> operator*(const T val) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]*=val;
		temp._arr[1]*=val;
		temp._arr[2]*=val;
		return temp;
	}
	void operator*=(const T val)
	{
		_arr[0]*=val;
		_arr[1]*=val;
		_arr[2]*=val;
	}
	const Vector3<T> operator*(const Vector3<T>& rhs) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]*=rhs._arr[0];
		temp._arr[1]*=rhs._arr[1];
		temp._arr[2]*=rhs._arr[2];
		return temp;
	}
	const Vector3<T> operator/(const T val) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]/=val;
		temp._arr[1]/=val;
		temp._arr[2]/=val;
		return temp;
	}
	void operator/=(const T val)
	{
		_arr[0]/=val;
		_arr[1]/=val;
		_arr[2]/=val;
	}
	const Vector3<T> operator/(const Vector3<T>& rhs) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]/=rhs._arr[0];
		temp._arr[1]/=rhs._arr[1];
		temp._arr[2]/=rhs._arr[2];
		return temp;
	}
	const Vector3<T> operator+(const Vector3<T>& rhs) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]+=rhs._arr[0];
		temp._arr[1]+=rhs._arr[1];
		temp._arr[2]+=rhs._arr[2];
		return temp;
	}
	const Vector3<T> operator-(const Vector3<T>& rhs) const
	{
		Vector3<T> temp = *this;
		temp._arr[0]-=rhs._arr[0];
		temp._arr[1]-=rhs._arr[1];
		temp._arr[2]-=rhs._arr[2];
		return temp;
	}
	/*const T operator[](size_t n) //dont like using this
	{
		assert(n <= 2);
		return _arr[n];
	}*/
private:
};

#endif

