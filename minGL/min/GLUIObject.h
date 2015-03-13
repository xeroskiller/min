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
#ifndef __GLUIOBJECT__
#define __GLUIOBJECT__
#include "stdafx.h"

class OpenGLContext;
class Mesh;
class glLocationManager;
class Material;
#include <string>
class GLUIObject
{
public:
	friend class SceneManager;
	GLUIObject(OpenGLContext& glContext);
	~GLUIObject();
	GLuint getVaoId();
	size_t getDrawCount();
	size_t getIndexCount();
	GLenum getRenderType();
	Mesh* getMesh();
	void setMesh(Mesh& mesh);
	Material* getMaterial();
	void setMaterial(Material& mat);
	void createObject(GLenum renderType);
	bool loadTexture(const std::string& texturePath);
	void loadRectangleDDS(size_t x_size, size_t y_size);
	bool loadFileObj(const std::string& absFilePath, const std::string& extension);
	bool isIndex();
	bool isTexture();
	bool isCreated();
	glLocationManager* getGLLocManager();
	size_t getSceneIndex();
	void release();
protected:
	OpenGLContext* _glContext;
	size_t _vertSize;
	size_t _indexSize;
	GLenum _renderType;
	GLuint _vaoId;
	GLuint _vbVertId;
	bool _isVbVert;
	GLuint _vbColorId;
	bool _isVbColor;
	GLuint _vbIndexId;
	bool _isVbIndex;
	GLuint _Text2DId;
	GLuint _vbUVID;
	bool _isVbText;

	size_t _textureLayer;
	Material* _material;
	Mesh* _mesh;
	glLocationManager* _glLocManager;
	size_t _objIndex;
	bool _created;
};

#endif