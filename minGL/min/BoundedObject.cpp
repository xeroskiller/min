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
#include "BoundedObject.h"
#include "GLObject.h"
#include "bsphere.h"
#include "aabbox3.h"
#include "oobbox3.h"
#include "Vector4.h"

BoundedObject::BoundedObject(OpenGLContext& context) : Object(context)
{
	_boundingVolume = 0;
}

BoundedObject::~BoundedObject()
{
	if(_boundingVolume)
		delete _boundingVolume;
}

void BoundedObject::setBoundingVolume(BoundingVolume<pfd>::BoundingVolumeEnum type)
{
	_type = type;
}
//must call setBoundingVolume before creation
void BoundedObject::createObject(std::vector<GLObject*>& glObjects)
{
	size_t n = glObjects.size();
	if(n > 0)
	{
		this->Object::createObject(glObjects);
	
		switch (_type)
		{
			case BoundingVolume<pfd>::SPHERE:
			{
				Mesh* mesh = getGLObject(0).getMesh();
				size_t vertSize = mesh->_vertices.size();
				bsphere<pfd> sphere((Vector4<pfd>*)(&mesh->_vertices[0]), vertSize);

				for(size_t i=1; i<n; i++)
				{
					mesh = getGLObject(i).getMesh();
					vertSize = mesh->_vertices.size();
					sphere.add((Vector4<pfd>*)(&mesh->_vertices[0]), vertSize);
				}
				_boundingVolume = new bsphere<pfd>(sphere.getCenter(), sphere.getRadius());
				break;
			}
			case BoundingVolume<pfd>::AABB:
			{
				Mesh* mesh = getGLObject(0).getMesh();
				size_t vertSize = mesh->_vertices.size();
				aabbox3<pfd> box(&mesh->_vertices[0], vertSize);
				for(size_t i=1; i<n; i++)
				{
					mesh = getGLObject(i).getMesh();
					vertSize = mesh->_vertices.size();
					box.add(&mesh->_vertices[0], vertSize);
				}
				_boundingVolume = new aabbox3<pfd>(box.getMin(), box.getMax());
				break;
			}
			case BoundingVolume<pfd>::OOBB:
			{
				Mesh* mesh = getGLObject(0).getMesh();
				size_t vertSize = mesh->_vertices.size();
				aabbox3<pfd> box(&mesh->_vertices[0], vertSize);
				for(size_t i=1; i<n; i++)
				{
					mesh = getGLObject(i).getMesh();
					vertSize = mesh->_vertices.size();
					box.add(&mesh->_vertices[0], vertSize);
				}
				_boundingVolume = new oobbox3<pfd>(box);
				break;
			}
		}
	}
}

bool BoundedObject::intersects(const BoundedObject& A)
{
	return (this->_boundingVolume->intersects(*A._boundingVolume));
}

bool BoundedObject::intersects(const BoundedObject& A, void * vpi, void * vpr)
{
	if(this->_boundingVolume->intersects(*A._boundingVolume))
	{
		Invoke(vpi);
		return true;
	}
	else
	{
		Reset(vpr);
		return false;
	}
}

bool BoundedObject::intersects(const BoundedObject& A, Vector3<pfd>& p)
{
	return (this->_boundingVolume->intersects(*A._boundingVolume, p));
}

bool BoundedObject::intersects(const BoundedObject& A, Vector3<pfd>& p, void * vpi, void * vpr)
{
	if(this->_boundingVolume->intersects(*A._boundingVolume, p))
	{
		Invoke(vpi);
		return true;
	}
	else
	{
		Reset(vpr);
		return false;
	}
}

void BoundedObject::setIntersectionCallBack(void (*Invokef)(void *), void (*Resetf)(void *))
{
	_Invokef = Invokef;
	_Resetf = Resetf;
}

void BoundedObject::Invoke(void* vp)
{
	_Invokef(vp);
}

void BoundedObject::Reset (void*vp)
{
	_Resetf(vp);
}
