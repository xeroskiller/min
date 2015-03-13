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
#include "GLObject.h"
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
#include "minutils.h"

GLObject::GLObject(OpenGLContext& glContext)
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
	_isVbUV = false;
	_vbTangentId = 0;
	_isVbTan = false;
	_vbBiTangentId = 0;
	_isVbBiTan = false;
	_vertSize=0;
	_indexSize = 0;
	_renderType = GL_TRIANGLES;
	_created = false;
	_drawCount = 1;
	_octree = 0;
	_octreeDepth = 0;
	_isOctree = false;

	for(size_t i=0; i < MAX_TEXTURE; i++)
	{
		_isTexture[i] = false;
		_textureLayer[i] = -1;
		_TextureId[i] = 0;
	}
	_currentTexture = 0;
}

GLObject::~GLObject()
{
	if(_mesh)
		delete _mesh;
	if(_material)
		delete _material;
	if (_isVbBiTan)
	{
		size_t bitan = _glLocManager->getVLocation(glLocationManager::BITANGENT_LOCATION);
		glDisableVertexAttribArray(bitan);
	}
	if (_isVbTan)
	{
		size_t tan = _glLocManager->getVLocation(glLocationManager::TANGENT_LOCATION);
		glDisableVertexAttribArray(tan);
	}
	if(_isVbNorm)
	{
		size_t norm = _glLocManager->getVLocation(glLocationManager::NORMAL_LOCATION);
		glDisableVertexAttribArray(norm);
	}
	if(_isVbUV)
	{
		size_t uv = _glLocManager->getVLocation(glLocationManager::UV_LOCATION);
		glDisableVertexAttribArray(uv);
	}

	for(size_t i=0; i < MAX_TEXTURE; i++)
	{
		if(_isTexture[i])
		{
			_glContext->tlmgr->disableTexture(_textureLayer[i]);
			glDeleteTextures(1, &_TextureId[i]);
		}
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
	if(_isVbUV)
		glDeleteBuffers(1, &_vbUVID);
	if (_isVbNorm)
		glDeleteBuffers(1, &_vbNormId);
	if (_isVbTan)
		glDeleteBuffers(1, &_vbTangentId);
	if (_isVbBiTan)
		glDeleteBuffers(1, &_vbBiTangentId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &_vaoId);

	check_glError(_glContext, L"OpenGL error", L"Error: Could not delete a VBO");
}

void  GLObject::createObject(GLenum renderType)
{
	if(!_created)
	{
		_renderType = renderType;

		GLenum ErrorCheckValue = glGetError();
		assert(ErrorCheckValue == 0);

		glGenVertexArrays(1, &_vaoId);
		glBindVertexArray(_vaoId);
		size_t vertSize = _mesh->_vertices.size();
		if(vertSize > 0)
		{
			size_t indexSize = _mesh->_indices.size();
			if(_isOctree && indexSize > 0)
			{
				const GLIndex* indices;
				size_t OctreeSize = _mesh->_octree.size();
				if(OctreeSize > 0)
				{
					indices = &_mesh->_octree[0];
					indexSize = _mesh->_octree.size();
				}	
				else
				{
					indices = &_mesh->_indices[0];
				}

				for(size_t i=0; i < indexSize; i+=3)
				{
					_triangleBuffer.push_back(Triangle<GLIndex>(indices[i], indices[i + 1], indices[i + 2], i));
				}
				
				_octree = new Octree<GLIndex, pfd>(&_mesh->_vertices, &_triangleBuffer, _octreeDepth);
				
				if( OctreeSize == 0)
				{
					_frustumSphere[0] = new bsphere<pfd>(&_mesh->_vertices[0], vertSize);
				}

				aabbox3<pfd> box(&_mesh->_vertices[0], vertSize);

				_octree->createOctree(box.getCenter(), box.getExtent());

				if(OctreeSize == 0)
				{
					_mesh->_indices.clear();
					size_t start = 0;
					for(size_t i=0; i < 8; i++)
					{
						_octreeStart[i] = start;
						_frustumSphere[i + 1] = _octree->sphereFromQuadrant(i, &_mesh->_indices);
						start += _mesh->_indices.size() - start;
					}
					_octreeStart[8] = start;
				}
			}
			else
			{
				_frustumSphere[0] = new bsphere<pfd>(&_mesh->_vertices[0], vertSize);
			}

			_vertSize = vertSize;
			_indexSize = indexSize;

			_isVbVert = true;
			const Vector4<pfd>* vertices = &_mesh->_vertices[0];
			glGenBuffers(1, &_vbVertId);
			glBindBuffer(GL_ARRAY_BUFFER, _vbVertId);
			glBufferData(GL_ARRAY_BUFFER, 4 * vertSize * sizeof(pfd), vertices, GL_STATIC_DRAW);
			size_t vl = _glLocManager->requestVLocation(glLocationManager::VERTEX_LOCATION);
			glVertexAttribPointer(vl, 4, GL_FLOAT, GL_FALSE, 0, 0); //GL_MAX_VERTEX_ATTRIBS is LIMIT
			glEnableVertexAttribArray(vl);
		}
		else
		{
			return;
		}

		if(_material != 0)
		{
			if(_material->GetColorSize() > 0)
			{
				_isVbColor = true;
				const Vector4<pfd>* colors = _material->GetColor();
				glGenBuffers(1, &_vbColorId);
				glBindBuffer(GL_ARRAY_BUFFER, _vbColorId);
				glBufferData(GL_ARRAY_BUFFER, 4 * _material->GetColorSize() * sizeof(pfd), colors, GL_STATIC_DRAW);
				size_t cl = _glLocManager->requestVLocation(glLocationManager::COLOR_LOCATION);
				glVertexAttribPointer(cl, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(cl);
			}

			Texture* text;
			for(size_t i=0 ; i < MAX_TEXTURE; i++)
			{
				if(_material->GetTexture(i) != 0)
				{
					text = _material->GetTexture(i);
					_isTexture[i] = true;
					_textureLayer[i] = _glContext->tlmgr->requestNewTextureLayer();
					glGenTextures(1, &_TextureId[i]);
					glBindTexture(GL_TEXTURE_2D, _TextureId[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
			}
		}
		size_t uvSize = _mesh->_uvcoords.size();
		if(uvSize > 0)
		{
			_isVbUV = true;
			const Vector2<pfd>* uvcoords = &_mesh->_uvcoords[0];
			glGenBuffers(1, &_vbUVID);
			glBindBuffer(GL_ARRAY_BUFFER, _vbUVID);
			glBufferData(GL_ARRAY_BUFFER, 2 * uvSize * sizeof(pfd), uvcoords, GL_STATIC_DRAW);
			size_t uv = _glLocManager->requestVLocation(glLocationManager::UV_LOCATION);
			glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(uv);
		}
		size_t tanSize = _mesh->_tangents.size();		
		if(tanSize > 0)
		{
			_isVbTan = true;
			const Vector3<pfd>* tangents = &_mesh->_tangents[0];
			glGenBuffers(1, &_vbTangentId);
			glBindBuffer(GL_ARRAY_BUFFER, _vbTangentId);
			glBufferData(GL_ARRAY_BUFFER, 3 * tanSize * sizeof(pfd), tangents, GL_STATIC_DRAW);
			size_t tl = _glLocManager->requestVLocation(glLocationManager::TANGENT_LOCATION);
			glVertexAttribPointer(tl, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(tl);
			size_t btanSize = _mesh->_bitangents.size();
			if(btanSize > 0)
			{
				_isVbBiTan = true;
				const Vector3<pfd>* bitangents = &_mesh->_bitangents[0];
				glGenBuffers(1, &_vbBiTangentId);
				glBindBuffer(GL_ARRAY_BUFFER, _vbBiTangentId);
				glBufferData(GL_ARRAY_BUFFER, 3 * btanSize * sizeof(pfd), bitangents, GL_STATIC_DRAW);
				size_t btl = _glLocManager->requestVLocation(glLocationManager::BITANGENT_LOCATION);
				glVertexAttribPointer(btl, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(btl);
			}
		}
		size_t normSize = _mesh->_normals.size();
		if(normSize > 0)
		{
			_isVbNorm = true;
			const Vector3<pfd>* normals = &_mesh->_normals[0];
			glGenBuffers(1, &_vbNormId);
			glBindBuffer(GL_ARRAY_BUFFER, _vbNormId);
			glBufferData(GL_ARRAY_BUFFER, 3 * normSize * sizeof(pfd), normals, GL_STATIC_DRAW);
			size_t nl = _glLocManager->requestVLocation(glLocationManager::NORMAL_LOCATION);
			glVertexAttribPointer(nl, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(nl);
		}
		size_t indexSize = _mesh->_indices.size();
		if(indexSize > 0)
		{
			const GLIndex* indices = &_mesh->_indices[0];
			_isVbIndex = true;
			glGenBuffers(1, &_vbIndexId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbIndexId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLIndex), indices, GL_STATIC_DRAW);
		}
		
		glBindVertexArray(0);
		check_glError(_glContext, L"OpenGL error", L"Error: Could not create animated object.");
		_created = true;
	}
}

bool GLObject::loadTexture(const std::string& texturePath)
{
	Texture* texture = new Texture;
	bool test = texture->LoadTexture(texturePath);
	if(test)
	{
		_material->SetTexture(texture, _currentTexture);
		_currentTexture++;
	}
	else
	{
		delete texture;
	}
	return test;
}

GLObject** GLObject::loadModel(OpenGLContext& glContext, OBJModel& model, size_t& n)
{
	if(model.isLoaded())
	{
		 const size_t meshCount = model.getMeshCount();
		 if(meshCount > 0)
		 {
			 GLObject** glObjArray = new GLObject*[meshCount];
			 n = meshCount;
			 for(size_t i=0; i < meshCount; i++)
			 {
				 glObjArray[i] = new GLObject(glContext);
				 glContext.smgr->addGLObject( glObjArray[i]);
				 Mesh& mesh = model.getMesh(i);
				 glObjArray[i]->setMesh(model.getMesh(i));
			 }
			 return glObjArray;
		 }
		 else
		 {
			 return 0;
		 }
	}
	else
	{
		return 0;
	}
}

//overwrite must equal true for now
bool GLObject::saveYageObj(const std::string& absFilePath, bool overwrite)
{
	if(_mesh == 0)
		return false;
	else
	{
		DataFile::Datafile file(absFilePath, overwrite);
		size_t loc, header, vertex, uv, normal, tangent, bitangent, index, octree, sphere, start; 
		DataFile::DatabaseTable* table = 0;
		if(file.is_open())
		{
			if(file.is_new())
			{
				loc = file.AddTable();
			}
			table = file.GetTable(loc);
			if(file.is_new())
			{
				header = table->Add(DataFile::DatabaseTable::STRING);
				DataFile::StringContainer*ptr_header = static_cast<DataFile::StringContainer*>(table->GetDatabaseContainer(header));
				if(_mesh->_vertices.size() > 0)
					ptr_header->Add("Vertex", 0);
				if(_mesh->_uvcoords.size() > 0)
					ptr_header->Add("UV", 1);
				if(_mesh->_normals.size() > 0)
					ptr_header->Add("Normal", 2);
				if(_mesh->_tangents.size() > 0)
					ptr_header->Add("Tangent", 3);
				if(_mesh->_bitangents.size() > 0)
					ptr_header->Add("Bitangent", 4);
				if(_mesh->_indices.size() > 0)
					ptr_header->Add("Index", 5);
				if (_isOctree > 0)
					ptr_header->Add("Octree", 6);
			}
			if(table != 0)
			{
				table->setName("YageObj");
				const size_t vertSize = _mesh->_vertices.size();
				if(vertSize > 0)
				{
					if(precision == GL_FLOAT)
					{
						vertex = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_vertex = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(vertex));
						for(size_t i=0; i < vertSize; i++)
						{
							ptr_vertex->Add(static_cast<float>(_mesh->_vertices[i]._arr[0]), i);
							ptr_vertex->Add(static_cast<float>(_mesh->_vertices[i]._arr[1]), i);
							ptr_vertex->Add(static_cast<float>(_mesh->_vertices[i]._arr[2]), i);
							ptr_vertex->Add(static_cast<float>(_mesh->_vertices[i]._arr[3]), i);
						}
					}
					else
					{
						vertex = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_vertex = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(vertex));
						for(size_t i=0; i < vertSize; i++)
						{
							ptr_vertex->Add(static_cast<double>(_mesh->_vertices[i]._arr[0]), i);
							ptr_vertex->Add(static_cast<double>(_mesh->_vertices[i]._arr[1]), i);
							ptr_vertex->Add(static_cast<double>(_mesh->_vertices[i]._arr[2]), i);
							ptr_vertex->Add(static_cast<double>(_mesh->_vertices[i]._arr[3]), i);
						}
					}
				}
				const size_t uvSize = _mesh->_uvcoords.size();
				if(uvSize > 0)
				{
					if(precision == GL_FLOAT)
					{
						uv = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_uv = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(uv));
						for(size_t i=0; i < uvSize; i++)
						{
							ptr_uv->Add(static_cast<float>(_mesh->_uvcoords[i]._arr[0]), i);
							ptr_uv->Add(static_cast<float>(_mesh->_uvcoords[i]._arr[1]), i);
						}
					}
					else
					{
						uv = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_uv = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(uv));
						for(size_t i=0; i < uvSize; i++)
						{
							ptr_uv->Add(static_cast<double>(_mesh->_uvcoords[i]._arr[0]), i);
							ptr_uv->Add(static_cast<double>(_mesh->_uvcoords[i]._arr[1]), i);
						}
					}
				}
				const size_t normSize = _mesh->_normals.size();
				if(normSize > 0)
				{
					if(precision == GL_FLOAT)
					{
						normal = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_normal = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(normal));
						for(size_t i=0; i < normSize; i++)
						{
							ptr_normal->Add(static_cast<float>(_mesh->_normals[i]._arr[0]), i);
							ptr_normal->Add(static_cast<float>(_mesh->_normals[i]._arr[1]), i);
							ptr_normal->Add(static_cast<float>(_mesh->_normals[i]._arr[2]), i);
						}
					}
					else
					{
						normal = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_normal = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(normal));
						for(size_t i=0; i < normSize; i++)
						{
							ptr_normal->Add(static_cast<double>(_mesh->_normals[i]._arr[0]), i);
							ptr_normal->Add(static_cast<double>(_mesh->_normals[i]._arr[1]), i);
							ptr_normal->Add(static_cast<double>(_mesh->_normals[i]._arr[2]), i);
						}
					}
				}
				const size_t tanSize = _mesh->_tangents.size();
				if(tanSize > 0)
				{
					if(precision == GL_FLOAT)
					{
						tangent = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_tangent = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(tangent));
						for(size_t i=0; i < tanSize; i++)
						{
							ptr_tangent->Add(static_cast<float>(_mesh->_tangents[i]._arr[0]), i);
							ptr_tangent->Add(static_cast<float>(_mesh->_tangents[i]._arr[1]), i);
							ptr_tangent->Add(static_cast<float>(_mesh->_tangents[i]._arr[2]), i);
						}
					}
					else
					{
						tangent = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_tangent = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(tangent));
						for(size_t i=0; i < tanSize; i++)
						{
							ptr_tangent->Add(static_cast<double>(_mesh->_tangents[i]._arr[0]), i);
							ptr_tangent->Add(static_cast<double>(_mesh->_tangents[i]._arr[1]), i);
							ptr_tangent->Add(static_cast<double>(_mesh->_tangents[i]._arr[2]), i);
						}
					}
				}
				const size_t btanSize = _mesh->_bitangents.size();
				if(btanSize > 0)
				{
					if(precision == GL_FLOAT)
					{
						bitangent = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_bitangent = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(bitangent));
						for(size_t i=0; i < btanSize; i++)
						{
							ptr_bitangent->Add(static_cast<float>(_mesh->_bitangents[i]._arr[0]), i);
							ptr_bitangent->Add(static_cast<float>(_mesh->_bitangents[i]._arr[1]), i);
							ptr_bitangent->Add(static_cast<float>(_mesh->_bitangents[i]._arr[2]), i);
						}
					}
					else
					{
						bitangent = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_bitangent = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(bitangent));
						for(size_t i=0; i < btanSize; i++)
						{
							ptr_bitangent->Add(static_cast<double>(_mesh->_bitangents[i]._arr[0]), i);
							ptr_bitangent->Add(static_cast<double>(_mesh->_bitangents[i]._arr[1]), i);
							ptr_bitangent->Add(static_cast<double>(_mesh->_bitangents[i]._arr[2]), i);
						}
					}
				}
				const size_t indexSize = _mesh->_indices.size();
				if(indexSize > 0)
				{
					index = table->Add(DataFile::DatabaseTable::UINT);
					DataFile::UIntContainer* ptr_index = static_cast<DataFile::UIntContainer*>(table->GetDatabaseContainer(index));
					for(size_t i=0; i < indexSize; i++)
					{
						ptr_index->Add(static_cast<unsigned long>(_mesh->_indices[i]), i);
					}
				}
				
				if(_isOctree)
				{
					octree = table->Add(DataFile::DatabaseTable::UINT);
					DataFile::UIntContainer* ptr_octree = static_cast<DataFile::UIntContainer*>(table->GetDatabaseContainer(octree));
					std::vector<GLIndex> indices = _octree->saveOctree();
					const size_t octreeSize = indices.size();
					for(size_t i=0; i < octreeSize; i++)
					{
						ptr_octree->Add(static_cast<unsigned long>(indices[i]), i);
					}

					if(precision == GL_FLOAT)
					{
						sphere = table->Add(DataFile::DatabaseTable::FLOAT);
						DataFile::FloatContainer* ptr_sphere = static_cast<DataFile::FloatContainer*>(table->GetDatabaseContainer(sphere));
						for(size_t i=0; i < 9; i++)
						{
							const bsphere<pfd>* sp = _frustumSphere[i];
							if(sp != 0)
							{
								Vector3<pfd> center = sp->getCenter();
								ptr_sphere->Add(center._arr[0], i);
								ptr_sphere->Add(center._arr[1], i);
								ptr_sphere->Add(center._arr[2], i);
								ptr_sphere->Add(sp->getRadius(), i);
							}
						}
					}
					else
					{
						sphere = table->Add(DataFile::DatabaseTable::DOUBLE);
						DataFile::DoubleContainer* ptr_sphere = static_cast<DataFile::DoubleContainer*>(table->GetDatabaseContainer(sphere));
						for(size_t i=0; i < 9; i++)
						{
							const bsphere<pfd>* sp = _frustumSphere[i];
							if(sp != 0)
							{
								Vector3<pfd> center = sp->getCenter();
								ptr_sphere->Add(center._arr[0], i);
								ptr_sphere->Add(center._arr[1], i);
								ptr_sphere->Add(center._arr[2], i);
								ptr_sphere->Add(sp->getRadius(), i);
							}
						}
					}

					start = table->Add(DataFile::DatabaseTable::UINT);
					ptr_octree = static_cast<DataFile::UIntContainer*>(table->GetDatabaseContainer(start));
					for(size_t i=0; i < 9; i++)
					{
						ptr_octree->Add(static_cast<unsigned long>(_octreeStart[i]), i);
					}
				}
				if(!file.Save())
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
				return false;
		}
		return false;
	}
}

bool GLObject::loadYageObj(const std::string& absFilePath)
{
	if(_created)
		return false;
	DataFile::Datafile file(absFilePath, false);
	DataFile::DatabaseTable* table = 0;
	if(file.is_open())
	{
		if(file.size())
			table = file.GetTable(0);
		if(table == 0)
			return false;
		bool vertex, uv, normal, tangent, bitangent, index, octree = false;
		DataFile::DatabaseContainerBase* ptr_header = table->GetDatabaseContainer(0);
		if(!ptr_header)
			return false;
		if(ptr_header->GetType() == DataFile::DatabaseTable::STRING)
		{
			DataFile::StringContainer* ptr_str = static_cast<DataFile::StringContainer*>(ptr_header);
			for(size_t i=0; i < ptr_header->Length(); i++)
			{
				std::string str = (*ptr_str)[i];
				if(str == "Vertex")
				{
					vertex = true;
				}
				else if(str == "UV")
				{
					uv = true;
				}
				else if(str == "Normal")
				{
					normal = true;
				}
				else if(str == "Tangent")
				{
					tangent = true;
				}
				else if(str == "Bitangent")
				{
					bitangent = true;
				}
				else if(str == "Index")
				{
					index = true;
				}
				else if(str == "Octree")
				{
					octree = true;
				}
			}

			if(vertex)
			{
				
				DataFile::DatabaseContainerBase* ptr_vertex = table->GetDatabaseContainer(1);
				if(!ptr_vertex)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr_vertex->GetType();
				std::vector<Vector4<pfd>>& vertex = _mesh->_vertices;
				if( data_enum == DataFile::DatabaseTable::DOUBLE)
				{
					DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr_vertex);
					for(size_t i=0; i < ptr_dbl->Length(); i+=4)
					{
						vertex.push_back(Vector4<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2]), static_cast<pfd>((*ptr_dbl)[i + 3])));
					}
				}
				else if(data_enum == DataFile::DatabaseTable::FLOAT)
				{
					DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr_vertex);
					for(size_t i=0; i < ptr_dbl->Length(); i+=4)
					{
						vertex.push_back(Vector4<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2]), static_cast<pfd>((*ptr_dbl)[i + 3])));
					}
				}
				else
					return false;
			}

			if(uv)
			{
				DataFile::DatabaseContainerBase* ptr_uv = table->GetDatabaseContainer(2);
				if(!ptr_uv)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr_uv->GetType();
				std::vector<Vector2<pfd>>& uv = _mesh->_uvcoords;
				if( data_enum == DataFile::DatabaseTable::DOUBLE)
				{
					DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr_uv);
					for(size_t i=0; i < ptr_dbl->Length(); i+=2)
					{
						uv.push_back(Vector2<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1])));
					}
				}
				else if(data_enum == DataFile::DatabaseTable::FLOAT)
				{
					DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr_uv);
					for(size_t i=0; i < ptr_dbl->Length(); i+=2)
					{
						uv.push_back(Vector2<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1])));
					}
				}
				else
					return false;
			}

			if(normal)
			{
				DataFile::DatabaseContainerBase* ptr_normal = table->GetDatabaseContainer(3);
				if(!ptr_normal)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr_normal->GetType();
				std::vector<Vector3<pfd>>& normal = _mesh->_normals;
				if( data_enum == DataFile::DatabaseTable::DOUBLE)
				{
					DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr_normal);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						normal.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else if(data_enum == DataFile::DatabaseTable::FLOAT)
				{
					DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr_normal);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						normal.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else
					return false;
			}

			if(tangent)
			{
				DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(4);
				if(!ptr)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
				std::vector<Vector3<pfd>>& tangent = _mesh->_tangents;
				if( data_enum == DataFile::DatabaseTable::DOUBLE)
				{
					DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						tangent.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else if(data_enum == DataFile::DatabaseTable::FLOAT)
				{
					DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						tangent.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else
					return false;
			}

			if(bitangent)
			{
				DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(5);
				if(!ptr)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
				std::vector<Vector3<pfd>>& bitangent = _mesh->_bitangents;
				if( data_enum == DataFile::DatabaseTable::DOUBLE)
				{
					DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						bitangent.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else if(data_enum == DataFile::DatabaseTable::FLOAT)
				{
					DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr);
					for(size_t i=0; i < ptr_dbl->Length(); i+=3)
					{
						bitangent.push_back(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])));
					}
				}
				else
					return false;
			}

			if(index)
			{
				DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(6);
				if(!ptr)
					return false;
				DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
				std::vector<GLIndex>& index = _mesh->_indices;
				if( data_enum == DataFile::DatabaseTable::UINT)
				{
					DataFile::UIntContainer* ptr_dbl = static_cast<DataFile::UIntContainer*>(ptr);
					for(size_t i=0; i < ptr_dbl->Length(); i++)
					{
						index.push_back(static_cast<GLIndex>((*ptr_dbl)[i]));
					}
				}
				else
					return false;
			}

			if(octree)
			{
				{
					DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(7);
					if(!ptr)
						return false;
					DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
					std::vector<GLIndex>& octree = _mesh->_octree;
					if( data_enum == DataFile::DatabaseTable::UINT)
					{
						DataFile::UIntContainer* ptr_dbl = static_cast<DataFile::UIntContainer*>(ptr);
						for(size_t i=0; i < ptr_dbl->Length(); i++)
						{
							octree.push_back(static_cast<GLIndex>((*ptr_dbl)[i]));
						}
					}
					else
						return false;
				}

				{
					DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(8);
					if(!ptr)
						return false;
					DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
					if( data_enum == DataFile::DatabaseTable::DOUBLE)
					{
						DataFile::DoubleContainer* ptr_dbl = static_cast<DataFile::DoubleContainer*>(ptr);
						size_t count = 0;
						for(size_t i=0; i < ptr_dbl->Length(); i+=4)
						{
							_frustumSphere[count] = new bsphere<pfd>(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])), static_cast<pfd>((*ptr_dbl)[i + 3]));
							count++;
						}
					}
					else if(data_enum == DataFile::DatabaseTable::FLOAT)
					{
						DataFile::FloatContainer* ptr_dbl = static_cast<DataFile::FloatContainer*>(ptr);
						size_t count = 0;
						for(size_t i=0; i < ptr_dbl->Length(); i+=4)
						{
							_frustumSphere[count] = new bsphere<pfd>(Vector3<pfd>(static_cast<pfd>((*ptr_dbl)[i]), static_cast<pfd>((*ptr_dbl)[i + 1]), static_cast<pfd>((*ptr_dbl)[i + 2])), static_cast<pfd>((*ptr_dbl)[i + 3]));
							count++;
						}
					}
					else
						return false;
				}

				{
					DataFile::DatabaseContainerBase* ptr = table->GetDatabaseContainer(9);
					if(!ptr)
						return false;
					DataFile::DatabaseTable::DatabaseTableEnum data_enum = ptr->GetType();
					if( data_enum == DataFile::DatabaseTable::UINT)
					{
						DataFile::UIntContainer* ptr_dbl = static_cast<DataFile::UIntContainer*>(ptr);
						size_t frustumCount = ptr_dbl->Length();
						for(size_t i=0; i < frustumCount; i++)
						{
							_octreeStart[i] = static_cast<GLsizei>((*ptr_dbl)[i]);
						}
					}
					else
						return false;
				}
			}
			return true;
		}
		else
			return false;
	}
	else
	{
		return false;
	}
}

void GLObject::scaleUV(pfd scaleFactor)
{
	std::vector<Vector2<pfd>>& uvcoords = _mesh->_uvcoords;
	const size_t uvSize = uvcoords.size();
	if(uvSize > 0)
	{
		for(size_t i=0; i < uvSize; i++)
		{
			uvcoords[i] = uvcoords[i] * scaleFactor;
		}
	}
	else
	{
		return;
	}
}

GLuint GLObject::getVaoId()
{
	return _vaoId;
}

GLenum GLObject::getRenderType()
{
	return _renderType;
}

size_t GLObject::getDrawCount()
{
	if(_isOctree)
	{
		return 8;
	}
	else
	{
		return 1;
	}
}

GLsizei GLObject::getIndexCount(size_t n)
{
	if (_isOctree)
	{
		return _octreeStart[n + 1] - _octreeStart[n];
	}
	else
	{
		return _indexSize;
	}
}

GLsizei GLObject::getVertexCount(size_t n)
{
	if(_isOctree)
	{
		return _octreeStart[n + 1] - _octreeStart[n];
	}
	else
	{
		return _vertSize;
	}
}

GLsizei GLObject::getStart(size_t n)
{
	if(_isOctree)
	{
		return _octreeStart[n];
	}
	else
	{
		return 0;
	}
}

bool GLObject::isCreated()
{
	return _created;
}

void GLObject::enableOctree(bool state, size_t depth)
{
	_isOctree = state;
	_octreeDepth = depth;
}

Material* GLObject::getMaterial()
{
	return _material;
}

void GLObject::setMaterial(Material& mat)
{
	if(_material)
		delete _material;
	_material = &mat;
}

Mesh* GLObject::getMesh()
{
	return _mesh;
}

void GLObject::setMesh(Mesh& mesh)
{
	if(_mesh)
		delete _mesh;
	_mesh = &mesh;
}

bool GLObject::isIndex()
{
	return _isVbIndex;
}

bool GLObject::isTexture(size_t n)
{
	assert(n < MAX_TEXTURE);
	return _isTexture[n];
}

glLocationManager* GLObject::getGLLocManager()
{
	return _glLocManager;
}

size_t GLObject::getSceneIndex()
{
	return _objIndex;
}

void GLObject::release()
{
	delete _mesh;
	delete _material;
}