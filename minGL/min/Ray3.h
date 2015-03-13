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
#ifndef __RAY3__
#define __RAY3__
#include "stdafx.h"
#include "Vector3.h"

template <class T> class Ray3
{
public:
	Ray3(Vector3<T> &origin, Vector3<T> &destination)
	{
		_origin = origin;
		_direction = destination - origin;
		_direction.normalize();
		_inv_direction = Vector3<T>((T)1.0/_direction.X(), (T)1.0/_direction.Y(), (T)1.0/_direction.Z());
	}
	~Ray3() {}
	Ray3& operator=(Ray3<T>& A)
	{
		if(this !=& A)
		{
			_origin = A._origin;
			_direction = A._direction;
			_inv_direction = A._inv_direction;
		}
		return *this;
	}
	Vector3<T> _origin;
	Vector3<T> _direction;
	Vector3<T> _inv_direction;
protected:
	Ray3(Ray3<T>& A)
	{
		if(this !=& A)
		{
			_origin = A._origin;
			_direction = A._direction;
			_inv_direction = A._inv_direction;
		}
	}
};

#endif
