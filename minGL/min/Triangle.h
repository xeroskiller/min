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
#ifndef __TRIANGLE__
#define __TRIANGLE

template <class T> class Vertex;
#include "Vertex.h"

template <class T> class Triangle
{
public:
	Triangle(T index0, T index1, T index2, size_t id)
	{
		_vertices[0] = Vertex<T>(index0, this);
		_vertices[1] = Vertex<T>(index1, this);
		_vertices[2] = Vertex<T>(index2, this);
		_id = id;
	}
	const Triangle<T>& operator=(const Triangle<T>& A)
	{
		if(this != &A)
		{
			_vertices[0] = A._vertices[0];
			_vertices[0]._triangle = this;
			_vertices[1] = A._vertices[1];
			_vertices[1]._triangle = this;
			_vertices[2] = A._vertices[2];
			_vertices[2]._triangle = this;
			_id = A._id;
		}
		return *this;
	}
	Triangle(const Triangle<T> & A)
	{
		if(this != &A)
		{
			_vertices[0] = A._vertices[0];
			_vertices[0]._triangle = this;
			_vertices[1] = A._vertices[1];
			_vertices[1]._triangle = this;
			_vertices[2] = A._vertices[2];
			_vertices[2]._triangle = this;
			_id = A._id;
		}
	}
	Vertex<T>* getVertices()
	{
		return _vertices;
	}
	size_t getId()
	{
		return _id;
	}
	~Triangle() {};
protected:
	Vertex<T> _vertices[3];
	size_t _id;
};

#endif


