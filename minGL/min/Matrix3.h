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
#ifndef __MATRIX3__
#define __MATRIX3__
#include "stdafx.h"
#include <math.h>
#include "Vector2.h"
#include "Matrix2.h"

template <class T> class Matrix3
{
public:
	static const int Matrix3<T>::IDENTITY_MATRIX=1947;
	Matrix3()
	{
		memset(_mat, 0, sizeof(_mat));
	}
	Matrix3(int mode)
	{
		if(mode == IDENTITY_MATRIX)
		{
			const T temp[9] = {1,0,0,0,1,0,0,0,1};
			memcpy(_mat, temp, sizeof(temp));
		}
	}
	Matrix3(const Vector2<T>& pos)
	{
		const T temp[9] = {1,0,0,0,1,0,0,0,1};
		memcpy(_mat, temp, sizeof(temp));
		this->SetTranslation(pos);
	}
	Matrix3(const Matrix2<T>& mat)
	{
		const T temp[9] = {1,0,0,0,1,0,0,0,1};
		memcpy(_mat, temp, sizeof(temp));
		this->SetRotation(mat);
	}
	Matrix3(const Vector2<T>& pos, const Matrix2<T>& mat)
	{
		const T temp[9] = {1,0,0,0,1,0,0,0,1};
		memcpy(_mat, temp, sizeof(temp));
		this->SetTranslation(pos);
		this->SetRotation(mat);
	}
	Matrix3(const Vector2<T>& pos, const Matrix2<T>& mat, const Vector2<T>& scale)
	{
		const T temp[9] = {1,0,0,0,1,0,0,0,1};
		memcpy(_mat, temp, sizeof(temp));
		this->SetScale(scale);
		this->RotateMatrix(mat);
		this->TranslateMatrix(pos);
	}
	~Matrix3() {}
	void MultiplyMatrices(const Matrix3* m2)
	{
		Matrix3 temp;
		temp._mat[0] = this->_mat[0] * m2->_mat[0] + this->_mat[3] * m2->_mat[1] + this->_mat[6] * m2->_mat[2];
		temp._mat[1] = this->_mat[1] * m2->_mat[0] + this->_mat[4] * m2->_mat[1] + this->_mat[7] * m2->_mat[2];
		temp._mat[2] = this->_mat[2] * m2->_mat[0] + this->_mat[5] * m2->_mat[1] + this->_mat[8] * m2->_mat[2];
		temp._mat[3] = this->_mat[0] * m2->_mat[3] + this->_mat[3] * m2->_mat[4] + this->_mat[6] * m2->_mat[5];
		temp._mat[4] = this->_mat[1] * m2->_mat[3] + this->_mat[4] * m2->_mat[4] + this->_mat[7] * m2->_mat[5];
		temp._mat[5] = this->_mat[2] * m2->_mat[3] + this->_mat[5] * m2->_mat[4] + this->_mat[8] * m2->_mat[5];
		temp._mat[6] = this->_mat[0] * m2->_mat[6] + this->_mat[3] * m2->_mat[7] + this->_mat[6] * m2->_mat[8];
		temp._mat[7] = this->_mat[1] * m2->_mat[6] + this->_mat[4] * m2->_mat[7] + this->_mat[7] * m2->_mat[8];
		temp._mat[8] = this->_mat[2] * m2->_mat[6] + this->_mat[5] * m2->_mat[7] + this->_mat[8] * m2->_mat[8];

		*(this) = temp;
	}
	void TranslateMatrix(T x, T y)
	{
		Matrix3 translation = Matrix3<T>(IDENTITY_MATRIX);
		
		translation._mat[6] = x;
		translation._mat[7] = y;

		this->MultiplyMatrices(&translation);
	}
	void TranslateMatrix(const Vector2<T>& vec)
	{
		Matrix3 translation = Matrix3<T>(IDENTITY_MATRIX);
		
		translation._mat[6] = vec.X();
		translation._mat[7] = vec.Y();

		this->MultiplyMatrices(&translation);
	}
	const Vector2<T> GetTranslation()
	{
		Vector2<T> pos;
		pos._arr[0] = translation._mat[6];
		pos._arr[1] = translation._mat[7];
	}
	void SetTranslation(const Vector2<T>& vec)
	{
		this->_mat[6] = vec.X();
		this->_mat[7] = vec.Y();
	}
	void RotateClockwise(T degAngle)
	{
		Matrix3 rotation = Matrix3(IDENTITY_MATRIX);
		T sine = (T)sin(angle);
		T cosine = (T)cos(angle);

		rotation._mat[0] = cosine;
		rotation._mat[1] = -sine;
		rotation._mat[3] = sine;
		rotation._mat[4] = cosine;

		this->MultiplyMatrices(&rotation);
	}
	void RotateCounterClockwise(T degAngle)
	{
		Matrix3 rotation = Matrix3(IDENTITY_MATRIX);
		T sine = (T)sin(angle);
		T cosine = (T)cos(angle);

		rotation._mat[0] = cosine;
		rotation._mat[1] = sine;
		rotation._mat[3] = -sine;
		rotation._mat[4] = cosine;

		this->MultiplyMatrices(&rotation);
	}
	void RotateMatrix(const Matrix2<T>& mat)
	{
		Matrix3 rot;
		rot._mat[0] = mat._mat[0]; 
		rot._mat[1] = mat._mat[1]; 
		rot._mat[3] = mat._mat[2]; 
		rot._mat[4] = mat._mat[3];

		this->MultiplyMatrices(&rot);
	}
	const Matrix2<T> GetRotation()
	{
		Matrix2<T> rot;
		rot._mat[0] = this->_mat[0]; 
		rot._mat[1] = this->_mat[1]; 
		rot._mat[2] = this->_mat[3]; 
		rot._mat[3] = this->_mat[4];
		return rot;
	}
	void SetRotation(const Matrix2<T>& rot)
	{
		this->_mat[0] = rot._mat[0]; 
		this->_mat[1] = rot._mat[1]; 
		this->_mat[3] = rot._mat[2]; 
		this->_mat[4] = rot._mat[3];
	}
	void SetScale(const Vector2<T>& vec)
	{
		this->_mat[0] = vec._arr[0]; 
		this->_mat[4] = vec._arr[1]; 
	}
	void ScaleMatrix(T x, T y)
	{
		Matrix3 scale = Matrix3(IDENTITY_MATRIX);
		scale._mat[0] = x; 
		scale._mat[4] = y;

		this->MultiplyMatrices(&rotation);
	}
	void ScaleMatrix(const Vector2<T>& vec)
	{
		Matrix3 scale = Matrix3(IDENTITY_MATRIX);
		scale._mat[0] = vec._arr[0]; 
		scale._mat[4] = vec._arr[1];

		this->MultiplyMatrices(&rotation);
	}
	void Inverse()
	{
		pfd inv[9], det;

		inv[0] = m[4]*m[8] - m[7]*m[5];
		inv[1] = m[7]*m[2] - m[1]*m[8];
		inv[2] = m[1]*m[5] - m[4]*m[2];
		inv[3] = m[6]*m[5] - m[3]*m[8];
		inv[4] = m[0]*m[8] - m[6]*m[2];
		inv[5] = m[3]*m[2] - m[0]*m[5];
		inv[6] = m[3]*m[7] - m[6]*m[4];
		inv[7] = m[6]*m[1] - m[0]*m[7];
		inv[8] = m[0]*m[4] - m[3]*m[1];
		
		det = m[0]*m[4]*m[8] + m[1]*m[5]*m[6] + m[2]*m[3]*m[7] - m[0]*m[5]*m[7] - m[2]*m[4]*m[6] - m[1]*m[3]*m[8];

		if (det == 0)
			return false;

		det = (T)1.0 / det;

		for (i = 0; i < 9; i++)
		{
			_mat[i] = inv[i] * det;
		}
		return true;
	}
	void Transpose()
	{
		pfd temp = _mat[1];
		_mat[1] = _mat[3];
		_mat[3] = temp;

		temp = _mat[2];
		_mat[2] = _mat[6];
		_mat[6] = temp;

		temp = _mat[5];
		_mat[5] = _mat[7];
		_mat[7] = temp;
	}
	T _mat[9];
	Matrix3 & operator=(const Matrix3& m2)
	{
		if(this != &m2)
		{
			memcpy(_mat, m2._mat, sizeof(_mat));
		}
		return *this;
	}
private:
	Matrix3(const Matrix3& m2)
	{
		if(this != &m2)
		{
			memcpy(_mat, m2._mat, sizeof(_mat));
		}
	}
};

#endif