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
#ifndef __MATRIX__
#define __MATRIX__
#include "stdafx.h"
const int IDENTITY_MATRIX=1947;

#include <math.h>

#include "Vector3.h"
#include "Quaternion.h"

class Matrix
{
public:
	Matrix();
	Matrix(int);
	Matrix(const Vector3<pfd>& pos);
	Matrix(const Quaternion<pfd>& rot);
	Matrix(const Quaternion<pfd>& rot, const Vector3<pfd>& pos);
	Matrix(const Quaternion<pfd>& rot, const Vector3<pfd>& pos, const Vector3<pfd>& scale);
	Matrix(const Matrix&);
	~Matrix();
	void MultiplyMatrices(const Matrix* m2);
	void RotateAboutX(float angle);
	void RotateAboutY(float angle);
	void RotateAboutZ(float angle);
	void ScaleMatrix(float x, float y, float z);
	void ScaleMatrix(const Vector3<pfd>& vec);
	void TranslateMatrix(float x, float y, float z);
	void TranslateMatrix(const Vector3<pfd>& vec);
	void SetTranslation(const Vector3<pfd>& vec);
	const Vector3<pfd> GetTranslation();
	void SetRotation(const Quaternion<pfd>& q);
	void SetScale(const Vector3<pfd>& vec);
	void RotateMatrix(const Quaternion<pfd>& q);
	bool Inverse();
	void Transpose();
	float _mat[16];
	Matrix & operator=(const Matrix&);
	Vector4<pfd> Transform(const Vector4<pfd> & A) const;
private:
};


#endif