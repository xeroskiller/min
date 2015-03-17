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
#ifndef __PHYSICSOBJECT__
#define __PHYSICSOBJECT__
#include "stdafx.h"
#include "BoundedObject.h"
class PhysicsManager;
#include "PhysicsManager.h"
template <class T> class Quaternion;
#include "Quaternion.h"
class Mesh;
#include "Mesh.h"
class glLocationManager;

class PhysicsObject : public BoundedObject
{
public:
	friend class SceneManager;
	friend class PhysicsManager;
	enum PhysicsObjectEnum: size_t {BOX=0, MODEL};
	friend void NewtonCleanupCallback (const NewtonBody* body)
	{
		PhysicsObject* ptr = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body));
		ptr->CleanupCallback();
	}
	friend void NewtonApplyForceAndTorqueCallback(const NewtonBody* body, pfd timestep, int threadIndex)
	{
		PhysicsObject* ptr = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body));
		if(ptr->_enableForceAndTorque)
		{
			if(ptr->_ForceAndTorqueCallback)
			{
				ptr->_ForceAndTorqueCallback(timestep, threadIndex);
			}
			else
			{
				ptr->ApplyForceAndTorqueCallback(timestep, threadIndex);
			}
		}
	}
	friend void NewtonApplyTransformCallback (const NewtonBody* body, const pfd* matrix, int threadIndex)
	{
		PhysicsObject* ptr = static_cast<PhysicsObject*>(NewtonBodyGetUserData(body));
		if(ptr->_enableTransform)
		{
			if(ptr->_TransformCallback)
			{
				ptr->_TransformCallback(matrix, threadIndex);
			}
			else
			{
				ptr->ApplyTransformCallback(matrix, threadIndex);
			}
		}
	}
	virtual void CleanupCallback () {};
	void enableForceAndTorqueCallback(bool state);
	void setForceAndTorqueCallback(void (*f)(pfd timestep, int threadIndex));
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
	void enableTransformCallback(bool state);
	void setTransformCallback(void (*f)(const pfd* matrix, int threadIndex));
	virtual void ApplyTransformCallback (const pfd* matrix, int threadIndex)
	{
		PhysicsObject* obj;
		Vector4<pfd> pos(matrix[12], matrix[13], matrix[14], 1.0f);
		Quaternion<pfd> rot;
		NewtonBodyGetRotation(_body, &rot._arr[0]);
		obj = static_cast<PhysicsObject*>(NewtonBodyGetUserData(_body));
		obj->setPosition(Vector3<pfd>(pos));
		obj->setRotation(rot);
	}
	~PhysicsObject();
	virtual void createObject(std::vector<GLObject*>& glObjects);
	virtual void setRotation(const Quaternion<pfd>& q);
	virtual void setPosition(const Vector3<pfd>& pos);
	virtual std::deque<Object*>::iterator Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k);
	const Vector3<pfd> getVelocity();
	void setVelocity(const Vector3<pfd>& vel);
	void setAngularVelocity(const Vector3<pfd>& vel);
	void applyForce(const Vector3<pfd>& force);
	inline void setMass(pfd mass) { _mass = mass;}; // Call before create method
	inline void setNewtonObjectType(PhysicsObjectEnum type) {_newtonType = type;}; // Call before create method
	void setWorldSizeFromBody(pfd x_pad, pfd y_pad, pfd z_pad);
	void setScale(const Vector3<pfd>& scale);
	static pfd rayCastPlacement (const NewtonBody* body, const pfd* normal, int collisionID, void* userData, pfd intersetParam);
	pfd findFloorRayCast(pfd x, pfd z);
	static unsigned convexCastCallback (const NewtonBody* body, const NewtonCollision* collision, void* userData);
	pfd findFloorConvexCast(bool& good);
	pfd getRadius();
	pfd getHeight();
	NewtonBody* getNewtonBody();
	int UniqueId;
protected:
	PhysicsObject(OpenGLContext& context);
	PhysicsManager* _pmgr;
	NewtonWorld* _nWorld;
	NewtonCollision* _collision;
	NewtonBody* _body;
	pfd _mass;
	Vector4<pfd> _force;
	PhysicsObjectEnum _newtonType;
	pfd _height;
	pfd _width;
	pfd _depth;
	pfd _radius;
	pfd _padding;
	bool _enableForceAndTorque;
	void (*_ForceAndTorqueCallback)(pfd timestep, int threadIndex);
	bool _enableTransform;
	void (*_TransformCallback)(const pfd* matrix, int threadIndex);
};

#endif