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
#ifndef __PLAYEROBJECT__
#define __PLAYEROBJECT__
#include "stdafx.h"
class PhysicsObject;
#include "PhysicsObject.h"

class Camera;
#include "Camera.h"

class PlayerObject : public PhysicsObject
{
public:
	friend class SceneManager;
	enum PlayerObjectEnum: size_t {THIRD_PERSON=0, FPS};
	virtual void CleanupCallback () {};
	virtual void ApplyForceAndTorqueCallback(pfd timestep, int threadIndex)
	{
		pfd Ixx;
		pfd Iyy;
		pfd Izz;
		pfd mass;

		NewtonBodyGetMassMatrix (_body, &mass, &Ixx, &Iyy, &Izz);
		
		_force += Vector4<pfd>((pfd)0.0, mass * PhysicsManager::GRAVITY, (pfd)0.0, (pfd)1.0);
		NewtonBodySetForce(_body, &_force._arr[0]);
		_force = Vector3<pfd>(0,0,0);
	}
	virtual void ApplyTransformCallback (const pfd* matrix, int threadIndex)
	{
		PlayerObject* obj;
		Vector4<pfd> pos(matrix[12], matrix[13], matrix[14], 1.0f);
		Quaternion<pfd> rot;
		NewtonBodyGetRotation(_body, &rot._arr[0]);
		obj = static_cast<PlayerObject*>(NewtonBodyGetUserData(_body));
		obj->setPosition(Vector3<pfd>(pos));
		//obj->setRotation(rot);
	}
	~PlayerObject(void);
	const Camera* getCamera();
	void setCamera(Camera*);
	void setCameraType(PlayerObjectEnum camType);
	virtual void createObject(std::vector<GLObject*>& glObjects);
	virtual void setRotation(const Quaternion<pfd>& q);
	virtual void setPosition(const Vector3<pfd>& pos);
	virtual std::deque<Object*>::iterator Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k);
	void setCameraPadding(pfd height);
protected:
	PlayerObject(OpenGLContext& context);
	Camera* _camera;
	PlayerObjectEnum _cameraType;
	pfd _cameraPadding;
};

#endif