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
#include "DrawManager.h"


DrawManager::DrawManager(Octree<GLIndex, pfd>* octree)
{
	_octree = octree;
}


DrawManager::~DrawManager()
{

}

//this is expensive
void DrawManager::update(std::vector<OctreeNode<GLIndex, pfd>*>* nodeList)
{
	std::vector<Triangle<GLIndex>*> triangleList;
	_octree->sortedGetList(nodeList, triangleList);
	
	_start.clear();
	_drawCount = 0;
	bool test = false;
	GLsizei start = 0;
	GLsizei end = 0;
	const size_t n = triangleList.size();
	for (size_t i = 0; i < n; i++)
	{
 		test = findNext(triangleList, start, end);
		if (test != false)
		{
			GLsizei startIndex = start * 3;
			GLsizei endIndex = end * 3;
			_start.push_back(startIndex);
			_start.push_back(endIndex);
			_drawCount++;
			start = end;
			start++;
 			if (end == n)
			{
				break;
			}
		}
		else
		{
			break; //end of file
		}
	}
}
GLsizei DrawManager::getDrawCount()
{
	return _drawCount;
}
GLsizei DrawManager::getIndexCount(size_t n)
{
	return _start[2*n + 1] - _start[2*n];
}
GLsizei DrawManager::getStart(size_t n)
{
	return _start[2*n];
}
//this function reduces the vector into a series of runs of triangle pointers and null values: information is stored in _start
bool DrawManager::findNext(std::vector<Triangle<GLIndex>*> list, GLsizei& start, GLsizei& end)
{
	bool nobreak = true;
	//find run of drawables
	for (size_t i = start; i < list.size(); i++)
	{
		if (list[i] != 0)
		{
			start = i;
			nobreak = false;
			break;
		}
	}
	if (nobreak)
	{
		return false;
	}
	//find run of nulls
	for (size_t i = start; i < list.size(); i++)
	{
		if (list[i] == 0)
		{
			end = i;
			return  true;
		}
	}
	end = list.size();
	return true;
}