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
#ifndef __GLOBJECT__
#define __GLOBJECT__
#include "stdafx.h"

template <class T, class K> class Octree;
#include "Octree.h"

class OpenGLContext;
class OBJModel;
#include "OBJModel.h"
class Mesh;
#include "Mesh.h"
class Material;
class glLocationManager;
#include "glLocationManager.h"

template <class T> class aabbox3;
#include "bsphere.h"
#include <string>

class GLObject
{
public:
	friend class SceneManager;
	static const size_t MAX_TEXTURE = 8;
	enum GLObjectEnum: _int32 {VERTEX=0, UV, NORMAL, TANGENT, BITANGENT, INDEX};
	GLObject(OpenGLContext& glContext);
	virtual ~GLObject();
	GLuint getVaoId();
	size_t getDrawCount();
	GLsizei getIndexCount(size_t n);
	GLsizei getVertexCount(size_t n);
	GLsizei getStart(size_t n);
	GLenum getRenderType();
	Mesh* getMesh();
	void setMesh(Mesh& mesh);
	Material* getMaterial();
	void setMaterial(Material& mat);
	virtual void createObject(GLenum renderType);
	bool loadTexture(const std::string& texturePath);
	static GLObject** loadModel(OpenGLContext& glContext, OBJModel& model, size_t& n);
	bool saveYageObj(const std::string& absFilePath, bool overwrite);
	bool loadYageObj(const std::string& absFilePath);
	void scaleUV(pfd scaleFactor);
	bool isIndex();
	bool isTexture(size_t n);
	bool isCreated();
	void enableOctree(bool state, size_t depth);
	glLocationManager* getGLLocManager();
	size_t getSceneIndex();
	void release();
	bsphere<pfd>* getBaseFrustumSphere() { return _frustumSphere[0];}
	bsphere<pfd>* getFrustumSphere(size_t n) { return _frustumSphere[n + 1];}
protected:
	OpenGLContext* _glContext;
	size_t _vertSize;
	size_t _indexSize;
	GLenum _renderType;
	GLuint _vaoId;
	GLuint _vbVertId;
	GLuint _vbColorId;
	GLuint _vbIndexId;
	GLuint _vbUVID;
	GLuint _vbNormId;
	GLuint _vbTangentId;
	GLuint _vbBiTangentId;
	bool _isVbVert;
	bool _isVbColor;
	bool _isVbIndex;
	bool _isVbUV;
	bool _isVbNorm;
	bool _isVbTan;
	bool _isVbBiTan;

	bool _isTexture[8];
	GLuint _TextureId[8];
	size_t _textureLayer[8];
	size_t _currentTexture;

	Material* _material;
	Mesh* _mesh;
	glLocationManager* _glLocManager;
	size_t _objIndex;
	bool _created;
	size_t _drawCount;
	bsphere<pfd>* _frustumSphere[9];
	GLsizei _octreeStart[9];
	Octree<GLIndex, pfd>* _octree;
	size_t _octreeDepth;
	bool _isOctree;
	std::vector<Triangle<GLIndex>> _triangleBuffer;
	
};

#endif