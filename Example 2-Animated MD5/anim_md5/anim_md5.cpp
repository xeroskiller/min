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
// opengl.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "min().h"
#include <string>

OpenGLContext* openglContext;
PhysicsObject* world =0;
GLObject* glWorld =0;
MD5Model* animmodel = 0;
GLObject* glAnim =0;
PhysicsObject* anim = 0;
Camera* cam;
Text* text1 =0;

size_t vert;
size_t frag;
size_t progIdIndex=0;
GLuint progId=0;
bool test = false;
bool running = true;

double old_time;
double now_time;

void Cleanup(void);
void CreateScene(void);
void DestroyCamera(void);
void DestroyShaders(void);
void update(void);
void Q(void * p);
void W (void * p);
void S (void * p);
void A (void * p);
void D (void * p);
void LM(void * p);
void Enter(void* p);
void Click(void * p);
void CollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{	
	OpenGLWindow window(hInstance, nCmdShow, L"Example 2 - MD5 Animation", L"HWL", 1900, 1080);
	openglContext = window.GetContext();

	CreateScene();
	
	window.GetContext()->setupScene();

	window.Invoke(&update);

	Cleanup();

	return 0;
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyCamera();
}

void CreateScene(void)
{
	openglContext->pmgr->SetCollisionCallback(CollisionCallback);

	cam = new Camera(openglContext, true);
	cam->setPosition(Vector3<pfd> (0,10, 0));
	cam->_frustum.setFovy(90.0);
	cam->_frustum.setFarPlane(600.0);
	openglContext->smgr->setCamera(cam);

	openglContext->KeyMap.RegisterKeyCallback(0x20, LM, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x51, Q, (void*)0);
	openglContext->KeyMap.RegisterKeyCallback(0x57, W, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x53, S, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x41, A, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x44, D, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x0D, Enter, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x01, Click, (void*)cam);

	openglContext->lmgr->addLight(Vector3<pfd>(5000,15000,5000), Vector3<pfd>(1.0,1.0,1.0), 0.7, Light::POINT);

	world = openglContext->smgr->addPhysicsObject();
	world->setNewtonObjectType(PhysicsObject::MODEL);

	OBJModel model;
	model.loadModel("Game Data\\Maps\\park.obj", true);
	model.CalculateTangents();
	size_t n = 0;
	GLObject** glObjects = GLObject::loadModel(*openglContext, model, n);

	for(size_t i=0; i < n; i++)
	{
		glWorld = glObjects[i];
		test = glWorld->loadTexture("Game Data\\Skins\\dirt.dds");
		test = glWorld->loadTexture("Game Data\\Skins\\grass6.dds");
		test = glWorld->loadTexture("Game Data\\Skins\\grass5.dds");
		glWorld->enableOctree(true, 5);
		glWorld->createObject(GL_TRIANGLES);
	}
	
	world->createObject(glObjects, n);
	
	for(size_t i=0; i < n; i++)
	{
		glWorld = glObjects[i];
		glWorld->release();
	}

	if(n > 0)
	{
		world->setWorldSizeFromBody(50, 10000, 50);
		vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,  "Game Data\\Shaders\\OBJECT_A_Di_Si_T3S_Nm_LnS_VS.glsl");
		frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_T3S_Nm_LnS_FS.glsl");
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		world->setProgram(progId, progIdIndex);
		world->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
	}

	openglContext->tmgr->Initialize("Game Data\\Skins\\font.dds");
	text1 = openglContext->tmgr->addText();
	size_t w = openglContext->windowWidth;
	size_t h = openglContext->windowHeight;
	text1->createText("FPS: ", 10, h-25, 25);
	size_t t_vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\UI_T_VS.glsl");
	size_t t_frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\UI_T_FS.glsl");
	progIdIndex = openglContext->smgr->linkProgram(t_vert, t_frag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	text1->setProgram(progId, progIdIndex);

	animmodel = new MD5Model(*openglContext);
	test = animmodel->LoadModel("Game Data\\Characters\\boblampclean.md5mesh");
	test = animmodel->LoadAnim("Game Data\\Characters\\boblampclean.md5anim");
	animmodel->CalculateTangents();
	n = 0;
	GLObject** glAnimObjects = GLAnimatedObject::loadModel(*openglContext, *animmodel, n);

	anim = openglContext->smgr->addPhysicsObject();
	anim->setNewtonObjectType(PhysicsObject::BOX);

	for(size_t i=0; i < n; i++)
	{
		glAnim = glAnimObjects[i];
		test = glAnim->loadTexture("Game Data\\Skins\\dirt.dds");
		test = glAnim->loadTexture("Game Data\\Skins\\grass6.dds");
		test = glAnim->loadTexture("Game Data\\Skins\\grass5.dds");
		glAnim->enableOctree(true, 5);
		glAnim->createObject(GL_TRIANGLES);
	}

	anim->createObject(glAnimObjects, n);
	for(size_t i=0; i < n; i++)
	{
		glAnim = glAnimObjects[i];
		glAnim->release();
	}
	
	if(n > 0)
	{
		anim->setPosition(Vector3<pfd>(0 , 10, 0));
		vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,  "Game Data\\Shaders\\MD5_A_Di_Si_Nm_Hm_LnS_VS.glsl");
		frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\MD5_A_Di_Si_Nm_Hm_LnS_FS.glsl");
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		animmodel->setProgram(progId);
		anim->setProgram(progId, progIdIndex);
		now_time = openglContext->SysClock->GetCurrentTime();
		anim->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
	}
}

void update()
{
	old_time = now_time;
	now_time = openglContext->SysClock->GetCurrentTime();
	double deltaTime = now_time - old_time;
	animmodel->Update(deltaTime);

	openglContext->KeyMap.Update();
	size_t h = openglContext->windowHeight;
	pfd fps = openglContext->getFPS();
	if(fps > 0.0f)
	{
		std::string ftos = to_string(fps);
		size_t find = ftos.find('.');
		text1->editText("FPS: " + ftos.substr(0,find), 10, h-25, 25);
	}
	Sleep(1);
}

void DestroyCamera(void)
{
	delete cam;
}

void DestroyShaders(void)
{
	openglContext->smgr->destroyShaders();
}

void Q(void * p)
{
	PostQuitMessage(0);
}

void W(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	c->setPosition(c->getPosition() + c->getDir() * 0.01);
}

void S(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	c->setPosition(c->getPosition() + c->getDir() * -0.01);
}

void A(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	c->setPosition(c->getPosition() + c->getRight() * -0.01);
}

void D(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	c->setPosition(c->getPosition() + c->getRight() * 0.01);
}

void LM(void * p)
{
	
}

void Click(void * p)
{
	
}

void Enter(void * p)
{
	
}

void CollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex)
{
	NewtonBody* body0;
	NewtonBody* body1;
	body0 = NewtonJointGetBody0(contactJoint);
	body1 = NewtonJointGetBody1(contactJoint);

	PhysicsObject* obj1 = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body0));
	PhysicsObject* obj2 = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body1));

	if (obj1 == obj2)
		return;
}