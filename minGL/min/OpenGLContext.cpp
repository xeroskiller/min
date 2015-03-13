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
#include "OpenGLContext.h"
#include "SceneManager.h"
#include "LightManager.h"
#include "TextManager.h"
#include "PhysicsManager.h"
#include "TextureManager.h"
#include "SystemClock.h"

OpenGLContext::OpenGLContext(void)
{

}
OpenGLContext::OpenGLContext(HWND hwnd, SystemClock* sysClock)
{
	_hwnd = hwnd;
	SysClock = sysClock;
	createContext(hwnd);
	smgr = new SceneManager(this);
	lmgr = new LightManager(this);
	tmgr = new TextManager(this);
	pmgr = new PhysicsManager(this, sysClock);
	tlmgr = new TextureManager();
	_fps = 0;
	_old_fps = 0;
}

OpenGLContext::~OpenGLContext(void)
{
	delete tlmgr;
	delete pmgr;
	delete tmgr;
	delete lmgr;
	delete smgr;
	wglMakeCurrent(_hdc, 0);
	wglDeleteContext(_hrc);
	ReleaseDC(hwnd,_hdc);
}

bool OpenGLContext::createContext(HWND hwnd)
{
	this->hwnd = hwnd;
	_hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(_hdc, &pfd);
	if (nPixelFormat == 0)
		return false;

	BOOL bResult = SetPixelFormat(_hdc, nPixelFormat, &pfd);
	if(!bResult)
		return false;
	HGLRC tempOpenGLContext = wglCreateContext(_hdc);
	wglMakeCurrent(_hdc, tempOpenGLContext);
	GLenum error = glewInit();
	if (error != GLEW_OK)
			return false;
	int attributes[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		_hrc = wglCreateContextAttribsARB(_hdc, NULL, attributes);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(tempOpenGLContext);
		wglMakeCurrent(_hdc,_hrc);
	}
	else{
		_hrc = tempOpenGLContext;
	}
	int glVersion[2] = {NULL, NULL};
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);

	return true;
}

void OpenGLContext::setupScene(void)
{
	glClearColor(0.4f,0.6f,0.9f,0.0f);
	glClearDepth(1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	pfd middleX = static_cast<pfd>(windowWidth/2);
	pfd middleY = static_cast<pfd>(windowHeight/2);

	POINT p;
	p.x = static_cast<long>(middleX);
	p.y = static_cast<long>(middleY);
	ClientToScreen(hwnd, &p);
	SetCursorPos(p.x,p.y);
}

void OpenGLContext::reshapeWindow(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	glViewport(0,0, windowWidth, windowHeight);
}

void OpenGLContext::renderScene()
{
	smgr->updateObjects();
	if(pmgr->Solve())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		_fps = 1.0f / static_cast<pfd>(SysClock->GetElapsedTime());
		SysClock->Reset();
		smgr->drawObjects();
		tmgr->drawText();
		SwapBuffers(_hdc);
		UpdateWindow(_hwnd);
	}
	else
	{
		Sleep(1);
	}
}

float OpenGLContext::getAspectRatio()
{
	return static_cast<float>(windowWidth)/static_cast<float>(windowHeight);
}

pfd OpenGLContext::getFPS()
{
	if(_fps != _old_fps)
	{
		_old_fps = _fps;
		return _fps;
	}
	else
	{
		return 0.0f;
	}
}