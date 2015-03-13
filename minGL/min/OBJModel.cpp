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
#include "OBJModel.h"
#include "GLObject.h"
#include <fstream>
#include <sstream>
#include <vector>

OBJModel::OBJModel()
{

}


OBJModel::~OBJModel()
{

}

bool OBJModel::loadModel(const std::string& absFilePath, bool invert)
{
	 std::vector<Vector4<pfd>> vert;
	 std::vector<Vector3<pfd>> norm;
	 std::vector<GLIndex> element;
	 std::vector<Vector2<pfd>> uv;
	
	 size_t count = 0;
	 size_t i = 0;
	 bool first = true;
	 pfd x,y,z;
	 size_t face_mode = 0;
	 std::ifstream file (absFilePath, std::ios::in);
	 if(file.is_open())
	 {
		std::string line;
		while(std::getline(file, line))
		{
			if(line.substr(0, 1) == "o")
			{
				count++;
			}
		}

		if(count == 0)
			return false;

		file.clear();
		file.seekg(0, std::ios_base::beg);
		_meshList.resize(count);

		Mesh* mesh = 0;

		while(std::getline(file, line))
		{
			if(line.substr(0, 2) == "v ")
			{
				std::istringstream s(line.substr(2));
				s >> x;
				s >> y;
				s >> z;
				Vector4<pfd> vec4(x,y,z,1.0);
				vert.push_back(vec4);
			}
			else if(line.substr(0,2) == "vt")
			{
				std::istringstream s(line.substr(2));
				pfd u,v;
				s >> u;
				s >> v;
				Vector2<pfd> vec2;
				if(!invert)
				{
					vec2 = Vector2<pfd> (u,v);
				}
				else
				{
					vec2 = Vector2<pfd> (u,(pfd)1.0 - v);
				}
				uv.push_back(vec2);
			}
			else if(line.substr(0,2) == "vn")
			{
				std::istringstream s(line.substr(2));
				s >> x;
				s >> y;
				s >> z;
				Vector3<pfd> vec3(x,y,z);
				norm.push_back(vec3);
			}
			else if(line.substr(0,1) == "f")
			{
				line = line.substr(2);
				std::vector<GLIndex> a;
				std::string each;
				for(size_t i=0; i<line.size(); i++)
				{
					if(line[i] == '/' || line[i] == ' ')
					{
						if(each.compare("") != 0)
						{
							a.push_back(static_cast<GLIndex>(std::stoi(each)));
							each.clear();
						}
					}
					else
					{
						each += line[i];
					}
				}
				a.push_back(static_cast<GLIndex>(std::stoi(each)));
				face_mode = a.size(); //should be const
				for(size_t i=0; i<face_mode;i++)
				{
					a[i]--;
					element.push_back(a[i]);
				}
			}
			else if(line.substr(0,1) == "o")
			{
				mesh = new Mesh;
				if(!first)
				{
					ProcessData(*mesh, face_mode, vert, norm, element, uv);
					_meshList[i] = mesh;
					i++;
					count--;
					_numMeshes++;

					vert.clear();
					norm.clear();
					element.clear();
					uv.clear();

					if(count == 0)
					{
						_loaded = true;
						return true;
					}
				}
				first = false;
			}
			else {}
		}
		ProcessData(*mesh, face_mode, vert, norm, element, uv);
		_meshList[i] = mesh;
		i++;
		count--;
		_numMeshes++;

		vert.clear();
		norm.clear();
		element.clear();
		uv.clear();

		if(count == 0)
		{
			_loaded = true;
			return true;
		}
	 }
	 return false;
}

void OBJModel::ProcessData(Mesh& mesh, size_t face_mode, std::vector<Vector4<pfd>>& vert, std::vector<Vector3<pfd>>& norm, std::vector<GLIndex>& element, std::vector<Vector2<pfd>>& uv)
{
	if(vert.size() > 0)
	{
		std::vector<Vector4<pfd>>& vert_out = mesh._vertices;
		std::vector<Vector3<pfd>>& nor_out = mesh._normals;
		std::vector<Vector2<pfd>>& uv_out= mesh._uvcoords;
		std::vector<GLIndex>& element_out= mesh._indices;
		switch(face_mode)
		{
			case 3:
			{			
				mesh._vertices = vert;
				mesh._indices = element;
				break;
			}
			case 6:
			{
				bool isUV = false;
				bool isNorm = false;
				if(uv.size() > 0)
				{
					isUV = true;
				}
				if(norm.size() > 0)
				{
					isNorm = true;
				}
				std::vector<GLIndex>::iterator k = element.begin();
				std::vector<std::vector<GLIndex>> map;
				std::vector<std::vector<GLIndex>>::iterator m = map.begin();

				bool fresh = true;
				size_t index = 0;
				for(; k!= element.end(); k += 2)
				{
					fresh = true;
					for(m = map.begin(); m!= map.end(); m++)
					{
						if(*k == (*m)[0] && *(k+1) == (*m)[1])
						{
							fresh = false;
							break;
						}
					}
					if(fresh)
					{
						std::vector<GLIndex> arr;
						arr.push_back(*k);
						arr.push_back(*(k+1));
						map.push_back(arr);
						vert_out.push_back(vert[*k]);
						if(isUV)
							uv_out.push_back(uv[*(k+1)]);
						if(isNorm)
							nor_out.push_back(norm[*(k+1)]);
						element_out.push_back(index);
						index++;
					}
					else
					{
						element_out.push_back(m - map.begin());
					}
				}
				break;
			}
			case 9:
			{
				std::vector<GLIndex>::iterator k = element.begin();
				std::vector<std::vector<GLIndex>> map;
				std::vector<std::vector<GLIndex>>::iterator m = map.begin();

				bool fresh = true;
				size_t index = 0;
				for(; k!= element.end(); k += 3)
				{
					fresh = true;
					for(m = map.begin(); m!= map.begin(); m++)
					{
						if(*k == (*m)[0] && *(k+1) == (*m)[1] && *(k+2) == (*m)[2])
						{
							fresh = false;
							break;
						}
					}
					if(fresh)
					{
						std::vector<GLIndex> arr;
						arr.push_back(*k);
						arr.push_back(*(k+1));
						arr.push_back(*(k+2));
						map.push_back(arr);
						vert_out.push_back(vert[*k]);
						uv_out.push_back(uv[*(k+1)]);
						nor_out.push_back(norm[*(k+2)]);
						element_out.push_back(index);
						index++;
					}
					else
					{
						element_out.push_back(m - map.begin());
					}
				}
				break;
			}
			default:
			{
				//NOTHING
				break;
			}
		}
	}
}