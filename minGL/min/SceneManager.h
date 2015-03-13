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
#ifndef __SceneManager__
#define __SceneManager__

#include "stdafx.h"
class Object;
class UIObject;
class GLObject;
class GLUIObject;
class BoundedObject;
class PhysicsObject;
class PlayerObject;
class Light;
class Shader;
#include "Shader.h"
class OpenGLContext;
#include "OpenGLContext.h"
#include "minutils.h"
class Camera;
#include "Camera.h"

#include <deque>

struct Program
{
	GLuint  _programId;
	size_t _vertIdIndex;
	size_t _fragIdIndex;
};

class SceneManager
{
public:
	static const size_t ShaderTableSize;
	static const size_t ProgramTableSize;
	SceneManager(OpenGLContext*);
	~SceneManager();
	size_t getObjectCount();
	Object* addObject();
	UIObject* addUIObject();
	GLObject* addGLObject();
	void addGLObject(GLObject* glObj);
	GLUIObject* addGLUIObject();
	void addGLUIObject(GLUIObject* glUIObj);
	BoundedObject* addBoundedObject();
	PhysicsObject* addPhysicsObject();
	PlayerObject* addPlayerObject();
	size_t loadShader(GLenum, char*);
	void printErrorLog(GLuint object);
	void destroyShaders();
	void resizeShaderTable();
	size_t linkProgram(size_t vertex, size_t fragment);
	void drawObjects();
	void updateObjects();
	GLuint getProgramId(size_t ProgIdIndex);
	void setCamera(Camera*);
	std::deque<Object*>::iterator removeFromScene(Object* obj);
	Camera* _camera;
private:
	OpenGLContext* _glContext;
	std::deque<Object*> _objects;
	std::deque<UIObject*> _uiObjects;
	std::deque<GLObject*> _glObjects;
	std::deque<GLUIObject*> _glUIObjects;
	std::deque<Program*> _programTable;
	PlayerObject* _playerObject;
	Shader** _pShaderTable;
	size_t  _iShaderCount;
};

#endif