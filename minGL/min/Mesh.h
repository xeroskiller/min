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
#ifndef __MESH__
#define __MESH__
#include "stdafx.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <vector>

class Mesh
{
public:
	struct Weight
    {
        int _jointID;
        float _bias;
        Vector3<pfd> _position;
    };
	struct VertexWeight
	{
		size_t startWeight;
		size_t weightCount;
	};
	friend class MD5Model;
	friend class Object;
	Mesh();
	~Mesh();
	std::vector<Vector4<pfd>> _vertices;
	std::vector<Vector4<pfd>> _bindposevertices; //bindpose vertices
	std::vector<Vector2<pfd>> _uiVertices;
	std::vector<GLIndex> _indices;
	std::vector<GLIndex> _octree;
	std::vector<Vector2<pfd>> _uvcoords;
	std::vector<Vector3<pfd>> _normals;
	std::vector<Vector3<pfd>> _bindposenormals; //bindpose normals
	std::vector<Vector3<pfd>> _tangents;
	std::vector<Vector3<pfd>> _bitangents;
	std::vector<Vector4<pfd>> _boneweights;
	std::vector<Vector4<pfd>> _boneindices;
	
	std::vector<Weight> _weights;
	std::vector<VertexWeight> _vertexWeights;
protected:
	std::string _name;
};

#endif