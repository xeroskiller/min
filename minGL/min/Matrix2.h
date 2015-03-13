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
#ifndef __MATRIX2__
#define __MATRIX2__
#include "stdafx.h"
template <class T> class Matrix2
{
public:
	static const T PI;

	Matrix2()
	{
		memset(_mat, 0, sizeof(_mat));
		T sine = (T)sin((T)0.0);
		T cosine = (T)cos((T)0.0);

		_mat[0] = cosine;
		_mat[1] = -sine;
		_mat[2] = sine;
		_mat[3] = cosine;

	}
	Matrix2(int)
	{
		if(mode == IDENTITY_MATRIX)
		{
			const float temp[4] = {1,0,0,1};
			memcpy(_mat, temp, sizeof(temp));
		}
	}
	static T DegreesToRadians(T degrees)
	{
		return degrees*(T)(PI/180);
	}

	Matrix2(T degAngle)
	{
		if(degAngle > 0)
		{
			T posAngle = DegreesToRadians(degAngle);
			T sine = (T)sin(posAngle);
			T cosine = (T)cos(posAngle);

			_mat[0] = cosine;
			_mat[1] = -sine;
			_mat[2] = sine;
			_mat[3] = cosine;
		}
		else
		{
			T posAngle = DegreesToRadians(abs(degAngle));
			T sine = (T)sin(posAngle);
			T cosine = (T)cos(posAngle);

			_mat[0] = cosine;
			_mat[1] = sine;
			_mat[2] = -sine;
			_mat[3] = cosine;
		}
	}
	~Matrix2() {}
	void MultiplyMatrices(const Matrix2* m2)
	{
		Matrix2 temp;
		temp._mat[0] = this->_mat[0] * m2->_mat[0] + this->_mat[2] * m2->_mat[1];
		temp._mat[1] = this->_mat[1] * m2->_mat[0] + this->_mat[3] * m2->_mat[1];
		temp._mat[2] = this->_mat[0] * m2->_mat[2] + this->_mat[2] * m2->_mat[3];
		temp._mat[3] = this->_mat[1] * m2->_mat[2] + this->_mat[3] * m2->_mat[3];

		*(this) = temp;
	}
	const Vector2<pfd> MultiplyVector(const Vector2<pfd>& vec)
	{
		Vector2<pfd> rot;
		rot._arr[0] = this->_mat[0] * vec._arr[0] + this->_mat[2] * vec._arr[1];
		rot._arr[1] = this->_mat[1] * vec._arr[0] + this->_mat[3] * vec._arr[1];
		return rot;
	}
	T _mat[4];
	Matrix2& operator=(const Matrix2& m2)
	{
		if(this != &m2)
		{
			memcpy(_mat, m2._mat, sizeof(_mat));
		}
		return *this;
	}
	Matrix2(const Matrix2& m2)
	{
		if(this != &m2)
		{
			memcpy(_mat, m2._mat, sizeof(_mat));
		}
	}
	bool inverse()
	{
		pfd mat[4];
		mat[0] = _mat[0];
		mat[1] = _mat[1];
		mat[2] = _mat[2];
		mat[3] = _mat[3];
		pfd det = 1 / (_mat[0]*_mat[3] - _mat[1]*_mat[2]);
		if(det = 0)
			return false;
		_mat[0] = mat[3] * det;
		_mat[1] = -mat[2] * det;
		_mat[2] = -mat[1] * det;
		_mat[3] = mat[0] * det;
	}
	void transpose()
	{
		pfd temp = _mat[1];
		_mat[1] = _mat[2];
		_mat[2] = temp;
	}
private:
	
};

template <class T> const T Matrix2<T>::PI = 3.14159265358979323846f;

#endif