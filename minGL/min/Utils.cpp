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
#include "Utils.h"

float Cotangent(float angle)
{
	return (float)(1.0 / tan(angle));
}

float DegreesToRadians(float degrees)
{
	return degrees*(float)(PI/180);
}

float RadiansToDegrees(float radians)
{
	return radians * (float)(180/PI);
}

void glhPerspectivef2(float *matrix, float fovyInDegrees, float aspectRatio,
                      float znear, float zfar)
{
    float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * PI / 360.0f);
    //ymin = -ymax;
    //xmin = -ymax * aspectRatio;
    xmax = ymax * aspectRatio;
    glhFrustumf2(matrix, -xmax, xmax, -ymax, ymax, znear, zfar);
}

void glhFrustumf2(float *matrix, float left, float right, float bottom, float top,
                  float znear, float zfar)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0] = temp / temp2;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = temp / temp3;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = (right + left) / temp2;
    matrix[9] = (top + bottom) / temp3;
    matrix[10] = (-zfar - znear) / temp4;
    matrix[11] = -1.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = (-temp * zfar) / temp4;
    matrix[15] = 0.0;
}


void glhLookAtf1( Matrix *matrix, float *eyePosition3D,
                  float *center3D, float *upVector3D )
{
   float forward[3], side[3], up[3];
   Matrix matrix2;

   //------------------
   forward[0] = center3D[0] - eyePosition3D[0];
   forward[1] = center3D[1] - eyePosition3D[1];
   forward[2] = center3D[2] - eyePosition3D[2];
   NormalizeVector(forward);
   //------------------
   //Side = forward x up
   ComputeNormalOfPlane(side, forward, upVector3D);
   NormalizeVector(side);
   //------------------
   //Recompute up as: up = side x forward
   ComputeNormalOfPlane(up, side, forward);
   //------------------
   matrix2._mat[0] = side[0];
   matrix2._mat[4] = side[1];
   matrix2._mat[8] = side[2];
   matrix2._mat[12] = 0.0;
   //------------------
   matrix2._mat[1] = up[0];
   matrix2._mat[5] = up[1];
   matrix2._mat[9] = up[2];
   matrix2._mat[13] = 0.0;
   //------------------
   matrix2._mat[2] = -forward[0];
   matrix2._mat[6] = -forward[1];
   matrix2._mat[10] = -forward[2];
   matrix2._mat[14] = 0.0;
   //------------------
   matrix2._mat[3] = matrix2._mat[7] = matrix2._mat[11] = 0.0;
   matrix2._mat[15] = 1.0;
   //------------------
   matrix->MultiplyMatrices(&matrix2);
   matrix->TranslateMatrix(-eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]); //translate object to origin
}

void glhLookAtf2( Matrix *matrix, float *eyePosition3D,
                  float *center3D, float *upVector3D )
{
	
   float forward[3], side[3], up[3];
   Matrix matrix2;
   *matrix = Matrix(IDENTITY_MATRIX);
   //------------------
   forward[0] = center3D[0] - eyePosition3D[0];
   forward[1] = center3D[1] - eyePosition3D[1];
   forward[2] = center3D[2] - eyePosition3D[2];
   NormalizeVector(forward);
   //------------------
   //Side = forward x up
   ComputeNormalOfPlane(side, forward, upVector3D);
   NormalizeVector(side);
   //------------------
   //Recompute up as: up = side x forward
   ComputeNormalOfPlane(up, side, forward);
   //------------------
   matrix2._mat[0] = side[0];
   matrix2._mat[4] = side[1];
   matrix2._mat[8] = side[2];
   matrix2._mat[12] = -eyePosition3D[0];
   //------------------
   matrix2._mat[1] = up[0];
   matrix2._mat[5] = up[1];
   matrix2._mat[9] = up[2];
   matrix2._mat[13] = -eyePosition3D[1];
   //------------------
   matrix2._mat[2] = -forward[0];
   matrix2._mat[6] = -forward[1];
   matrix2._mat[10] = -forward[2];
   matrix2._mat[14] = -eyePosition3D[2];
   //------------------
   matrix2._mat[3] = matrix2._mat[7] = matrix2._mat[11] = 0.0;
   matrix2._mat[15] = 1.0;
   //------------------
   matrix->_mat[12] = eyePosition3D[0];
   matrix->_mat[13] = eyePosition3D[1];
   matrix->_mat[14] = eyePosition3D[2];

   matrix->MultiplyMatrices(&matrix2);
}

void NormalizeVector(float* vec)
{
    float length = sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
    vec[0] = vec[0]/length;
    vec[1] = vec[1]/length;
    vec[2] = vec[2]/length;
}

void ComputeNormalOfPlane(float* a, float* b, float* c)
{
    a[0] = b[1]*c[2] - c[1]*b[2];
    a[1] = b[2]*c[0] - c[2]*b[0];
    a[2] = b[0]*c[1] - c[0]*b[1];
}
