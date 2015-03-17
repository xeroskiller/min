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
#include "Geometry.h"
#include "GLObject.h"
#include "PhysicsObject.h"
#include "Material.h"
#include "SceneManager.h"

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

GLObject* Geometry::CreateCube(SceneManager* smgr, Vector4<pfd> color[])
{
	Vector4<pfd>* v4fun = 0;
	Material* mat = 0;
	if(color == 0)
	{
		pfd white[] = 
		{ 
		    1.0f,  1.0f,  1.0f, 1.0f,
		    1.0f,  1.0f,  1.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 1.0f,

		    1.0f,  1.0f,  1.0f, 1.0f,
		    1.0f,  1.0f,  1.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};
		v4fun = new Vector4<pfd>[8];
		memcpy(v4fun, white, 32*sizeof(pfd));
		mat = new Material(v4fun, 8);
	}
	else
	{
		mat = new Material(color, 8);
	}
	
	pfd Vertices[] = 
	{ 
	   -1.0f, -1.0f,  1.0f, 1.0f, //minxy-maxz
	   -1.0f,  1.0f,  1.0f, 1.0f, //minx-maxyz
		1.0f,  1.0f,  1.0f, 1.0f, //maxxyz
		1.0f, -1.0f,  1.0f, 1.0f, //miny-maxxz

	   -1.0f, -1.0f, -1.0f, 1.0f, //minxyz
	   -1.0f,  1.0f, -1.0f, 1.0f, //minxz-maxy
		1.0f,  1.0f, -1.0f, 1.0f, //minz-maxxy
		1.0f, -1.0f, -1.0f, 1.0f  //minyz-maxx
	};
	GLIndex Index[] = 
	{
		0,2,1,
		0,3,2,
		4,3,0,
		4,7,3,
		4,1,5,
		4,0,1,
		3,6,2,
		3,7,6,
		1,6,5,
		1,2,6,
		7,5,6,
		7,4,5
	};

	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();
	mesh->_vertices.resize(8);
	memcpy(&mesh->_vertices[0], Vertices, 8 * sizeof(Vector4<pfd>));
	mesh->_indices.resize(36);
	memcpy(&mesh->_indices[0], Index, 36 * sizeof(GLIndex));

	obj->setMaterial(*mat);
	return obj;
}

GLObject* Geometry::CreateBox(SceneManager* smgr, const aabbox3<pfd>& box, Vector4<pfd> color[])
{
	Vector4<pfd>* v4fun = 0;
	Material* mat = 0;
	if (color == 0)
	{
		pfd white[] =
		{
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};
		v4fun = new Vector4<pfd>[8];
		memcpy(v4fun, white, 32 * sizeof(pfd));
		mat = new Material(v4fun, 8);
	}
	else
	{
		mat = new Material(color, 8);
	}
	Vector3<pfd> min = box.getMin();
	Vector3<pfd> max = box.getMax();

	pfd Vertices[] =
	{
		min.X(), min.Y(), max.Z(), 1.0f, //minxy-maxz
		min.X(), max.Y(), max.Z(), 1.0f, //minx-maxyz
		max.X(), max.Y(), max.Z(), 1.0f, //maxxyz
		max.X(), min.Y(), max.Z(), 1.0f, //miny-maxxz

		min.X(), min.Y(), min.Z(), 1.0f, //minxyz
		min.X(), max.Y(), min.Z(), 1.0f, //minxz-maxy
		max.X(), max.Y(), min.Z(), 1.0f, //minz-maxxy
		max.X(), min.X(), min.Z(), 1.0f  //minyz-maxx
	};
	GLIndex Index[] =
	{
		0, 2, 1,
		0, 3, 2,
		4, 3, 0,
		4, 7, 3,
		4, 1, 5,
		4, 0, 1,
		3, 6, 2,
		3, 7, 6,
		1, 6, 5,
		1, 2, 6,
		7, 5, 6,
		7, 4, 5
	};

	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();
	mesh->_vertices.resize(8);
	memcpy(&mesh->_vertices[0], Vertices, 8 * sizeof(Vector4<pfd>));
	mesh->_indices.resize(36);
	memcpy(&mesh->_indices[0], Index, 36 * sizeof(GLIndex));

	obj->setMaterial(*mat);
	return obj;
}

GLObject* Geometry::CreateBoxOutline(SceneManager* smgr, const aabbox3<pfd>& box, Vector4<pfd> color[])
{
	Vector4<pfd>* v4fun = 0;
	Material* mat = 0;
	if (color == 0)
	{
		pfd white[] =
		{
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f
		};
		v4fun = new Vector4<pfd>[8];
		memcpy(v4fun, white, 32 * sizeof(pfd));
		mat = new Material(v4fun, 8);
	}
	else
	{
		mat = new Material(color, 8);
	}
	Vector3<pfd> min = box.getMin();
	Vector3<pfd> max = box.getMax();

	pfd Vertices[] =
	{
		min.X(), min.Y(), max.Z(), 1.0f, //5               1 ------- 2        y+ z+ 
		min.X(), max.Y(), max.Z(), 1.0f, //1              -         -         | / 
		max.X(), max.Y(), max.Z(), 1.0f, //2             -         -          |/___x+
		max.X(), min.Y(), max.Z(), 1.0f, //6           0 ------- 3

		min.X(), min.Y(), min.Z(), 1.0f, //4              5 ------- 6
		min.X(), max.Y(), min.Z(), 1.0f, //0            -         -
		max.X(), max.Y(), min.Z(), 1.0f, //3            -         -
		max.X(), min.Y(), min.Z(), 1.0f  //7          4 ------- 7
	};
	GLIndex Index[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0, //top
		4, 5, 5, 6, 6, 7, 7, 4, //bottom
		4, 0, 5, 1, 6, 2, 7, 3
	};

	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();
	mesh->_vertices.resize(8);
	memcpy(&mesh->_vertices[0], Vertices, 8 * sizeof(Vector4<pfd>));
	mesh->_indices.resize(36);
	memcpy(&mesh->_indices[0], Index, 24 * sizeof(GLIndex));

	obj->setMaterial(*mat);
	return obj;
}

GLObject* Geometry::CreateBoxes(SceneManager* smgr, std::vector<aabbox3<pfd>>& boxes, Vector4<pfd> color[])
{
	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();

	size_t n = boxes.size();
	Vector4<pfd>* white = new Vector4<pfd>[8*n];
	
	for (size_t i = 0; i < n; i++)
	{
		if (color == 0)
		{
			white[i]   = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+1] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+2] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+3] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);

			white[i+4] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+5] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+6] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
			white[i+7] = Vector4<pfd>(1.0f, 1.0f, 1.0f, 1.0f);
		}
		Vector3<pfd> min = boxes[i].getMin();
		Vector3<pfd> max = boxes[i].getMax();

		mesh->_vertices.push_back(Vector4<pfd>(min.X(), min.Y(), max.Z(), 1.0f)); //5               1 ------- 2        y+ z+ 
		mesh->_vertices.push_back(Vector4<pfd>(min.X(), max.Y(), max.Z(), 1.0f)); //1              -         -         | / 
		mesh->_vertices.push_back(Vector4<pfd>(max.X(), max.Y(), max.Z(), 1.0f)); //2             -         -          |/___x+
		mesh->_vertices.push_back(Vector4<pfd>(max.X(), min.Y(), max.Z(), 1.0f)); //6           0 ------- 3

		mesh->_vertices.push_back(Vector4<pfd>(min.X(), min.Y(), min.Z(), 1.0f)); //4              5 ------- 6
		mesh->_vertices.push_back(Vector4<pfd>(min.X(), max.Y(), min.Z(), 1.0f)); //0            -         -
		mesh->_vertices.push_back(Vector4<pfd>(max.X(), max.Y(), min.Z(), 1.0f)); //3            -         -
		mesh->_vertices.push_back(Vector4<pfd>(max.X(), min.Y(), min.Z(), 1.0f)); //7          4 ------- 7
		
		//top
		mesh->_indices.push_back(8*i);
		mesh->_indices.push_back(8*i + 1);
		mesh->_indices.push_back(8*i + 1);
		mesh->_indices.push_back(8*i + 2);
		mesh->_indices.push_back(8*i + 2);
		mesh->_indices.push_back(8*i + 3);
		mesh->_indices.push_back(8*i + 3);
		mesh->_indices.push_back(8*i);
		//bottom
		mesh->_indices.push_back(8*i + 4);
		mesh->_indices.push_back(8*i + 5);
		mesh->_indices.push_back(8*i + 5);
		mesh->_indices.push_back(8*i + 6);
		mesh->_indices.push_back(8*i + 6);
		mesh->_indices.push_back(8*i + 7);
		mesh->_indices.push_back(8*i + 7);
		mesh->_indices.push_back(8*i + 4);
		//columns
		mesh->_indices.push_back(8*i + 4);
		mesh->_indices.push_back(8*i);
		mesh->_indices.push_back(8*i + 5);
		mesh->_indices.push_back(8*i + 1);
		mesh->_indices.push_back(8*i + 6);
		mesh->_indices.push_back(8*i + 2);
		mesh->_indices.push_back(8*i + 7);
		mesh->_indices.push_back(8*i + 3);
	}
	Material* mat = 0;
	if (color == 0)
	{
		mat = new Material(white, 8*n);
	}
	else
	{
		mat = new Material(color, 8*n);
	}
	obj->setMaterial(*mat);
	return obj;
}

GLObject* Geometry::CreateTriangularPrism(SceneManager* smgr, Vector4<pfd> color[])
{
	Vector4<pfd>* v4fun = 0;
	Material* mat = 0;
	if(color == 0)
	{
		pfd white[] = 
		{ 
		    1.0f,  1.0f,  1.0f, 1.0f, //3
			1.0f,  1.0f,  1.0f, 1.0f, //2
		    1.0f,  1.0f,  1.0f, 1.0f, //1

			1.0f,  1.0f,  1.0f, 1.0f, //0
			1.0f,  1.0f,  1.0f, 1.0f, //5
		    1.0f,  1.0f,  1.0f, 1.0f //4
		};
		v4fun = new Vector4<pfd>[6];
		memcpy(v4fun, white, 24*sizeof(pfd));
		mat = new Material(v4fun, 6);
	}
	else
	{
		mat = new Material(color, 6);
	}
	pfd Vertices[] = 
	{ 
	    0.0f, -1.0f, -1.0f, 1.0f, //3
	    1.0f, -1.0f,  1.0f, 1.0f, //2
	   -1.0f, -1.0f,  1.0f, 1.0f, //1

		0.0f,  1.0f, -1.0f, 1.0f, //0
		1.0f,  1.0f,  1.0f, 1.0f, //5
	   -1.0f,  1.0f,  1.0f, 1.0f //4
	};
	
	GLIndex Index[] = 
	{
		0,3,4,
		0,4,1,
		1,4,5,
		1,5,2,
		2,5,3,
		2,3,0,
		0,1,2,
		3,5,4
	};
	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();
	mesh->_vertices.resize(6);
	memcpy(&mesh->_vertices[0], Vertices, 6 * sizeof(Vector4<pfd>));
	mesh->_indices.resize(24);
	memcpy(&mesh->_indices[0], Index, 24 * sizeof(GLIndex));

	obj->setMaterial(*mat);
	return obj;
}

GLObject* Geometry::CreateRightTriangularPrism(SceneManager* smgr, Vector4<pfd> color[])
{
	Vector4<pfd>* v4fun = 0;
	Material* mat = 0;
	if(color == 0)
	{
		pfd white[] = 
		{ 
		    1.0f,  1.0f,  1.0f, 1.0f, //3
			1.0f,  1.0f,  1.0f, 1.0f, //2
		    1.0f,  1.0f,  1.0f, 1.0f, //1

		    1.0f,  1.0f,  1.0f, 1.0f, //0
			1.0f,  1.0f,  1.0f, 1.0f, //5
		    1.0f,  1.0f,  1.0f, 1.0f //4
		};
		v4fun = new Vector4<pfd>[6];
		memcpy(v4fun, white, 24*sizeof(pfd));
		mat = new Material(v4fun, 6);
	}
	else
	{
		mat = new Material(color, 6);
	}
	pfd Vertices[] = 
	{ 
	   -1.0f, -1.0f, -1.0f, 1.0f, //3
	    1.0f, -1.0f,  1.0f, 1.0f, //2
	   -1.0f, -1.0f,  1.0f, 1.0f, //1

	   -1.0f,  1.0f, -1.0f, 1.0f, //0
		1.0f,  1.0f,  1.0f, 1.0f, //5
	   -1.0f,  1.0f,  1.0f, 1.0f //4
	};
	
	GLIndex Index[] = 
	{
		0,3,4,
		0,4,1,
		1,4,5,
		1,5,2,
		2,5,3,
		2,3,0,
		0,1,2,
		3,5,4
	};

	GLObject* obj = smgr->addGLObject();
	Mesh* mesh = obj->getMesh();
	mesh->_vertices.resize(6);
	memcpy(&mesh->_vertices[0], Vertices, 6 * sizeof(Vector4<pfd>));
	mesh->_indices.resize(24);
	memcpy(&mesh->_indices[0], Index, 24 * sizeof(GLIndex));

	obj->setMaterial(*mat);
	return obj;
}