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
#include "PhysicsManager.h"
#include "PhysicsObject.h"
#include "OpenGLContext.h"
#include "SystemClock.h"

pfd PhysicsManager::FPS = (pfd)300.0;
double PhysicsManager::TIME_STEP = ((pfd)1.0/FPS);
pfd PhysicsManager::GRAVITY = -100.0f;

PhysicsManager::PhysicsManager(OpenGLContext* glContext, SystemClock* sysClock)
{
	_sysClock = sysClock;
	_timeAccumulator = 0;
	_currentTime = GetTimeInSeconds();
	NewtonSetMemorySystem (NULL, NULL);
	_world = NewtonCreate();

	// use the standard x87 floating point model  
	NewtonSetPlatformArchitecture (_world, 0);

	// set a fix world size
	_minBox = Vector4<pfd>(-500.0f, -500.0f, -500.0f, 1.0f);
	_maxBox = Vector4<pfd>( 500.0f,  500.0f,  500.0f, 1.0f);
	NewtonSetWorldSize (_world, _minBox._arr, _maxBox._arr); 

	_context = glContext;
}


PhysicsManager::~PhysicsManager()
{
	NewtonDestroyAllBodies (_world);
	// finally destroy the newton world 
	NewtonDestroy (_world);
}

double	PhysicsManager::GetTimeInSeconds()
{
	return _sysClock->GetCurrentTime();
}

bool PhysicsManager::Solve()
{
	bool solved = false;
	// do the physics simulation here
	double timeInSecounds = GetTimeInSeconds();
	double deltaTime;

	// get the time step
	deltaTime = timeInSecounds - _currentTime;
	_currentTime = timeInSecounds;
	_timeAccumulator += deltaTime;

	_physicTime = 0;
	// advance the simulation at a fix step
	while (_timeAccumulator >= TIME_STEP)
	{
		// sample time before the Update
		_physicTime = GetTimeInSeconds();

		// run the newton update function
		NewtonUpdate (_world, (1.0f/FPS));

		// calculate the time spent in the physical Simulation
		_physicTime = GetTimeInSeconds() - _physicTime;

		// subtract time from time accumulator
		_timeAccumulator -= TIME_STEP;
		solved = true;
	}	
	return solved;
}

void PhysicsManager::setWorld(PhysicsObject* po)
{
	po->_nWorld = _world;
	po->_pmgr = this;
}

void PhysicsManager::setMinBox(const Vector4<pfd>& min)
{
	_minBox = min;
}

void PhysicsManager::setMaxBox(const Vector4<pfd>& max)
{
	_maxBox = max;
}

pfd PhysicsManager::getMaxAltitude()
{
	return _maxBox._arr[1];
}

pfd PhysicsManager::getMinAltitude()
{
	return _minBox._arr[1];
}

void PhysicsManager::SetCollisionCallback(void (*f)(const NewtonJoint* contactJoint, dFloat timestep, int threadIndex))
{
	_collisionCallback = f;
	int defaultMaterialId = NewtonMaterialGetDefaultGroupID (_world);
	NewtonMaterialSetCollisionCallback (_world, defaultMaterialId, defaultMaterialId, NULL, NULL, _collisionCallback);
}