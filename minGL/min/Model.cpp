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
#include "stdafx.h"
#include "Model.h"


Model::Model()
{
	_numMeshes = 0;
	_loaded = false;
}


Model::~Model()
{
	std::vector<Mesh*>::iterator k = _meshList.begin();
	std::vector<Mesh*>::iterator end = _meshList.begin();
	for(; k != end; )
	{
		delete (*k);
		k = _meshList.erase(k);
	}
}

void Model::CalculateNormals()
{
	GLIndex ia,ib,ic;
	const size_t size = _meshList.size();
	for(size_t i=0; i < size; i++)
	{
		std::vector<Vector3<pfd>>& norm = _meshList[i]->_normals;
		std::vector<Vector4<pfd>>& vert = _meshList[i]->_vertices;
		std::vector<GLIndex>& element = _meshList[i]->_indices;

		norm.resize(vert.size(), Vector3<pfd>(0,0,0));
		for (size_t j=0; j < element.size(); j+= 3)
		{
			//calculate tangents and bitangents
			ia = element[j];
			ib = element[j + 1];
			ic = element[j + 2];
			Vector3<pfd> v0 = vert[ia];
			Vector3<pfd> v1 = vert[ib];
			Vector3<pfd> v2 = vert[ic];

			Vector3<pfd> n = (v2-v0).crossProduct(v1-v0); //negate for CCW

			norm[ia] += n;
			norm[ib] += n;
			norm[ic] += n;
		}
		std::vector<Vector3<pfd>>::iterator k = norm.begin();
		for(; k!= norm.end(); k++)
		{
			(*k).normalize();
		}
	}
}

void Model::CalculateTangents()
{
	for(size_t i=0; i < _meshList.size(); i++)
	{
		const size_t elementSize = _meshList[i]->_indices.size();

		for(size_t j=0; j < elementSize; j+=3)
		{
			std::vector<Vector3<pfd>>& norm = _meshList[i]->_normals;
			std::vector<Vector3<pfd>>& tangents = _meshList[i]->_tangents;
			std::vector<Vector3<pfd>>& bitangents = _meshList[i]->_bitangents;
			std::vector<Vector4<pfd>>& vert = _meshList[i]->_vertices;
			std::vector<Vector2<pfd>>& uv = _meshList[i]->_uvcoords;
			std::vector<GLIndex>& element = _meshList[i]->_indices;

			//calculate tangents and bitangents
			Vector3<pfd> v0 = vert[element[j]];
			Vector3<pfd> v1 = vert[element[j + 1]];
			Vector3<pfd> v2 = vert[element[j + 2]];

			Vector3<pfd> n = (v2-v0).crossProduct(v1-v0); //negate for CCW

			Vector2<pfd> uv0 = uv[element[j]];
			Vector2<pfd> uv1 = uv[element[j + 1]];
			Vector2<pfd> uv2 = uv[element[j + 2]];

			Vector3<pfd> deltaPos1 = v1-v0;
			Vector3<pfd> deltaPos2 = v2-v0;

			Vector2<pfd> deltaUV1 = uv1-uv0;
			Vector2<pfd> deltaUV2 = uv2-uv0;

			float r = 1.0f / (deltaUV1.X() * deltaUV2.Y() - deltaUV1.Y() * deltaUV2.X());
			Vector3<pfd> tangent = (deltaPos1 * deltaUV2.Y() - deltaPos2 * deltaUV1.Y())*r;
			Vector3<pfd> bitangent = (deltaPos2 * deltaUV1.X() - deltaPos1 * deltaUV2.X())*r;

			tangent = (tangent - norm[element.back()] * norm[element.back()].dotProduct(tangent));
			tangent.normalize();

			bitangent.normalize();

			if((n.crossProduct(tangent)).dotProduct(bitangent) < (pfd)0.0)
			{
				tangent = tangent * (pfd)-1.0;
			}

			tangents.push_back(tangent);
			tangents.push_back(tangent);
			tangents.push_back(tangent);
 
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
			bitangents.push_back(bitangent);
		}
	}
}