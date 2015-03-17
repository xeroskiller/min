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

//tree node class
template <class T, class K> class OctreeNode
{
	template <class T, class K> friend class Octree;
	OctreeNode()
	{
		isLeaf = false;
		for(size_t i=0; i<8; i++)
		{
			_child[i] = 0;
		}
	}
	//center of node
	Vector3<K> _center;
	//halfwidth of node
	Vector3<K> _halfWidth;
	//children node list
	OctreeNode<T, K>* _child[8];
	//list of vertices contained in node
	std::vector<Vertex<T>*> _vertexNodeList;
	bool isLeaf;
};

template <class T, class K> class Octree
{
public:
	typedef void (*OctreeCallback)(void * p); 
	//init's data and copies the vertex pointers out of triangle buffer
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
	//destroys octree
	Octree::~Octree()
	{
		Octree::Delete(_root);
	}
	//creates the tree structure: tree structure will not rotate since it is an AABB-fix this
	void createOctree(const Vector3<K>& center, const Vector3<K>& extent)
	{
		_root = new OctreeNode<T, K>;
		_root->_center = center;
		_root->_halfWidth = extent * static_cast<K>(0.5);

		buildOctree(_root, &_vertexList, _depth);
	}
	//return list of OctreeNodes that interesect frustum
	std::vector<OctreeNode<T, K>*> testFrustumIntersection(OctreeNode<T, K>* node, const Vector3<K>& worldPos, Frustum* frustum)
	{
		if(node == 0)
		{
			return testFrustumIntersection(_root, worldPos, frustum);
		}

		std::vector<OctreeNode<T, K>*> list;
		
		if (frustum == 0)
			return list;

		for(size_t i=0; i < 8; i++)
		{
			std::vector<OctreeNode<T, K>*> childList;
			if(node->_child[i])
			{
				childList = testFrustumIntersection(node->_child[i], worldPos, frustum);
				list.insert(list.end(), childList.begin(), childList.end());
			}
		}
		if (node->isLeaf)
		{
			Vector3<K> min = node->_center - node->_halfWidth + worldPos;
			Vector3<K> max = node->_center + node->_halfWidth + worldPos;
			if (frustum->isAABBInFrustum(aabbox3<K>(min, max)) && node->_vertexNodeList.size() > 0)
			{
				list.push_back(node);
			}
 		}
		return list;
	}
	//this function performs getList on a nodeList and then returns the triangles in sorted order: the original order provided to the Octree on Octree::createOctree() in a hash function you need to prune out the missing triangle by check the pointing against null
	void sortedGetList(std::vector<OctreeNode<T, K>*>* nodeList, std::vector<Triangle<T>*>& _stack)
	{
		_stack = std::vector<Triangle<T>*>(_vertexList.size() / 3, 0);
		std::vector<Vertex<T>*> list;
		for (size_t i = 0; i < nodeList->size();i++)
		{
			if ((*nodeList)[i] != 0)
			{
				list = getList((*nodeList)[i]);
			}
			else
			{
				return;
			}

			std::vector<Vertex<T>*>::iterator k = list.begin();
			std::vector<Vertex<T>*>::iterator end = list.end();

			//this sorts the triangles using a simple hash function; id is in multiples of 3: see GLObject::createObject()

			for (; k != end; k++)
			{
				Triangle<T>* triangle = (*k)->getTriangle();
				T id = triangle->getId();
				if (_stack[id / 3] == 0)
				{
					_stack[id / 3] = triangle;
				}
			}
		}
	}
	//Looks up all the triangle pointers and sorts them into the original index order and returns that set of indices see GLObject::createObject(); original indices may have been deleted with release(); used in Min() file format
	std::vector<T> saveOctree() 
	{
		std::vector<Vertex<T>*> list = getList(_root);

		std::vector<Vertex<T>*>::iterator k = list.begin();
		std::vector<Vertex<T>*>::iterator end = list.end();

		//this sorts the triangles using a simple hash function; id is in multiples of 3: see GLObject::createObject()
		std::vector<Triangle<T>*> _stack(_vertexList.size()/3, 0);
		for(; k!= end; k++)
		{
			Triangle<T>* triangle = (*k)->getTriangle();
			size_t id = triangle->getId();
			if(_stack[id/3] == 0)
			{
				_stack[id/3] = triangle;
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
	std::vector<aabbox3<K>> getBoxes(OctreeNode<T, K>* node)
	{
		if (node == 0)
		{
			return getBoxes(_root);
		}

		std::vector<aabbox3<K>> list;

		for (size_t i = 0; i < 8; i++)
		{
			std::vector<aabbox3<K>> childList;
			if (node->_child[i])
			{
				childList = getBoxes(node->_child[i]);
				list.insert(list.end(), childList.begin(), childList.end());
			}
		}
		if (node->isLeaf)
		{
			Vector3<K> min = node->_center - node->_halfWidth;
			Vector3<K> max = node->_center + node->_halfWidth;
			aabbox3<K> box(min, max);
			list.push_back(box);
		}
		return list;
	}
private:
	//Returns a list of Vertex pointers contained in the leafnodes of a target node
	std::vector<Vertex<T>*> getList(OctreeNode<T, K>* node)
	{
		std::vector<Vertex<T>*> list;
		
		for(size_t i=0; i < 8; i++)
		{
			std::vector<Vertex<T>*> childList;
			if(node->_child[i])
			{
				childList = getList(node->_child[i]);
				list.insert(list.end(), childList.begin(), childList.end());
			}
		}
		if (node->isLeaf)
		{
			std::vector<Vertex<T>*>::iterator k = node->_vertexNodeList.begin();
			std::vector<Vertex<T>*>::iterator end = node->_vertexNodeList.end();
			for (; k != end; k++)
			{
				list.push_back(*k);
			}
		}
		return list;
	}
	//Recursively builds the octree via top down architechure
	void Octree::buildOctree(OctreeNode<T, K>* node, std::vector<Vertex<T>*>* vertexBuffer, size_t currentDepth)
	{
		if(currentDepth == -1)
		{
			node->isLeaf = true;
			std::vector<Vertex<T>*>::iterator k = vertexBuffer->begin();
			for(; k!= vertexBuffer->end(); k++)
			{
				node->_vertexNodeList.push_back(*k);
			}
			return;
		}

		std::vector<Vertex<T>*> children[8];

		const Vector3<K>& c = node->_center;

		std::vector<Vertex<T>*>::iterator k = vertexBuffer->begin();
		for(; k!= vertexBuffer->end(); k++)
		{
			Vector4<K>& p = (*_pointBuffer)[(*k)->getIndex()];

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
	//Recursively deletes the octree nodes
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
	//depth of tree
	size_t _depth;
	//root node pointer
	OctreeNode<T, K>* _root;
	//list of Vertex pointers; same order as original input indices
	std::vector<Vertex<T>*> _vertexList;
	//original  mesh vertices pointer
	std::vector<Vector4<K>>* _pointBuffer;
};

#endif