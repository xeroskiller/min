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
#ifndef __OCTREE__
#define __OCTREE__

class Frustum;
#include "Frustum.h"

template <class T> class Vertex;
#include "Vertex.h"
template <class T> class Triangle;
#include "Triangle.h"

template <class T> class bsphere;
#include "bsphere.h"
template <class T> class Vector4;
#include "Vector4.h"

#include <list>	
#include <vector>

template <class T, class K> class OctreeNode
{
	template <class T, class K> friend class Octree;
	OctreeNode()
	{
		for(size_t i=0; i<8; i++)
		{
			_child[i] = 0;
		}
	}
	
	Vector3<K> _center;
	Vector3<K> _halfWidth;
	OctreeNode<T, K>* _child[8];
	std::vector<Vertex<T>*> _objList;
};

template <class T, class K> class Octree
{
public:
	typedef void (*OctreeCallback)(void * p); 

	Octree::Octree(std::vector<Vector4<K>>* points, std::vector<Triangle<T>>* triangles, size_t maxDepth)
	{
		std::vector<Triangle<T>>::iterator k = triangles->begin();
		std::vector<Triangle<T>>::iterator end = triangles->end();
		for(; k != end; k++)
		{
			Vertex<T> * vertices = (*k).getVertices();
			_vertexList.push_back(&vertices[0]);
			_vertexList.push_back(&vertices[1]);
			_vertexList.push_back(&vertices[2]);
		}
		_pointBuffer = points;
		_root = 0;
		_depth = maxDepth;
	}
	Octree::~Octree()
	{
		Octree::Delete(_root);
	}
	void createOctree(const Vector3<K>& center, const Vector3<K>& extent)
	{
		_root = new OctreeNode<T, K>;
		_root->_center = center;
		_root->_halfWidth = extent * static_cast<K>(0.5);

		buildOctree(_root, &_vertexList, _depth);
	}
	std::vector<Vertex<T>*> testFrustumIntersection(OctreeNode<T, K>* node, Frustum* frustum)
	{
		if(node == 0)
		{
			return testFrustumIntersection(_root, frustum);
		}

		std::vector<Vertex<T>*> list;
		
		for(size_t i=0; i < 8; i++)
		{
			std::vector<Vertex<T>*> childList;
			if(node->_child[i])
			{
				childList = testFrustumIntersection(node->_child[i], frustum);
			}
			else
			{
				std::vector<Vertex<T>*>::iterator k = node->_objList.begin();
				std::vector<Vertex<T>*>::iterator end = node->_objList.end();
				for(; k!= end; k++)
				{
					if(frustum->isPointInFrustum(_pointBuffer[(*k).getIndex()]))
					{
						list.push_back(*k);
					}
				}
			}
			std::vector<Vertex<T>*>::iterator k = childList.begin();
			for(;k != childList.end(); k++)
			{
				list.push_back(*k);
			}
		}
		return list;
	}
	std::vector<Vertex<T>*> testAABBIntersection(OctreeNode<T, K>* node, const aabbox3<K>& box)
	{
		if(node == 0)
		{
			return testAABBIntersection(_root, frustum);
		}

		std::vector<Vertex<T>*> list;
		
		for(size_t i=0; i < 8; i++)
		{
			std::vector<Vertex<T>*> childList;
			if(node->_child[i])
			{
				childList = testAABBIntersection(node->_child[i], box);
			}
			else
			{
				std::vector<Vertex<T>*>::iterator k = node->_objList.begin();
				std::vector<Vertex<T>*>::iterator end = node->_objList.end();
				for(; k!= end; k++)
				{
					if(box.isPointInside(_pointBuffer[(*k).getIndex()]))
					{
						list.push_back(*k);
					}
				}
			}
			std::vector<Vertex<T>*>::iterator k = childList.begin();
			for(;k != childList.end(); k++)
			{
				list.push_back(*k);
			}
		}
		return list;
	}
	std::vector<Vertex<T>*> testSphereIntersection(OctreeNode<T, K>* node, const bsphere<K>& sp)
	{
		if(node == 0)
		{
			return testSphereIntersection(_root, frustum);
		}

		std::vector<Vertex<T>*> list;
		
		for(size_t i=0; i < 8; i++)
		{
			std::vector<Vertex<T>*> childList;
			if(node->_child[i])
			{
				childList = testSphereIntersection(node->_child[i], sp);
			}
			else
			{
				std::vector<Vertex<T>*>::iterator k = node->_objList.begin();
				std::vector<Vertex<T>*>::iterator end = node->_objList.end();
				for(; k!= end; k++)
				{
					if(sp.isPointInside(_pointBuffer[(*k).getIndex()]))
					{
						list.push_back(*k);
					}
				}
			}
			std::vector<Vertex<T>*>::iterator k = childList.begin();
			for(;k != childList.end(); k++)
			{
				list.push_back(*k);
			}
		}
		return list;
	}
	bsphere<K>* sphereFromQuadrant(size_t n, std::vector<T>* indices)
	{
		OctreeNode<T, K>* node = _root->_child[n];
		std::vector<Vertex<T>*> list;

		if(node != 0)
		{
			list = getList(node);
			if(indices == 0)
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}

		std::vector<Vector4<K>> verts;

		std::vector<Vertex<T>*>::iterator k = list.begin();
		std::vector<Vertex<T>*>::iterator end = list.end();

		std::vector<int> _hash;
		_hash.resize(_vertexList.size());
		
		for(size_t i=0; i<_hash.size(); i++)
		{
			_hash[i] = -1;
		}

		std::vector<Triangle<T>*> _stack;
		
		for(; k!= end; k++)
		{
			Triangle<T>* triangle = (*k)->getTriangle();
			int id = static_cast<int>(triangle->getId());

			if(_hash[id] == -1)
			{
				_stack.push_back(triangle);
				_hash[id] = id;
			}
		}

		std::vector<Triangle<T>*>::iterator l = _stack.begin();
		std::vector<Triangle<T>*>::iterator m = _stack.end();

		for(; l!= m; l++)
		{
			Triangle<T>* triangle = (*l);
			Vertex<T> * vertex = triangle->getVertices();
			T ind0 = vertex[0].getIndex();
			T ind1 = vertex[1].getIndex();
			T ind2 = vertex[2].getIndex();
			indices->push_back(ind0);
			indices->push_back(ind1);
			indices->push_back(ind2);
			verts.push_back((*_pointBuffer)[ind0]);
			verts.push_back((*_pointBuffer)[ind1]);
			verts.push_back((*_pointBuffer)[ind2]);
		}

		if(verts.size() >= 2)
		{
			return new bsphere<K>(&verts[0], verts.size());
		}
		else
		{
			return new bsphere<K>(verts[0], (pfd)1.0);
		}
	}

	std::vector<T> saveOctree()
	{
		std::vector<Vertex<T>*> list = getList(_root);

		std::vector<Vertex<T>*>::iterator k = list.begin();
		std::vector<Vertex<T>*>::iterator end = list.end();

		std::vector<int> _hash;
		_hash.resize(_vertexList.size());
		
		for(size_t i=0; i<_hash.size(); i++)
		{
			_hash[i] = -1;
		}

		std::vector<Triangle<T>*> _stack;
		
		for(; k!= end; k++)
		{
			Triangle<T>* triangle = (*k)->getTriangle();
			int id = static_cast<int>(triangle->getId());

			if(_hash[id] == -1)
			{
				_stack.push_back(triangle);
				_hash[id] = id;
			}
		}

		std::vector<Triangle<T>*>::iterator l = _stack.begin();
		std::vector<Triangle<T>*>::iterator m = _stack.end();

		std::vector<T> indexList;

		for(; l!= m; l++)
		{
			Triangle<T>* triangle = (*l);
			Vertex<T> * vertex = triangle->getVertices();
			indexList.push_back(vertex[0].getIndex());
			indexList.push_back(vertex[1].getIndex());
			indexList.push_back(vertex[2].getIndex());
		}

		return indexList;
	}
private:
	std::vector<Vertex<T>*> getList(OctreeNode<T, K>* node)
	{
		std::vector<Vertex<T>*> list;
		
		for(size_t i=0; i < 8; i++)
		{
			std::vector<Vertex<T>*> childList;
			if(node->_child[i])
			{
				childList = getList(node->_child[i]);
			}
			else
			{
				std::vector<Vertex<T>*>::iterator k = node->_objList.begin();
				std::vector<Vertex<T>*>::iterator end = node->_objList.end();
				for(; k!= end; k++)
				{
					list.push_back(*k);
				}
			}
			std::vector<Vertex<T>*>::iterator k = childList.begin();
			for(;k != childList.end(); k++)
			{
				list.push_back(*k);
			}
		}
		return list;
	}
	void Octree::buildOctree(OctreeNode<T, K>* node, std::vector<Vertex<T>*>* vertexBuffer, size_t currentDepth)
	{
		if(currentDepth == -1)
		{
			std::vector<Vertex<T>*>::iterator k = vertexBuffer->begin();
			for(; k!= vertexBuffer->end(); k++)
			{
				node->_objList.push_back(*k);
			}
			return;
		}

		std::vector<Vertex<T>*> children[8];

		const Vector3<K>& c = node->_center;

		std::vector<Vertex<T>*>::iterator k = vertexBuffer->begin();
		for(; k!= vertexBuffer->end(); k++)
		{
			Vector3<K> p = (*_pointBuffer)[(*k)->getIndex()];

			int index = 0;

			if(p.X() > c.X()) index |= 1;
			if(p.Y() > c.Y()) index |= 2;
			if(p.Z() > c.Z()) index |= 4;

			children[index].push_back(*k);
		}

		for(size_t i=0; i < 8; i++)
		{
			if(children[i].size() <= 0)
				continue;

			Vector3<K> offset;
			Vector3<K> newWidth = node->_halfWidth * static_cast<K>(0.5);

			offset._arr[0] = ((i & 1) ? newWidth.X() : - newWidth.X());
			offset._arr[1] = ((i & 2) ? newWidth.Y() : - newWidth.Y());
			offset._arr[2] = ((i & 4) ? newWidth.Z() : - newWidth.Z());

			node->_child[i] = new OctreeNode<T, K>;
			node->_child[i]->_center = c + offset;
			node->_child[i]->_halfWidth = newWidth;

			buildOctree(node->_child[i], &children[i], currentDepth - 1);
		}
		return;
	}
	void Octree::Delete(OctreeNode<T, K>* node)
	{
		for(size_t i=0; i < 8; i++)
		{
			if(node->_child[i])
			{
				Delete(node->_child[i]);
			}
		}
		delete node;
	}

	size_t _depth;
	OctreeNode<T, K>* _root;
	std::vector<Vertex<T>*> _vertexList;
	std::vector<Vector4<K>>* _pointBuffer;
};

#endif