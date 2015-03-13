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
#define __BUILD_XP__
#include "stdafx.h"
#include "min().h"
#include <string>

UIObject* UI_SG = 0;
GLUIObject* glUI_SG = 0;

UIObject* UI_SGP = 0;
GLUIObject* glUI_SGP = 0;
Vector2<pfd> pointer_direction(-1, 0);
pfd pointer_angle;
pfd pointer_offset = 40;
const pfd start_angle = -21.0;
const Matrix2<pfd> start_rot = Matrix2<pfd>(start_angle);
const pfd range_angle = 180.0 + (pfd)4.0*abs(start_angle);
const pfd maximum_speed = 420.0;
Vector2<pfd> pointer_start = Vector2<pfd>(1700,125);


OpenGLContext* openglContext;
PhysicsObject* world =0;
GLObject* glWorld =0;
PlayerObject* player =0;
GLObject* glPlayer =0;
GLObject* glSkybox = 0;
Object* skybox = 0;


GLObject* glTarget = 0;
GLObject* glCoin = 0;
const size_t NO_COINS = 2;
const size_t NO_TARGETS = 4;
PhysicsObject* coinBuffer[NO_COINS];
PhysicsObject* targetBuffer[NO_TARGETS];
size_t coin_pos = 0;
size_t target_pos = 0;

Vector3<pfd> target_offset(1,0,0);

GLObject* glArrow = 0;
PhysicsObject* arrow;
size_t arrowVert=0;
size_t arrowFrag=0;
const size_t NO_ARROWS = 5;
PhysicsObject* arrowBuffer[NO_ARROWS];
size_t arrow_pos = 0;

pfd score = 0;
pfd coin_spacing = 150;
pfd target_spacing = 85;

const int PLAYER_UNIQUE = 1;
const int COIN_UNIQUE = 2;
const int TARGET_UNIQUE = 3;
const int ARROW_UNIQUE = 4;

Camera* cam;
Text* text1 =0;
Text* text2 =0;
Text* text3 =0;
Text* text4 =0;
Text* text5 =0;
pfd STEP_SIZE = 1.0f;
size_t vert;
size_t frag;
size_t progIdIndex=0;
GLuint progId=0;
bool test = false;
bool running = true;
pfd max_vel = 0.0;

double timestep = 0.01667;
double timestepaccum = 0;
double timeover420 = 0;
double timeover125 = 0;
double newtime;
double oldtime;
double accumulator = 0.0;

pfd target_alternate = -1.0;
pfd targetrb = 5.0;

pfd coin_alternate = -1.0;
pfd coinrb = 5.0;

void Cleanup(void);
void CreateObj(void);
void DestroyVBO(void);
void DestroyShaders(void);
void update(void);
void Q(void * p);
void A (void * p);
void D (void * p);
void LM(void * p);
void Enter(void* p);
void Click(void * p);
void CollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{	
	OpenGLWindow window(hInstance, nCmdShow, L"OpenGL Test", L"OPENGL", 1900, 1080);
	openglContext = window.GetContext();

	CreateObj();
	
	window.GetContext()->setupScene();

	window.Invoke(&update);

	Cleanup();

	return 0;
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void CreateObj(void)
{
	openglContext->pmgr->SetCollisionCallback(CollisionCallback);

	newtime = openglContext->SysClock->GetCurrentTime();
	oldtime = newtime;

	cam = new Camera(openglContext, true);
	cam->setPosition(Vector3<pfd> (0,10, 0));
	cam->_frustum.setFovy(90.0);
	cam->_frustum.setFarPlane(600.0);
	openglContext->smgr->setCamera(cam);

	openglContext->KeyMap.RegisterKeyCallback(0x20, LM, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x51, Q, (void*)0);
	openglContext->KeyMap.RegisterKeyCallback(0x41, A, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x44, D, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x0D, Enter, (void*)cam);
	openglContext->KeyMap.RegisterKeyCallback(0x01, Click, (void*)cam);

	openglContext->lmgr->addLight(Vector3<pfd>(0,15000,5000), Vector3<pfd>(1.0,1.0,1.0), 0.7, Light::POINT);
	
	UI_SG = openglContext->smgr->addUIObject();
	glUI_SG = openglContext->smgr->addGLUIObject();
	test = glUI_SG->loadTexture("Game Data\\Skins\\UI_SG.dds");
	glUI_SG->loadRectangleDDS(325,325);
	glUI_SG->createObject(GL_TRIANGLES);
	UI_SG->createObject(glUI_SG);
	UI_SG->setPosition(pointer_start);
	glUI_SG->release();
	
	if(test)
	{
		vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\UI_SG_VS.glsl");
		frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\UI_SG_FS.glsl");
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		UI_SG->setProgram(progId, progIdIndex);
	}

	UI_SGP = openglContext->smgr->addUIObject();
	glUI_SGP = openglContext->smgr->addGLUIObject();
	test = glUI_SGP->loadTexture("Game Data\\Skins\\UI_SGP.dds");
	glUI_SGP->loadRectangleDDS(165,15);
	glUI_SGP->createObject(GL_TRIANGLES);
	UI_SGP->createObject(glUI_SGP);
	UI_SGP->setPosition(pointer_start);
	glUI_SGP->release();

	if(test)
	{
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		UI_SGP->setProgram(progId, progIdIndex);
	}

	world = openglContext->smgr->addPhysicsObject();
	world->setNewtonObjectType(PhysicsObject::MODEL);

	
	OBJModel model;
	model.loadModel("Game Data\\Maps\\level1a.obj", true);
	model.CalculateTangents();

	size_t n = 0;
	GLObject** glObjects = GLObject::loadModel(*openglContext, model, n);

	for (size_t i = 0; i < n; i++)
	{
		glWorld = glObjects[i];
		test = glWorld->loadTexture("Game Data\\Skins\\level1a.dds");
		test = glWorld->loadTexture("Game Data\\Skins\\level1a_NRM.dds");
		test = glWorld->loadTexture("Game Data\\Skins\\level1a_DISP.dds");
		glWorld->enableOctree(true, 5);
		glWorld->createObject(GL_TRIANGLES);
	}

	world->createObject(glObjects, n);

	//You can save into yage format after loading any glObject
	//glWorld->saveYageObj("Game Data\\Maps\\level1a.yage", true);

	for (size_t i = 0; i < n; i++)
	{
		glWorld = glObjects[i];
		glWorld->release();
	}

	/*
	//if loading from yage file format
	glWorld = openglContext->smgr->addGLObject();
	glWorld->loadYageObj("Game Data\\Maps\\level1a.yage");
	test = glWorld->loadTexture("Game Data\\Skins\\level1a.dds");
	test = glWorld->loadTexture("Game Data\\Skins\\level1a_NRM.dds");
	test = glWorld->loadTexture("Game Data\\Skins\\level1a_DISP.dds");
	glWorld->enableOctree(true, 5);
	glWorld->createObject(GL_TRIANGLES);
	size_t n = 1;
	world->createObject(&glWorld, n);
	*/
	if(test)
	{
		world->setWorldSizeFromBody(50, 10000, 50);
		vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_VS.glsl");
		frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_FS.glsl");
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		world->setProgram(progId, progIdIndex);
		world->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
	}

	player = openglContext->smgr->addPlayerObject();
	player->UniqueId = PLAYER_UNIQUE;
	player->setCamera(cam);
	player->setMass(100.0f);
	player->setNewtonObjectType(PhysicsObject::BOX);
	player->setCameraType(PlayerObject::THIRD_PERSON);

	OBJModel model2;
	model2.loadModel("Game Data\\Characters\\player.obj", true);
	model2.CalculateTangents();

	GLObject** glObjects2 = GLObject::loadModel(*openglContext, model2, n);

	for (size_t i = 0; i < n; i++)
	{
		glPlayer = glObjects2[i];
		test = glPlayer->loadTexture("Game Data\\Skins\\player.dds");
		test = glPlayer->loadTexture("Game Data\\Skins\\player_NRM.dds");
		test = glPlayer->loadTexture("Game Data\\Skins\\player_DISP.dds");
		glPlayer->createObject(GL_TRIANGLES);
	}
	player->createObject(glObjects2, n);
	
	for (size_t i = 0; i < n; i++)
	{
		glPlayer = glObjects2[i];
		glPlayer->release();
	}

	if(test)
	{
		player->setCameraPadding(player->getHeight());
		player->setPosition(Vector3<pfd>(0, 2, 0));
		progIdIndex = openglContext->smgr->linkProgram(vert, frag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		player->setProgram(progId, progIdIndex);
		player->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
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

	text2 = openglContext->tmgr->addText();
	w = openglContext->windowWidth;
	h = openglContext->windowHeight;
	text2->createText("Speed: ", 10, h-50, 25);
	progIdIndex = openglContext->smgr->linkProgram(t_vert, t_frag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	text2->setProgram(progId, progIdIndex);

	text3 = openglContext->tmgr->addText();
	w = openglContext->windowWidth;
	h = openglContext->windowHeight;
	text3->createText("Max Speed: ", 10, h-75, 25);
	progIdIndex = openglContext->smgr->linkProgram(t_vert, t_frag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	text3->setProgram(progId, progIdIndex);

	text4 = openglContext->tmgr->addText();
	w = openglContext->windowWidth;
	h = openglContext->windowHeight;
	text4->createText("X:, Y:, Z:, ", 10, h-100, 25);
	progIdIndex = openglContext->smgr->linkProgram(t_vert, t_frag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	text4->setProgram(progId, progIdIndex);

	text5 = openglContext->tmgr->addText();
	w = openglContext->windowWidth;
	h = openglContext->windowHeight;
	text5->createText("Score: ", 540, h-200, 75);
	progIdIndex = openglContext->smgr->linkProgram(t_vert, t_frag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	text5->setProgram(progId, progIdIndex);

	OBJModel model3;
	model3.loadModel("Game Data\\Characters\\coin.obj", true);
	model3.CalculateTangents();

	GLObject** glObjects3 = GLObject::loadModel(*openglContext, model3, n);

	for (size_t i = 0; i < n; i++)
	{
		glCoin = glObjects3[i];
		test = glCoin->loadTexture("Game Data\\Skins\\coin.dds");
		test = glCoin->loadTexture("Game Data\\Skins\\coin_NRM.dds");
		test = glCoin->loadTexture("Game Data\\Skins\\coin_DISP.dds");
		glCoin->createObject(GL_TRIANGLES);
	}

	for(size_t i=0; i<NO_COINS; i++)
	{
		coinBuffer[i] = openglContext->smgr->addPhysicsObject();
		coinBuffer[i]->UniqueId = COIN_UNIQUE;
		Quaternion<pfd> q(Vector3<pfd>(1,0,0),Vector3<pfd>(0,0,1));
		coinBuffer[i]->Object::rotateObj(q);
		coinBuffer[i]->setMass(0.10f);
		coinBuffer[i]->setNewtonObjectType(PhysicsObject::BOX);
		coinBuffer[i]->createObject(glObjects3, n);

		if(test)
		{
			coinBuffer[i]->enableForceAndTorqueCallback(false);
			coinBuffer[i]->enableTransformCallback(false);
			pfd floor = coinBuffer[i]->findFloorRayCast(0,-coin_spacing*i);
			coinBuffer[i]->setPosition(Vector3<pfd>(coinrb*coin_alternate, floor + coinBuffer[i]->getHeight(), -coin_spacing*i));
			coin_alternate++;
			if(coin_alternate == 2)
				coin_alternate = -1;
			vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_VS.glsl");
			frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_FS.glsl");
			progIdIndex = openglContext->smgr->linkProgram(vert, frag);
			progId = openglContext->smgr->getProgramId(progIdIndex);
			coinBuffer[i]->setProgram(progId, progIdIndex);
			coinBuffer[i]->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
		}
		coin_pos++;
	}

	for (size_t i = 0; i < n; i++)
	{
		glCoin = glObjects3[i];
		glCoin->release();
	}

	OBJModel model4;
	model4.loadModel("Game Data\\Characters\\target.obj", true);
	model4.CalculateTangents();

	GLObject** glObjects4 = GLObject::loadModel(*openglContext, model4, n);

	for (size_t i = 0; i < n; i++)
	{
		glTarget = glObjects4[i];
		test = glTarget->loadTexture("Game Data\\Skins\\target.dds");
		test = glTarget->loadTexture("Game Data\\Skins\\target_NRM.dds");
		test = glTarget->loadTexture("Game Data\\Skins\\target_DISP.dds");
		glTarget->createObject(GL_TRIANGLES);
	}

	for(size_t i=0; i<NO_TARGETS; i++)
	{
		targetBuffer[i] = openglContext->smgr->addPhysicsObject();
		targetBuffer[i]->UniqueId = TARGET_UNIQUE;
		Quaternion<pfd> q(Vector3<pfd>(1,0,0),Vector3<pfd>(0,0,1));
		targetBuffer[i]->Object::rotateObj(q);
		targetBuffer[i]->setMass(0.10f);
		targetBuffer[i]->setNewtonObjectType(PhysicsObject::BOX);
		targetBuffer[i]->createObject(glObjects4, n);

		if(test)
		{
			targetBuffer[i]->enableForceAndTorqueCallback(false);
			targetBuffer[i]->enableTransformCallback(false);
			pfd floor = targetBuffer[i]->findFloorRayCast(0,-target_spacing*i);
			targetBuffer[i]->setPosition(Vector3<pfd>(targetrb*target_alternate, floor + 25, -target_spacing*target_pos));
			target_alternate++;
			if(target_alternate == 2)
				target_alternate = -1;
			target_pos++;
			vert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_VS.glsl");
			frag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_Nm_SHm_LnS_FS.glsl");
			progIdIndex = openglContext->smgr->linkProgram(vert, frag);
			progId = openglContext->smgr->getProgramId(progIdIndex);
			targetBuffer[i]->setProgram(progId, progIdIndex);
			targetBuffer[i]->setMaterialProperties(Vector3<pfd>(0.3f, 1.0, 0.25));
		}
		target_pos++;
	}

	for (size_t i = 0; i < n; i++)
	{
		glTarget = glObjects4[i];
		glTarget->release();
	}

	OBJModel model5;
	model5.loadModel("Game Data\\Characters\\arrow.obj", true);

	GLObject** glObjects5 = GLObject::loadModel(*openglContext, model5, n);

	for (size_t i = 0; i < n; i++)
	{
		glArrow = glObjects5[i];
		test = glArrow->loadTexture("Game Data\\Skins\\arrow_128.dds");
		glArrow->createObject(GL_TRIANGLES);
	}

	arrowVert = openglContext->smgr->loadShader(GL_VERTEX_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_LnS_VS.glsl");
	arrowFrag = openglContext->smgr->loadShader(GL_FRAGMENT_SHADER,"Game Data\\Shaders\\OBJECT_A_Di_Si_LnS_FS.glsl");

	for(size_t i=0; i<NO_ARROWS; i++)
	{
		arrowBuffer[i] = openglContext->smgr->addPhysicsObject();
		arrowBuffer[i]->UniqueId = ARROW_UNIQUE;
		arrowBuffer[i]->enableForceAndTorqueCallback(true);
		arrowBuffer[i]->enableTransformCallback(true);
		arrowBuffer[i]->setMass(5.0f);
		arrowBuffer[i]->setNewtonObjectType(PhysicsObject::BOX);
		arrowBuffer[i]->createObject(glObjects5, n);
	
		arrowBuffer[i]->setPosition(Vector3<pfd>(-1000,-1000,-1000));

		progIdIndex = openglContext->smgr->linkProgram(arrowVert, arrowFrag);
		progId = openglContext->smgr->getProgramId(progIdIndex);
		arrowBuffer[i]->setProgram(progId, progIdIndex);
	}

	for (size_t i = 0; i < n; i++)
	{
		glArrow = glObjects5[i];
		glArrow->release();
	}

	skybox = openglContext->smgr->addObject();
	OBJModel model6;
	model6.loadModel("Game Data\\Maps\\skybox.obj", true);
	GLObject** glObjects6 = GLObject::loadModel(*openglContext, model6, n);
	for (size_t i = 0; i < n; i++)
	{
		glSkybox = glObjects6[i];
		test = glSkybox->loadTexture("Game Data\\Skins\\skybox.dds");
		glSkybox->createObject(GL_TRIANGLES);
	}
	skybox->createObject(glObjects6, n);

	skybox->setPosition(player->getPosition());
	progIdIndex = openglContext->smgr->linkProgram(arrowVert, arrowFrag);
	progId = openglContext->smgr->getProgramId(progIdIndex);
	skybox->setProgram(progId, progIdIndex);

	for (size_t i = 0; i < n; i++)
	{
		glSkybox = glObjects6[i];
		glSkybox->release();
	}
}

void update()
{
	openglContext->KeyMap.Update();
	if(max_vel > 0)
		max_vel -= timestep/10;
	if(max_vel > 800.0)
	{
		timeover420 += timestep + timeover125;
		timeover125 += timestep;
		timestepaccum = (max_vel/7500.0)*timestep;
		timestepaccum += 0.01 * max_vel* timestep*timeover420;
		max_vel -= timestep + timestepaccum;
	}
	else if(max_vel > 420.0)
	{
		timeover420 += timestep + timeover125;
		timeover125 += timestep;
		timestepaccum = (max_vel/7500.0)*timestep;
		timestepaccum += 0.001 * max_vel* timestep*timeover420;
		max_vel -= timestep + timestepaccum;
	}
	else if(max_vel > 125)
	{
		timeover420 = 0;
		timeover125 += timestep;
		timestepaccum = (max_vel/7500.0)*timestep;
		timestepaccum += 0.0001 * max_vel* timestep*timeover125;
		max_vel -= timestep + timestepaccum;
	}
	else
	{
		timeover420 = 0;
		timeover125 = 0;
	}
	
	size_t h = openglContext->windowHeight;
	pfd fps = openglContext->getFPS();
	if(fps > 0.0f)
	{
		std::string ftos = to_string(fps);
		size_t find = ftos.find('.');
		text1->editText("FPS: " + ftos.substr(0,find), 10, h-25, 25);
	}
	Vector3<pfd> vel = player->getVelocity();
	pfd vmag = vel.magnitude();
	std::string speed = std::to_string(static_cast<double>(vmag));
	size_t find = speed.find('.');
	speed = speed.substr(0, find);
	text2->editText("Speed: " + speed, 10, h-50, 25);
	std::string max = std::to_string(max_vel);
	find = max.find('.');
	max = max.substr(0, find);
	text3->editText("Max Speed: " + max, 10, h-75, 25);
	std::string spos = "X: ";
	Vector3<pfd> pos = player->getPosition();
	std::string len = to_string(pos.X());
	find = len.find('.');
	len = len.substr(0 , find);
	spos += len;
	spos += " Y: ";

	len = to_string(pos.Y());
	find = len.find('.');
	len = len.substr(0 , find);
	spos += len;
	spos += " Z: ";

	len = to_string(pos.Z());
	find = len.find('.');
	len = len.substr(0 , find);
	spos += len;

	text4->editText(spos, 10, h-100, 25);
	Vector3<pfd> move(0, 0, -1);

	std::string sscore = std::to_string(score);
	find = sscore.find('.');
	sscore = sscore.substr(0, find);
	text5->editText("Score: " + sscore, 540, h-200, 75);

	newtime = openglContext->SysClock->GetCurrentTime();
	double delta = newtime - oldtime;
	oldtime = newtime;

	if(vmag < max_vel)
	{
		pfd strength = 6.5;
		pfd m = -strength/max_vel;
		pfd b = strength;
		strength = m*vmag + b;
		if(strength > 0)
		{
			accumulator += delta;
			while(accumulator > timestep)
			{
				player->applyForce(move*max_vel*strength);
				accumulator -= timestep;
			}
		}
	}

	for(size_t i=0; i<NO_COINS; i++)
	{
		if( (player->getPosition().Z() - coinBuffer[i]->getPosition().Z()) < 0)
		{
			pfd floor = coinBuffer[i]->findFloorRayCast(0,-coin_spacing*coin_pos);
			coinBuffer[i]->setPosition(Vector3<pfd>(coinrb*coin_alternate, floor + coinBuffer[i]->getHeight(), -coin_spacing*coin_pos));
			coin_alternate++;
			if(coin_alternate == 2)
				coin_alternate = -1;
			coin_pos++;
		}
	}

	for(size_t i=0; i<NO_TARGETS; i++)
	{
		if( (player->getPosition().Z() - targetBuffer[i]->getPosition().Z()) < 0)
		{
			pfd floor = targetBuffer[i]->findFloorRayCast(0,-target_spacing*target_pos);
			Vector3<pfd> player_floor = player->getPosition();
			if(player_floor.Y() > floor + 25)
			{
				targetBuffer[i]->setPosition(Vector3<pfd>(targetrb*target_alternate, floor + 25, -target_spacing*target_pos));
				target_alternate++;
				if(target_alternate == 2)
					target_alternate = -1;
			}
			else
			{
				targetBuffer[i]->setPosition(Vector3<pfd>(targetrb*target_alternate, floor + 25, -target_spacing*target_pos));
				target_alternate++;
				if(target_alternate == 2)
					target_alternate = -1;
			}
			target_pos++;
		}
	}

	pointer_angle = start_angle + (range_angle/maximum_speed)*vmag;
	Matrix2<pfd> rot = start_rot;
	Matrix2<pfd> rotation = Matrix2<pfd>((pfd)pointer_angle);
	rot.MultiplyMatrices(&rotation);
	UI_SGP->setRotation(rot);
	Vector2<pfd> newdir = rot.MultiplyVector(pointer_direction);
	UI_SGP->setPosition(newdir*pointer_offset + pointer_start);

	skybox->setPosition(player->getPosition());
	skybox->setMaterialProperties(Vector3<pfd>(0.5f, 0.0, 0.0));
}

void DestroyVBO(void)
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

void A(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	Vector3<pfd> vel = player->getVelocity();
	vel._arr[0] = -1.0 * 17.50;
	player->setVelocity(vel);
}

void D(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	Vector3<pfd> vel = player->getVelocity();
	vel._arr[0] = 17.50;
	player->setVelocity(vel);
}

void LM(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	player->applyForce(Vector3<pfd>(0,200,0) + Vector3<pfd>(0,0,-1)*max_vel);
}

void Click(void * p)
{
	Camera*c = static_cast<Camera*>(p);
	Vector3<pfd> dir = c->getDir();
	Quaternion<pfd> q(Vector3<pfd>(1,0,0),dir);
	arrowBuffer[arrow_pos]->setVelocity(Vector3<pfd>(0,0,0));
	arrowBuffer[arrow_pos]->setAngularVelocity(Vector3<pfd>(0,0,0));
	arrowBuffer[arrow_pos]->setRotation(q);
	
	arrowBuffer[arrow_pos]->setPosition(player->getPosition() + c->getDir()*(player->getRadius() + arrowBuffer[arrow_pos]->getRadius()));
	pfd player_vel = player->getVelocity().magnitude();
	arrowBuffer[arrow_pos]->setVelocity(dir*(100.0 + player_vel));
	arrow_pos++;
	if(arrow_pos == NO_ARROWS)
	{
		arrow_pos = 0;
	}
}

void Enter(void * p)
{
	coin_pos = 0;
	score = 0;
	target_pos = 0;
	timeover125 = 0;
	timeover420 = 0;
	for(size_t i=0; i<NO_COINS; i++)
	{
		pfd floor = coinBuffer[i]->findFloorRayCast(0,-coin_spacing*coin_pos);
		coinBuffer[i]->setPosition(Vector3<pfd>(coinrb*coin_alternate, floor + coinBuffer[i]->getHeight(), -coin_spacing*coin_pos));
		coin_pos++;
	}
	
	for(size_t i=0; i<NO_TARGETS; i++)
	{
		pfd floor = targetBuffer[i]->findFloorRayCast(0,-target_spacing*target_pos);
		targetBuffer[i]->setPosition(Vector3<pfd>(targetrb*target_alternate, floor + 25, -target_spacing*target_pos));
		target_alternate++;
		if(target_alternate == 2)
			target_alternate = -1;
		target_pos++;
	}
	
	max_vel = 40.0f;
	player->setVelocity(Vector3<pfd>(0,0,-1)*max_vel);
	player->setPosition(Vector3<pfd>(0,2,0));
	Camera*c = static_cast<Camera*>(p);
	c->lookAt(Vector3<pfd>(0,0,-1));
}

void CollisionCallback(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex)
{
	NewtonBody* body0;
	NewtonBody* body1;
	body0 = NewtonJointGetBody0(contactJoint);
	body1 = NewtonJointGetBody1(contactJoint);

	PhysicsObject* obj1 = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body0));
	PhysicsObject* obj2 = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body1));

	if(obj1 == obj2)
		return;
	if(obj1->UniqueId == PLAYER_UNIQUE && obj2->UniqueId == COIN_UNIQUE)
	{
		pfd floor = obj2->findFloorRayCast(0,-100.0*coin_pos);
		obj2->setPosition(Vector3<pfd>(0, floor + obj2->getHeight(), -100.0*coin_pos));
		coin_pos++;
		score += 100;
		max_vel += 100.0;
		player->applyForce(Vector3<pfd>(0,0,-1)*20000);
	}
	else if(obj2->UniqueId == PLAYER_UNIQUE && obj1->UniqueId == COIN_UNIQUE)
	{
		pfd floor = obj1->findFloorRayCast(0,-100.0*coin_pos);
		obj1->setPosition(Vector3<pfd>(0, floor + obj1->getHeight(), -100.0*coin_pos));
		coin_pos++;
		score += 100;
		max_vel += 100.0;
		player->applyForce(Vector3<pfd>(0,0,-1)*20000);
	}
	else if(obj1->UniqueId == ARROW_UNIQUE && obj2->UniqueId == TARGET_UNIQUE)
	{
		pfd floor = obj2->findFloorRayCast(0,-target_spacing*target_pos);
		if(target_alternate)
		{
			obj2->setPosition(Vector3<pfd>(5.0, floor + 25, -target_spacing*target_pos));
			target_alternate = false;
		}
		else
		{
			obj2->setPosition(Vector3<pfd>(-5.0, floor + 25, -target_spacing*target_pos));
			target_alternate = true;
		}
		target_pos++;
		score += 10;
		max_vel += 30.0;
		player->applyForce(Vector3<pfd>(0,0,-1)*10000);
	}
	else if(obj2->UniqueId == ARROW_UNIQUE && obj1->UniqueId == TARGET_UNIQUE)
	{
		pfd floor = obj1->findFloorRayCast(0,-target_spacing*target_pos);
		if(target_alternate)
		{
			obj1->setPosition(Vector3<pfd>(5.0, floor + 25, -target_spacing*target_pos));
			target_alternate = false;
		}
		else
		{
			obj1->setPosition(Vector3<pfd>(-5.0, floor + 25, -target_spacing*target_pos));
			target_alternate = true;
		}
		target_pos++;
		score += 10;
		max_vel += 50.0;
		player->applyForce(Vector3<pfd>(0,0,-1)*10000);
	}
}