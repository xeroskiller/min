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
#include "OpenGLContext.h"
#include "GLUIObject.h"
#include "Material.h"
#include "Mesh.h"
#include "glLocationManager.h"
#include "Vector2.h"
#include "TextureManager.h"

#include <fstream>
#include <sstream>
#include <vector>
#include "minutils.h"

GLUIObject::GLUIObject(OpenGLContext& glContext)
{
	_glContext = &glContext;
	_glLocManager = new glLocationManager;
	
	_mesh = new Mesh;
	_material = new Material;

	_vaoId=0;
	_vbVertId=0;
	_isVbVert = false;
	_vbColorId=0;
	_isVbColor = false;
	_vbIndexId = 0;
	_isVbIndex = false;
	_Text2DId = -1;
	_isVbText = false;
	_textureLayer = 0;
	
	_vertSize=0;
	_indexSize = 0;
	_renderType = GL_TRIANGLES;
	_created = false;
}


GLUIObject::~GLUIObject(void)
{
	if(_mesh)
		delete _mesh;
	if(_material)
		delete _material;
	if(_isVbText)
	{
		size_t uv = _glLocManager->getVLocation(glLocationManager::UV_LOCATION);
		glDisableVertexAttribArray(uv);
		_glContext->tlmgr->disableTexture(_textureLayer);
		glDeleteTextures(1, &_Text2DId);
	}
	if(_isVbColor)
	{
		size_t cl = _glLocManager->getVLocation(glLocationManager::COLOR_LOCATION);
		glDisableVertexAttribArray(cl);
	}
	if(_isVbVert)
	{
		size_t vl = _glLocManager->getVLocation(glLocationManager::VERTEX_LOCATION);
		glDisableVertexAttribArray(vl);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if(_isVbVert)
		glDeleteBuffers(1, &_vbVertId);
	if(_isVbColor)
		glDeleteBuffers(1, &_vbColorId);
	if(_isVbIndex)
		glDeleteBuffers(1, &_vbIndexId);
	if(_isVbText)
		glDeleteBuffers(1, &_vbUVID);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_vaoId);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not delete a VBO");
}

void GLUIObject::createObject(GLenum renderType)
{
	if(!_created)
	{
		_renderType = renderType;
		size_t vertSize = _mesh->_uiVertices.size();
		size_t indexSize = _mesh->_indices.size();

		_vertSize = vertSize;
		_indexSize = indexSize;

		GLenum ErrorCheckValue = glGetError();
		assert(ErrorCheckValue == 0);

		glGenVertexArrays(1, &_vaoId);
		glBindVertexArray(_vaoId);
		if(vertSize > 0)
		{
			_isVbVert = true;
			const Vector2<pfd>* vertices = &_mesh->_uiVertices[0];
			glGenBuffers(1, &_vbVertId);
			glBindBuffer(GL_ARRAY_BUFFER, _vbVertId);
			glBufferData(GL_ARRAY_BUFFER, 2 * vertSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
			size_t vl = _glLocManager->requestVLocation(glLocationManager::VERTEX_LOCATION);
			glVertexAttribPointer(vl, 2, GL_FLOAT, GL_FALSE, 0, 0); //GL_MAX_VERTEX_ATTRIBS is LIMIT
			glEnableVertexAttribArray(vl);
		}
		if(_material != 0)
		{
			if(_material->GetColorSize() > 0)
			{
				_isVbColor = true;
				const Vector4<pfd>* colors = _material->GetColor();
				glGenBuffers(1, &_vbColorId);
				glBindBuffer(GL_ARRAY_BUFFER, _vbColorId);
				glBufferData(GL_ARRAY_BUFFER, 4 * _material->GetColorSize() * sizeof(GLfloat), colors, GL_STATIC_DRAW);
				size_t cl = _glLocManager->requestVLocation(glLocationManager::COLOR_LOCATION);
				glVertexAttribPointer(cl, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(cl);
			}

			if(_material->GetTexture(0) != 0)
			{
				Texture* text = _material->GetTexture(0);
				_isVbText = true;
				_textureLayer = _glContext->tlmgr->requestNewTextureLayer();
				glGenTextures(1, &_Text2DId);
				glBindTexture(GL_TEXTURE_2D, _Text2DId);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				if(text->Format() == GL_RGB8)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, text->Format(), text->Width(), text->Height(), 0, GL_BGR, GL_UNSIGNED_BYTE, text->Data());
				}
				else
				{
					unsigned int blockSize = (text->Format() == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
					unsigned int offset = 0;
					//load all mipmaps
					unsigned int width = text->Width();
					unsigned int height = text->Height();
					for(unsigned int level = 0; level < text->MipMapCount() &&  (width || height); ++level)
					{
						unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
						glCompressedTexImage2D(GL_TEXTURE_2D, level, text->Format(), width, height, 0, size, text->Data() + offset);

						offset += size;
						width  /= 2;
						height /= 2;
					}
				}
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			const size_t uvSize = _mesh->_uvcoords.size(); 
			if(uvSize > 0)
			{
				const Vector2<pfd>* uvcoords = &_mesh->_uvcoords[0];
				glGenBuffers(1, &_vbUVID);
				glBindBuffer(GL_ARRAY_BUFFER, _vbUVID);
				glBufferData(GL_ARRAY_BUFFER, 2 * uvSize * sizeof(GLfloat), uvcoords, GL_STATIC_DRAW);
				size_t uv = _glLocManager->requestVLocation(glLocationManager::UV_LOCATION);
				glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(uv);
			}
		}
		
		if(indexSize > 0)
		{
			const GLIndex* indices = &_mesh->_indices[0];
			_isVbIndex = true;
			glGenBuffers(1, &_vbIndexId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbIndexId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLIndex), indices, GL_STATIC_DRAW);
		}

		glBindVertexArray(0);
		check_glError(_glContext, L"OpenGL error", L"Error: Could not create a VBO");
		_created = true;
	}
}

bool GLUIObject::loadTexture(const std::string& texturePath)
{
	Texture* texture = new Texture;
	bool test = texture->LoadTexture(texturePath);
	if(test)
	{
		_material->SetTexture(texture, 0);
	}
	else
	{
		delete texture;
	}
	return test;
}

void GLUIObject::loadRectangleDDS(size_t x_size, size_t y_size)
{
	pfd x_pos = (pfd)0.0;
	pfd y_pos = (pfd)0.0;
	_mesh->_uiVertices.clear();
	_mesh->_uiVertices.reserve(4);
	
	_mesh->_indices.clear();
	_mesh->_indices.reserve(6);

	_mesh->_uvcoords.clear();
	_mesh->_uvcoords.reserve(4);

	size_t w = _glContext->windowWidth;
	size_t h = _glContext->windowHeight;

	pfd x_C = ((pfd)1.0/w) * (pfd)x_size;
	pfd y_C = ((pfd)1.0/h) * (pfd)y_size;

	_mesh->_indices.push_back(0);
	_mesh->_indices.push_back(3);
	_mesh->_indices.push_back(1);
	_mesh->_indices.push_back(2);
	_mesh->_indices.push_back(1);
	_mesh->_indices.push_back(3);

	_mesh->_uiVertices.push_back(Vector2<pfd>((pfd)(x_pos - x_C), (pfd)(y_pos + y_C)));
	_mesh->_uiVertices.push_back(Vector2<pfd>((pfd)(x_pos + x_C), (pfd)(y_pos + y_C)));
	_mesh->_uiVertices.push_back(Vector2<pfd>((pfd)(x_pos + x_C), (pfd)(y_pos - y_C)));
	_mesh->_uiVertices.push_back(Vector2<pfd>((pfd)(x_pos - x_C), (pfd)(y_pos - y_C)));

	_mesh->_uvcoords.push_back(Vector2<pfd>((pfd)0.0, (pfd)0.0));
	_mesh->_uvcoords.push_back(Vector2<pfd>((pfd)1.0, (pfd)0.0));
	_mesh->_uvcoords.push_back(Vector2<pfd>((pfd)1.0, (pfd)1.0));
	_mesh->_uvcoords.push_back(Vector2<pfd>((pfd)0.0, (pfd)1.0));
}

bool GLUIObject::loadFileObj(const std::string& absFilePath, const std::string& extension)
{
	std::vector<Vector2<pfd>> vert;
	std::vector<GLIndex> element;
	std::vector<Vector2<pfd>> uv;
	
	 pfd x,y;
	 size_t face_mode = 0;
	 std::ifstream file (absFilePath, std::ios::in);
	 if(file.is_open())
	 {
		std::string line;
		while(std::getline(file, line))
		{
			if(line.substr(0, 2) == "v ")
			{
				std::istringstream s(line.substr(2));
				s >> x;
				s >> y;
				Vector2<pfd> vec2(x,y);
				vert.push_back(vec2);
			}
			else if(line.substr(0,2) == "vt")
			{
				std::istringstream s(line.substr(2));
				pfd u,v;
				s >> u;
				s >> v;
				Vector2<pfd> vec2;
				if(extension == "bmp" || extension == "BMP")
				{
					vec2 = Vector2<pfd> (u,v);
				}
				else if(extension == "dds" || extension == "DDS")
				{
					vec2 = Vector2<pfd> (u,(pfd)1.0 - v);
				}
				uv.push_back(vec2);
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
			else {}
		}
		if(vert.size() > 0)
		{
			std::vector<Vector2<pfd>>& vert_out= _mesh->_uiVertices;
			std::vector<Vector2<pfd>>& uv_out= _mesh->_uvcoords;
			std::vector<GLIndex>& element_out= _mesh->_indices;
			switch(face_mode)
			{
				case 3:
				{
					_mesh->_uiVertices = vert;
					_mesh->_indices = element;
					break;
				}
				case 6:
				{
					bool isUV = false;
					if(uv.size() > 0)
					{
						isUV = true;
					}
					std::vector<GLIndex>::iterator k = element.begin();
					std::vector<std::vector<GLIndex>> map;
					std::vector<std::vector<GLIndex>>::iterator m = map.begin();
					size_t step = 2;
					bool fresh = true;
					size_t index = 0;
					for(; k!= element.end(); k = k + step)
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
							element_out.push_back(index);
							index++;
						}
						else
						{
							element_out.push_back(m - map.begin());
						}
					}
				}
			}
			return true;
		}
	 }
	 return false;
}

GLuint GLUIObject::getVaoId()
{
	return _vaoId;
}

GLenum GLUIObject::getRenderType()
{
	return _renderType;
}

size_t GLUIObject::getDrawCount()
{
	return _vertSize;
}

size_t GLUIObject::getIndexCount()
{
	return _indexSize;
}

bool GLUIObject::isCreated()
{
	return _created;
}

Material* GLUIObject::getMaterial()
{
	return _material;
}

void GLUIObject::setMaterial(Material& mat)
{
	if(_material)
		delete _material;
	_material = &mat;
}

Mesh* GLUIObject::getMesh()
{
	return _mesh;
}

void GLUIObject::setMesh(Mesh& mesh)
{
	if(_mesh)
		delete _mesh;
	_mesh = &mesh;
}

bool GLUIObject::isIndex()
{
	return _isVbIndex;
}

bool GLUIObject::isTexture()
{
	return _isVbText;
}

glLocationManager* GLUIObject::getGLLocManager()
{
	return _glLocManager;
}

size_t GLUIObject::getSceneIndex()
{
	return _objIndex;
}

void GLUIObject::release()
{
	delete _mesh;
	delete _material;
}