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
#ifndef __VECTOR2__
#define __VECTOR2__

#include "stdafx.h"
#include <math.h>
#define _USE_MATH_DEFINES
template <class T> class Vector2
{
public:
	Vector2(void)
	{
		_arr[0]=0;
		_arr[1]=0;
	}
	Vector2(T x, T y)
	{
		_arr[0]=x;
		_arr[1]=y;
	}
	Vector2(const Vector2<T>& copy)
	{
		_arr[0]=copy._arr[0];
		_arr[1]=copy._arr[1];
	}
	~Vector2(void){}
	T _arr[2];
	T X() const
	{
		return _arr[0];
	}
	T Y() const
	{
		return _arr[1];
	}
	T magnitude() const
	{
		return sqrt(_arr[0]*_arr[0] + _arr[1]*_arr[1]);
	}
	void normalize()
	{
		T mag = magnitude();
		_arr[0] /= mag;
		_arr[1] /= mag;
	}
	const T dotProduct(const Vector2<T>& b) const
	{
		return this->_arr[0]*b._arr[0] + this->_arr[1]*b._arr[1];
	}
	const T crossProduct(const Vector2<T>& b) const
	{
		 return this->_arr[0]*b._arr[1] - this->_arr[1]*b._arr[0];
		
	}
	const Vector2<T>& operator=(const Vector2<T>& rhs)
	{
		if(this == & rhs)
			return *this;
		else
		{
			_arr[0]=rhs._arr[0];
			_arr[1]=rhs._arr[1];
		}
		return *this;
	}
	const Vector2<T>& operator+=(const Vector2<T>& rhs)
	{
		_arr[0]+=rhs._arr[0];
		_arr[1]+=rhs._arr[1];
		return *this;
	}
	const Vector2<T>& operator-=(const Vector2<T>& rhs)
	{
		_arr[0]-=rhs._arr[0];
		_arr[1]-=rhs._arr[1];
		return *this;
	}
	const Vector2<T> operator*(const T val) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]*=val;
		temp._arr[1]*=val;
		return temp;
	}
	void operator*=(const T val)
	{
		_arr[0]*=val;
		_arr[1]*=val;
	}
	const Vector2<T> operator*(const Vector2<T>& rhs) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]*=rhs._arr[0];
		temp._arr[1]*=rhs._arr[1];
		return temp;
	}
	const Vector2<T> operator/(const T val) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]/=val;
		temp._arr[1]/=val;
		return temp;
	}
	const Vector2<T> operator/(const Vector2<T>& rhs) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]/=rhs._arr[0];
		temp._arr[1]/=rhs._arr[1];
		return temp;
	}
	const Vector2<T> operator+(const Vector2<T>& rhs) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]+=rhs._arr[0];
		temp._arr[1]+=rhs._arr[1];
		return temp;
	}
	const Vector2<T> operator-(const Vector2<T>& rhs) const
	{
		Vector2<T> temp = *this;
		temp._arr[0]-=rhs._arr[0];
		temp._arr[1]-=rhs._arr[1];
		return temp;
	}
	/*const T operator[](size_t n) 
	{
		return _arr[n];
	}*/
};

#endif


