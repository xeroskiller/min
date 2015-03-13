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
#include "SceneManager.h"
#include "LightManager.h"
#include "Object.h"
#include "UIObject.h"
#include "GLObject.h"
#include "GLUIObject.h"
#include "BoundedObject.h"
#include "PhysicsObject.h"
#include "PlayerObject.h"
#include "TextureManager.h"
#include "SystemClock.h"
#include "GLObject.h"
#include "aabbox3.h"
#include "minutils.h"

const size_t SceneManager::ShaderTableSize =10;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

SceneManager::SceneManager(OpenGLContext* glContext)
{
	_glContext = glContext;
	_pShaderTable = new Shader*[ShaderTableSize];
	_iShaderCount = 0;
	_camera = 0;
	_playerObject = 0;
}

SceneManager::~SceneManager(void)
{
	std::deque<Object*>::iterator k = _objects.begin();
	for(; k != _objects.end(); )
	{
		delete (*k);
		k = _objects.erase(k);
	}
	std::deque<GLObject*>::iterator l = _glObjects.begin();
	for(; l != _glObjects.end(); )
	{
		delete (*l);
		l = _glObjects.erase(l);
	}
	std::deque<UIObject*>::iterator m = _uiObjects.begin();
	for(; m != _uiObjects.end(); )
	{
		delete (*m);
		m = _uiObjects.erase(m);
	}
	std::deque<GLUIObject*>::iterator n = _glUIObjects.begin();
	for(; n != _glUIObjects.end(); )
	{
		delete (*n);
		n = _glUIObjects.erase(n);
	}

	if(_playerObject)
		delete _playerObject;

	std::deque<Program*>::iterator p = _programTable.begin();
	for(; p!= _programTable.end();)
	{
		delete (*p);
		p = _programTable.erase(p);
	}

	for(size_t i=0;i<_iShaderCount;i++)
	{
		delete _pShaderTable[i];
	}
	delete [] _pShaderTable;
}

size_t SceneManager::getObjectCount()
{
	return _objects.size();
}

Object* SceneManager::addObject()
{
	Object* obj = new Object(*_glContext);
	_objects.push_back(obj);
	obj->_objectIndex = _objects.size() - 1;
	return obj;
}

UIObject* SceneManager::addUIObject()
{
	UIObject* uiObj = new UIObject(*_glContext);
	_uiObjects.push_back(uiObj);
	uiObj->_objectIndex = _uiObjects.size() - 1;
	return uiObj;
}

GLObject* SceneManager::addGLObject()
{
	GLObject* glObj = new GLObject(*_glContext);
	_glObjects.push_back(glObj);
	glObj->_objIndex = _glObjects.size() - 1;
	return glObj;
}

void SceneManager::addGLObject(GLObject* glObj)
{
	_glObjects.push_back(glObj);
	glObj->_objIndex = _glObjects.size() - 1;
}

GLUIObject* SceneManager::addGLUIObject()
{
	GLUIObject* glUIObj = new GLUIObject(*_glContext);
	_glUIObjects.push_back(glUIObj);
	glUIObj->_objIndex = _glObjects.size() - 1;
	return glUIObj;
}

void SceneManager::addGLUIObject(GLUIObject* glUIObj)
{
	_glUIObjects.push_back(glUIObj);
	glUIObj->_objIndex = _glObjects.size() - 1;
}

BoundedObject* SceneManager::addBoundedObject()
{
	BoundedObject* obj = new BoundedObject(*_glContext);
	_objects.push_back(obj);
	obj->_objectIndex = _objects.size() - 1;
	return obj;
}

PhysicsObject* SceneManager::addPhysicsObject()
{
	PhysicsObject* obj = new PhysicsObject(*_glContext);
	_glContext->pmgr->setWorld(obj);
	_objects.push_back(obj);
	obj->_objectIndex = _objects.size() - 1;
	return obj;
}

PlayerObject* SceneManager::addPlayerObject()
{
	if(_playerObject != 0)
	{
		delete _playerObject;
	}
	PlayerObject* obj = new PlayerObject(*_glContext);
	_glContext->pmgr->setWorld(obj);
	_objects.push_back(obj);
	_playerObject = obj;
	return obj;
}

size_t SceneManager::loadShader(GLenum shaderType, char* filename)
{
	_pShaderTable[_iShaderCount] = new Shader(shaderType, _glContext);
	_pShaderTable[_iShaderCount]->LoadShader(filename);
	_iShaderCount++;
	if(_iShaderCount >= ShaderTableSize)
	{
		resizeShaderTable();
	}
	return _iShaderCount-1;
}

void SceneManager::resizeShaderTable()
{
	Shader** tempTable = new Shader*[2*_iShaderCount+1];
	for(size_t i=0; i<_iShaderCount; i++)
	{
		tempTable[i] = _pShaderTable[i];
	}
	delete [] _pShaderTable;
	_pShaderTable = tempTable;
}

size_t SceneManager::linkProgram(size_t vertex, size_t fragment)
{
	_programTable.push_back(new Program);
	GLuint prog = (_programTable.back())->_programId = glCreateProgram();
	(_programTable.back())->_vertIdIndex = vertex;
	(_programTable.back())->_fragIdIndex = fragment;

	glAttachShader(prog, _pShaderTable[vertex]->_shaderId);
	glAttachShader(prog, _pShaderTable[fragment]->_shaderId);
	glLinkProgram(prog);
	GLint link_ok;
	glGetProgramiv(prog, GL_LINK_STATUS, &link_ok);
	if(!link_ok)
    {
		fatal_error(_glContext, L"OpenGL error", L"Error: Failed to link shader program.");
    }
	check_glError(_glContext, L"OpenGL error", L"Error: GL error during linking.");
	
	glUseProgram(0);
	return _programTable.size() - 1;
}

void SceneManager::printErrorLog(GLuint object)
{
	GLint log_length =0;
	if(glIsProgram(object))
	{
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
		char* log = new char[log_length];
		glGetProgramInfoLog(object, log_length, NULL, log);
		wchar_t * wc = new wchar_t[log_length];
		mbstowcs(wc,log,log_length);
		error(_glContext, L"OpenGL error", wc);
		delete [] log;
		delete [] wc;
		exit(-1);
	}
	else 
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Object is not a program. Can't print error log.");
	}
}

void SceneManager::drawObjects()
{
	{
		Object* current = 0;
		std::deque<Object*>::iterator k = _objects.begin();
		Frustum* frustum = &_camera->_frustum;
		for(; k != _objects.end(); k++)
		{
			current = (*k);
			for(size_t i=0; i < current->_numGLObjects; i++)
			{
				bsphere<pfd> sphere = current->getFrustumSphere(i);
				if(frustum->isSphereInFrustum(sphere))
				{
					GLObject& glCurrent = (*k)->getGLObject(i);
					glBindVertexArray(glCurrent.getVaoId());
					GLuint progId = _programTable[current->getProgramIndex()]->_programId;
					glUseProgram(progId);
					for(size_t i=0; i < GLObject::MAX_TEXTURE; i++)
					{
						if(glCurrent.isTexture(i))
						{
							size_t tl = glCurrent.getGLLocManager()->getULocation(static_cast<glLocationManager::glLocationEnum>(glLocationManager::TEXTURE1_LOCATION + i));
							_glContext->tlmgr->switchTexture(glCurrent._textureLayer[i], tl, glCurrent._TextureId[i]);
						}
					}
					if(glCurrent.isIndex())
					{
						size_t drawCalls = glCurrent.getDrawCount();
 						if(drawCalls > 1)
						{
							for(size_t i=0; i < drawCalls; i++)
							{
								const bsphere<pfd>* sphere = glCurrent.getFrustumSphere(i);
								if(sphere != 0)
								{
									if(frustum->isSphereInFrustum(*sphere))
									{
										GLsizei start = sizeof(GLIndex)*glCurrent.getStart(i);
										glDrawElements(glCurrent.getRenderType(), glCurrent.getIndexCount(i), GL_UNSIGNED_SHORT, BUFFER_OFFSET(start));
									}
								}
							}
						}
						else
						{
							GLsizei start = sizeof(GLIndex)*glCurrent.getStart(0);
							glDrawElements(glCurrent.getRenderType(), glCurrent.getIndexCount(0), GL_UNSIGNED_SHORT, BUFFER_OFFSET(start));
						}
					}
					else
					{
						glDrawArrays(glCurrent.getRenderType(), 0, glCurrent.getVertexCount(0));
					}
				}
			}
		}
	}
	{
		UIObject* current = 0;
		GLUIObject* glCurrent = 0;
		std::deque<UIObject*>::iterator l = _uiObjects.begin();
		for(; l != _uiObjects.end(); l++)
		{
			current = (*l);
			glCurrent = (*l)->getGLUIObject();

			glBindVertexArray(glCurrent->getVaoId());
			GLuint progId = _programTable[current->getProgramIndex()]->_programId;
			glUseProgram(progId);
			if(glCurrent->isTexture())
			{
				size_t tl = glCurrent->getGLLocManager()->getULocation(glLocationManager::TEXTURE1_LOCATION);
				_glContext->tlmgr->switchTexture(glCurrent->_textureLayer, tl, glCurrent->_Text2DId);
			}
			if(glCurrent->isIndex())
			{
				glDrawElements(glCurrent->getRenderType(), glCurrent->getIndexCount(), GL_UNSIGNED_SHORT, (void*)0);
			}
			else
			{
				glDrawArrays(glCurrent->getRenderType(), 0, glCurrent->getDrawCount());
			}
		}
	}
	glUseProgram(0);
	glBindVertexArray(0);

	check_glError(_glContext, L"OpenGL error", L"Error: Could not draw objects.");
}

void SceneManager::updateObjects()
{
	_camera->update();
	{
		Object* current = 0;
		std::deque<Object*>::iterator k = _objects.begin();
		_camera->getMouseInput();
		#undef GetCurrentTime
		double time = _glContext->SysClock->GetCurrentTime();

		bool lightUpdate = _glContext->lmgr->isUpdated();

		for(; k != _objects.end(); )
		{
			current = (*k);
			if(_camera->hasMoved() || !current->isUpdated())
			{
				for(size_t i=0; i < current->_numGLObjects; i++)
				{
					glBindVertexArray(current->getGLObject(i).getVaoId());
					GLuint progId = _programTable[current->getProgramIndex()]->_programId;
					glUseProgram(progId);
					if(!lightUpdate)
					{
						_glContext->lmgr->setProgram(progId);
						_glContext->lmgr->SetLight();
					}
				}
				k = current->Update(*_camera, time, k);
			}
			else
			{
				k++;
			}
		}
		if(!lightUpdate)
		{
			_glContext->lmgr->setUpdate(true);
		}
	}
	{
		UIObject* current = 0;
		std::deque<UIObject*>::iterator l = _uiObjects.begin();
		for(; l != _uiObjects.end(); l++)
		{
			current = (*l);
			if(!current->isUpdated())
			{
				glBindVertexArray(current->getGLUIObject()->getVaoId());
				GLuint progId = _programTable[current->getProgramIndex()]->_programId;
				glUseProgram(progId);
				current->update();
			}
		}
	}

	glUseProgram(0);
	glBindVertexArray(0);
	check_glError(_glContext, L"OpenGL error", L"Error: Could not update objects.");
}

void SceneManager::destroyShaders()
{
	glUseProgram(0);

	for(size_t i=0;i<_programTable.size();i++)
	{
		GLuint prog = _programTable[i]->_programId;
		GLuint vertex = _pShaderTable[_programTable[i]->_vertIdIndex]->_shaderId;
		GLuint fragment = _pShaderTable[_programTable[i]->_fragIdIndex]->_shaderId;
		glDetachShader(prog, vertex);
		glDetachShader(prog, fragment);
		glDeleteProgram(prog);
	}
	for (size_t i = 0; i < _iShaderCount; i++)
	{
		GLuint shader = _pShaderTable[i]->_shaderId;
		glDeleteShader(shader);
	}
	check_glError(_glContext, L"OpenGL error", L"Error: Could not delete shaders.");
}

GLuint SceneManager::getProgramId(size_t progIdIndex)
{
	return _programTable[progIdIndex]->_programId;
}

void SceneManager::setCamera(Camera* cam)
{
	_camera = cam;
}

std::deque<Object*>::iterator SceneManager::removeFromScene(Object* obj)
{
	std::deque<Program*>::iterator k = _programTable.begin();
	std::deque<Object*>::iterator o = _objects.begin();
	std::advance(k, obj->getProgramIndex());
	std::advance(o, obj->getObjectIndex());
	delete (*o);
	o = _objects.erase(o);
	std::deque<Object*>::iterator p = o;
	for(; o != _objects.end(); o++)
	{
		(*o)->_objectIndex--;
		(*o)->_programIndex--;
	}
	delete (*k);
	k = _programTable.erase(k);

	return p;
}