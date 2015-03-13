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
#include "GLAnimatedObject.h"
#include "OpenGLContext.h"
#include "SceneManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "TextureManager.h"

#include "Datafile.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <vector>
#include "minutils.h"

GLAnimatedObject::GLAnimatedObject(OpenGLContext& context) : GLObject(context)
{
	_vbBoneWeightId = 0;
	_vbBoneIndexId = 0;
	_isVbBoneWeight = false;
	_isVbBoneIndex = false;
}

GLAnimatedObject::~GLAnimatedObject()
{
	if (_isVbBoneWeight)
	{
		size_t bwl = _glLocManager->getVLocation(glLocationManager::BONEWEIGHT_LOCATION);
		glDisableVertexAttribArray(bwl);
	}
	if (_isVbBoneIndex)
	{
		size_t bil = _glLocManager->getVLocation(glLocationManager::BONEINDEX_LOCATION);
		glDisableVertexAttribArray(bil);
	}
	if (_isVbBoneWeight)
		glDeleteBuffers(1, &_vbBoneWeightId);
	if (_isVbBoneIndex)
		glDeleteBuffers(1, &_vbBoneIndexId);
}

 void GLAnimatedObject::createObject(GLenum renderType)
 {
	 GLObject::createObject(renderType);
	 // add animated parts
	 GLenum ErrorCheckValue = glGetError();
	 assert(ErrorCheckValue == 0);

	 glBindVertexArray(_vaoId);
	 const size_t boneWeightSize = _mesh->_boneweights.size();
	 if(boneWeightSize > 0)
	 {
		_isVbBoneWeight = true;
		const Vector4<pfd>* weights = &_mesh->_boneweights[0];
		glGenBuffers(1, &_vbBoneWeightId);
		glBindBuffer(GL_ARRAY_BUFFER, _vbBoneWeightId);
		glBufferData(GL_ARRAY_BUFFER, 4 * boneWeightSize * sizeof(pfd), weights, GL_STATIC_DRAW);
		size_t bwl = _glLocManager->requestVLocation(glLocationManager::BONEWEIGHT_LOCATION);
		glVertexAttribPointer(bwl, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(bwl);
	 }

	 const size_t boneIndexSize = _mesh->_boneindices.size();
	 if(boneIndexSize > 0)
	 {
		_isVbBoneIndex = true;
		const Vector4<pfd>* indices = &_mesh->_boneindices[0];
		glGenBuffers(1, &_vbBoneIndexId);
		glBindBuffer(GL_ARRAY_BUFFER, _vbBoneIndexId);
		glBufferData(GL_ARRAY_BUFFER, 4 * boneIndexSize * sizeof(pfd), indices, GL_STATIC_DRAW);
		size_t bil = _glLocManager->requestVLocation(glLocationManager::BONEINDEX_LOCATION);
		glVertexAttribPointer(bil, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(bil);
	 }

	glBindVertexArray(0);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not create animated object.");
 }

 GLObject** GLAnimatedObject::loadModel(OpenGLContext& glContext, MD5Model& model, size_t& n)
 {
	 if(model.isLoaded())
	 {
		 const size_t meshCount = model.getMeshCount();
		 GLObject** glAnimObjArray = new GLObject*[meshCount];
		 n = meshCount;
		 for(size_t i=0; i < meshCount; i++)
		 {
			 glAnimObjArray[i] = new GLAnimatedObject(glContext);
			 glContext.smgr->addGLObject(glAnimObjArray[i]);
			 Mesh& mesh = model.getMesh(i);
			 glAnimObjArray[i]->setMesh(model.getMesh(i));
		 }
		 return glAnimObjArray;
	 }
	 else
		 return 0;
 }