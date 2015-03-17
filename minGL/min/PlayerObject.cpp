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
#include "PlayerObject.h"
#include "aabbox3.h"

PlayerObject::PlayerObject(OpenGLContext& context): PhysicsObject(context)
{
	_camera = 0;
	_cameraType = FPS;
	_cameraPadding = _padding;
}

PlayerObject::~PlayerObject(void)
{

}

void PlayerObject::setCameraType(PlayerObjectEnum camType)
{
	_cameraType = camType;
}

//must set camera before creation AND physicsobj stuff
void PlayerObject::createObject(std::vector<GLObject*>& glObjects)
{
	if(glObjects.size() > 0)
	{
		this->PhysicsObject::createObject(glObjects);
		NewtonBodySetUserData (_body, this);
	}
}

void PlayerObject::setCamera(Camera* cam)
{
	_camera = cam;
	_camera->setPosition(_position);
}

void PlayerObject::setCameraPadding(pfd height)
{
	_cameraPadding = height;
}

void PlayerObject::setRotation(const Quaternion<pfd>& q)
{
	PhysicsObject::setRotation(q);
}

void PlayerObject::setPosition(const Vector3<pfd>& pos)
{
	bool test;
	pfd floor = findFloorConvexCast(test);
	if(test == true && (pos.Y() + _height + _padding) < floor)
	{
		PhysicsObject::setPosition(Vector3<pfd>(pos.X(), floor + _padding, pos.Z()));
	}
	else
	{
		PhysicsObject::setPosition(pos);
	}
	//update camera
	if (_camera != 0)
	{
		if (_cameraType == THIRD_PERSON)
		{
			Vector3<pfd> adjust = _camera->getDir()*((pfd)2.0*_radius);
			_camera->setPosition(pos + Vector3<pfd>(-adjust.X(), _height / 2 + _cameraPadding, -adjust.Z()));
		}
		else if (_cameraType == FPS)
		{
			_camera->setPosition(pos + Vector3<pfd>(0, _height / 2, 0));
		}
	}
}

std::deque<Object*>::iterator PlayerObject::Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k)
{
	pfd maxAltitude = _pmgr->getMaxAltitude();
	if(_position.Y() > maxAltitude)
	{
		PlayerObject::setPosition(Vector3<pfd>(_position.X(), maxAltitude - _padding, _position.Z()));
	}
	
	return PhysicsObject::Update(cam, ref_time, k);
}

const Camera* PlayerObject::getCamera()
{
	return _camera;
}