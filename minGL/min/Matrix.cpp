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
#include "StdAfx.h"
#include "Matrix.h"

Matrix::Matrix()
{
	memset(_mat, 0, sizeof(_mat));
}

Matrix::Matrix(int mode)
{
	if(mode == IDENTITY_MATRIX)
	{
		const float temp[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		memcpy(_mat, temp, sizeof(temp));
	}
}

Matrix::Matrix(const Vector3<pfd>& pos)
{
	const float temp[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	memcpy(_mat, temp, sizeof(temp));
	this->SetTranslation(pos);
}

Matrix::Matrix(const Quaternion<pfd>& q)
{
	const float temp[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	memcpy(_mat, temp, sizeof(temp));
	this->SetRotation(q);
}

Matrix::Matrix(const Quaternion<pfd>& q, const Vector3<pfd>& pos)
{
	const float temp[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	memcpy(_mat, temp, sizeof(temp));
	this->SetRotation(q);
	this->SetTranslation(pos);
}

Matrix::Matrix(const Quaternion<pfd>& q, const Vector3<pfd>& pos, const Vector3<pfd>& scale)
{
	const float temp[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	memcpy(_mat, temp, sizeof(temp));
	this->SetScale(scale);
	this->RotateMatrix(q);
	this->TranslateMatrix(pos);
}

Matrix::~Matrix() {}

void Matrix::MultiplyMatrices(const Matrix* m2)
{
	Matrix temp;
	size_t row, column, row_offset;

	for (row = 0, row_offset = row * 4; row < 4; ++row, row_offset = row * 4)
	{
		for (column = 0; column < 4; ++column)
		{
			temp._mat[row_offset + column] =
				(this->_mat[column + 0]  * m2->_mat[row_offset + 0]) +
				(this->_mat[column + 4]  * m2->_mat[row_offset + 1]) +
				(this->_mat[column + 8]  * m2->_mat[row_offset + 2]) +
				(this->_mat[column + 12] * m2->_mat[row_offset + 3]);
		}
	}
	*(this) = temp;
}

Vector4<pfd> Matrix::Transform(const Vector4<pfd> & A) const
{
	Vector4<pfd> temp;
	for (size_t column = 0; column < 4; ++column)
	{
		temp._arr[column] = 
			this->_mat[column + 0]  * A._arr[0] + 
			this->_mat[column + 4]  * A._arr[1] + 
			this->_mat[column + 8]  * A._arr[2] + 
			this->_mat[column + 12] * A._arr[3];
	}
	return temp;
}

void Matrix::RotateAboutX(float angle)
{
	Matrix rotation = Matrix(IDENTITY_MATRIX);
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation._mat[5] = cosine;
	rotation._mat[6] = -sine;
	rotation._mat[9] = sine;
	rotation._mat[10] = cosine;

	this->MultiplyMatrices(&rotation);
}

void Matrix::RotateAboutY(float angle)
{
	Matrix rotation = Matrix(IDENTITY_MATRIX);
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation._mat[0] = cosine;
	rotation._mat[8] = sine;
	rotation._mat[2] = -sine;
	rotation._mat[10] = cosine;

	this->MultiplyMatrices(&rotation);
}

void Matrix::RotateAboutZ(float angle)
{
	Matrix rotation = Matrix(IDENTITY_MATRIX);
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);

	rotation._mat[0] = cosine;
	rotation._mat[1] = sine;
	rotation._mat[4] = -sine;
	rotation._mat[5] = cosine;

	this->MultiplyMatrices(&rotation);
}

void Matrix::ScaleMatrix(float x, float y, float z)
{
	Matrix translation = Matrix(IDENTITY_MATRIX);

	translation._mat[0] = x;
	translation._mat[5] = y;
	translation._mat[10] = z;

	this->MultiplyMatrices(&translation);
}

void Matrix::ScaleMatrix(const Vector3<pfd>& scale)
{
	Matrix translation = Matrix(IDENTITY_MATRIX);

	translation._mat[0] = scale._arr[0];
	translation._mat[5] = scale._arr[1];
	translation._mat[10] = scale._arr[2];

	this->MultiplyMatrices(&translation);
}

void Matrix::TranslateMatrix(float x, float y, float z)
{
	Matrix translation = Matrix(IDENTITY_MATRIX);

	translation._mat[12] = x;
	translation._mat[13] = y;
	translation._mat[14] = z;

	this->MultiplyMatrices(&translation);
}

void Matrix::TranslateMatrix(const Vector3<pfd>& pos)
{
	Matrix translation = Matrix(IDENTITY_MATRIX);

	translation._mat[12] = pos._arr[0];
	translation._mat[13] = pos._arr[1];
	translation._mat[14] = pos._arr[2];

	this->MultiplyMatrices(&translation);
}

void Matrix::RotateMatrix(const Quaternion<pfd>& rot)
{
	Matrix rotation = Matrix(IDENTITY_MATRIX);
	rotation._mat[0] = (pfd)1.0 - 2*rot._arr[2]*rot._arr[2] - 2*rot._arr[3]*rot._arr[3];
	rotation._mat[1] = 2*rot._arr[1]*rot._arr[2] + 2*rot._arr[3]*rot._arr[0];
	rotation._mat[2] = 2*rot._arr[1]*rot._arr[3] - 2*rot._arr[2]*rot._arr[0];
	//
	rotation._mat[4] = 2*rot._arr[1]*rot._arr[2] - 2*rot._arr[3]*rot._arr[0];
	rotation._mat[5] = (pfd)1.0 - 2*rot._arr[1]*rot._arr[1] - 2*rot._arr[3]*rot._arr[3];
	rotation._mat[6] = 2*rot._arr[2]*rot._arr[3] + 2*rot._arr[1]*rot._arr[0];
	//
	rotation._mat[8] = 2*rot._arr[1]*rot._arr[3] + 2*rot._arr[2]*rot._arr[0];
	rotation._mat[9] = 2*rot._arr[2]*rot._arr[3] - 2*rot._arr[1]*rot._arr[0];
	rotation._mat[10] = (pfd)1.0 - 2*rot._arr[1]*rot._arr[1] - 2*rot._arr[2]*rot._arr[2];

	this->MultiplyMatrices(&rotation);
}

const Vector3<pfd> Matrix::GetTranslation()
{
	Vector3<pfd> temp;
	temp._arr[0] = _mat[12];
	temp._arr[0] = _mat[13];
	temp._arr[0] = _mat[14];
	return temp;
}

void Matrix::SetTranslation(const Vector3<pfd>& pos)
{
	_mat[12] = pos._arr[0];
	_mat[13] = pos._arr[1];
	_mat[14] = pos._arr[2];
}

void Matrix::SetRotation(const Quaternion<pfd>& rot)
{
	_mat[0] = (pfd)1.0 - 2*rot._arr[2]*rot._arr[2] - 2*rot._arr[3]*rot._arr[3];
	_mat[1] = 2*rot._arr[1]*rot._arr[2] + 2*rot._arr[3]*rot._arr[0];
	_mat[2] = 2*rot._arr[1]*rot._arr[3] - 2*rot._arr[2]*rot._arr[0];
	//
	_mat[4] = 2*rot._arr[1]*rot._arr[2] - 2*rot._arr[3]*rot._arr[0];
	_mat[5] = (pfd)1.0 - 2*rot._arr[1]*rot._arr[1] - 2*rot._arr[3]*rot._arr[3];
	_mat[6] = 2*rot._arr[2]*rot._arr[3] + 2*rot._arr[1]*rot._arr[0];
	//
	_mat[8] = 2*rot._arr[1]*rot._arr[3] + 2*rot._arr[2]*rot._arr[0];
	_mat[9] = 2*rot._arr[2]*rot._arr[3] - 2*rot._arr[1]*rot._arr[0];
	_mat[10] = (pfd)1.0 - 2*rot._arr[1]*rot._arr[1] - 2*rot._arr[2]*rot._arr[2];
}
void Matrix::SetScale(const Vector3<pfd>& scale)
{
	_mat[0] = scale._arr[0];
	_mat[5] = scale._arr[1];
	_mat[10] = scale._arr[2];
}

Matrix & Matrix::operator=(const Matrix& m2)
{
	if(this != &m2)
	{
		memcpy(this->_mat, m2._mat, sizeof(m2));
	}
	return *this;
}

Matrix::Matrix(const Matrix& m2)
{
	if(this != &m2)
	{
		memcpy(this->_mat, m2._mat, sizeof(m2));
	}
	return;
}

bool Matrix::Inverse()
{
	pfd inv[16], det;
    int i;

    inv[0] = _mat[5]  * _mat[10] * _mat[15] - 
             _mat[5]  * _mat[11] * _mat[14] - 
             _mat[9]  * _mat[6]  * _mat[15] + 
             _mat[9]  * _mat[7]  * _mat[14] +
             _mat[13] * _mat[6]  * _mat[11] - 
             _mat[13] * _mat[7]  * _mat[10];

    inv[4] = -_mat[4]  * _mat[10] * _mat[15] + 
              _mat[4]  * _mat[11] * _mat[14] + 
              _mat[8]  * _mat[6]  * _mat[15] - 
              _mat[8]  * _mat[7]  * _mat[14] - 
              _mat[12] * _mat[6]  * _mat[11] + 
              _mat[12] * _mat[7]  * _mat[10];

    inv[8] = _mat[4]  * _mat[9] * _mat[15] - 
             _mat[4]  * _mat[11] * _mat[13] - 
             _mat[8]  * _mat[5] * _mat[15] + 
             _mat[8]  * _mat[7] * _mat[13] + 
             _mat[12] * _mat[5] * _mat[11] - 
             _mat[12] * _mat[7] * _mat[9];

    inv[12] = -_mat[4]  * _mat[9] * _mat[14] + 
               _mat[4]  * _mat[10] * _mat[13] +
               _mat[8]  * _mat[5] * _mat[14] - 
               _mat[8]  * _mat[6] * _mat[13] - 
               _mat[12] * _mat[5] * _mat[10] + 
               _mat[12] * _mat[6] * _mat[9];

    inv[1] = -_mat[1]  * _mat[10] * _mat[15] + 
              _mat[1]  * _mat[11] * _mat[14] + 
              _mat[9]  * _mat[2] * _mat[15] - 
              _mat[9]  * _mat[3] * _mat[14] - 
              _mat[13] * _mat[2] * _mat[11] + 
              _mat[13] * _mat[3] * _mat[10];

    inv[5] = _mat[0]  * _mat[10] * _mat[15] - 
             _mat[0]  * _mat[11] * _mat[14] - 
             _mat[8]  * _mat[2] * _mat[15] + 
             _mat[8]  * _mat[3] * _mat[14] + 
             _mat[12] * _mat[2] * _mat[11] - 
             _mat[12] * _mat[3] * _mat[10];

    inv[9] = -_mat[0]  * _mat[9] * _mat[15] + 
              _mat[0]  * _mat[11] * _mat[13] + 
              _mat[8]  * _mat[1] * _mat[15] - 
              _mat[8]  * _mat[3] * _mat[13] - 
              _mat[12] * _mat[1] * _mat[11] + 
              _mat[12] * _mat[3] * _mat[9];

    inv[13] = _mat[0]  * _mat[9] * _mat[14] - 
              _mat[0]  * _mat[10] * _mat[13] - 
              _mat[8]  * _mat[1] * _mat[14] + 
              _mat[8]  * _mat[2] * _mat[13] + 
              _mat[12] * _mat[1] * _mat[10] - 
              _mat[12] * _mat[2] * _mat[9];

    inv[2] = _mat[1]  * _mat[6] * _mat[15] - 
             _mat[1]  * _mat[7] * _mat[14] - 
             _mat[5]  * _mat[2] * _mat[15] + 
             _mat[5]  * _mat[3] * _mat[14] + 
             _mat[13] * _mat[2] * _mat[7] - 
             _mat[13] * _mat[3] * _mat[6];

    inv[6] = -_mat[0]  * _mat[6] * _mat[15] + 
              _mat[0]  * _mat[7] * _mat[14] + 
              _mat[4]  * _mat[2] * _mat[15] - 
              _mat[4]  * _mat[3] * _mat[14] - 
              _mat[12] * _mat[2] * _mat[7] + 
              _mat[12] * _mat[3] * _mat[6];

    inv[10] = _mat[0]  * _mat[5] * _mat[15] - 
              _mat[0]  * _mat[7] * _mat[13] - 
              _mat[4]  * _mat[1] * _mat[15] + 
              _mat[4]  * _mat[3] * _mat[13] + 
              _mat[12] * _mat[1] * _mat[7] - 
              _mat[12] * _mat[3] * _mat[5];

    inv[14] = -_mat[0]  * _mat[5] * _mat[14] + 
               _mat[0]  * _mat[6] * _mat[13] + 
               _mat[4]  * _mat[1] * _mat[14] - 
               _mat[4]  * _mat[2] * _mat[13] - 
               _mat[12] * _mat[1] * _mat[6] + 
               _mat[12] * _mat[2] * _mat[5];

    inv[3] = -_mat[1] * _mat[6] * _mat[11] + 
              _mat[1] * _mat[7] * _mat[10] + 
              _mat[5] * _mat[2] * _mat[11] - 
              _mat[5] * _mat[3] * _mat[10] - 
              _mat[9] * _mat[2] * _mat[7] + 
              _mat[9] * _mat[3] * _mat[6];

    inv[7] = _mat[0] * _mat[6] * _mat[11] - 
             _mat[0] * _mat[7] * _mat[10] - 
             _mat[4] * _mat[2] * _mat[11] + 
             _mat[4] * _mat[3] * _mat[10] + 
             _mat[8] * _mat[2] * _mat[7] - 
             _mat[8] * _mat[3] * _mat[6];

    inv[11] = -_mat[0] * _mat[5] * _mat[11] + 
               _mat[0] * _mat[7] * _mat[9] + 
               _mat[4] * _mat[1] * _mat[11] - 
               _mat[4] * _mat[3] * _mat[9] - 
               _mat[8] * _mat[1] * _mat[7] + 
               _mat[8] * _mat[3] * _mat[5];

    inv[15] = _mat[0] * _mat[5] * _mat[10] - 
              _mat[0] * _mat[6] * _mat[9] - 
              _mat[4] * _mat[1] * _mat[10] + 
              _mat[4] * _mat[2] * _mat[9] + 
              _mat[8] * _mat[1] * _mat[6] - 
              _mat[8] * _mat[2] * _mat[5];

    det = _mat[0] * inv[0] + _mat[1] * inv[4] + _mat[2] * inv[8] + _mat[3] * inv[12];

    if (det == 0)
        return false;

    det = (pfd)1.0 / det;

    for (i = 0; i < 16; i++)
	{
        _mat[i] = inv[i] * det;
	}
	return true;
}

void Matrix::Transpose()
{
	pfd temp = _mat[1];
	_mat[1] = _mat[4];
	_mat[4] = temp;

	temp = _mat[2];
	_mat[2] = _mat[8];
	_mat[8] = temp;

	temp = _mat[6];
	_mat[6] = _mat[9];
	_mat[9] = temp;

	temp = _mat[3];
	_mat[3] = _mat[12];
	_mat[12] = temp;

	temp = _mat[7];
	_mat[7] = _mat[13];
	_mat[13] = temp;

	temp = _mat[11];
	_mat[11] = _mat[14];
	_mat[14] = temp;
}