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
#include "PhysicsObject.h"
#include "GLObject.h"
#include "aabbox3.h"
#include "Matrix.h"

PhysicsObject::PhysicsObject(OpenGLContext& context) : BoundedObject(context)
{
	_nWorld = 0;
	_nWorld = 0;
	_collision = 0;
	_body = 0;
	_mass=0.0;
	_newtonType = BOX;
	_height = 0;
	_width = 0;
	_depth = 0;
	_radius = 0;
	_padding = 1.0;
	_enableForceAndTorque = true;
	_ForceAndTorqueCallback = 0;
	_enableTransform = true;
	_TransformCallback = 0;
}

PhysicsObject::~PhysicsObject(void)
{
	NewtonDestroyBody(_nWorld, _body);
}

// must set mass before calling
//must see NewtonBounding type
void PhysicsObject::createObject(std::vector<GLObject*>& glObjects)
{
	if(glObjects.size() > 0)
	{
		//create BoundedObject
		this->setBoundingVolume(BoundingVolume<pfd>::AABB);
		this->BoundedObject::createObject(glObjects);
		Vector3<pfd> origin = this->_boundingVolume->getCenter();
		//create box
		if(_newtonType == BOX)
		{
			const aabbox3<pfd> * ptr = static_cast<const aabbox3<pfd>*>(this->_boundingVolume);
			Vector3<pfd> size = ptr->getExtent();
			_width = size.X();
			_height = size.Y();
			_depth = size.Z();
			_radius = sqrt(_depth*_depth + _width*_width);
			Matrix mat(origin);

			_collision = NewtonCreateBox (_nWorld, size._arr[0], size._arr[1], size._arr[2], 0, mat._mat);
		}
		//create Mesh
		else if(_newtonType == MODEL)
		{
			_collision = NewtonCreateTreeCollision(_nWorld, NULL);
			NewtonTreeCollisionBeginBuild(_collision);
			for(size_t i=0; i < _numGLObjects; i++) //this is for sub-meshes, only 1 for now
			{
				Mesh* mesh = getGLObject(i).getMesh();
				int index0, index1, index2;
				Vector3<pfd> face[3];
				const size_t IndexSize = mesh->_indices.size();
				for(size_t j=0; j< IndexSize; j+=3)
				{
					index0 = mesh->_indices[j];
					index1 = mesh->_indices[j+1];
					index2 = mesh->_indices[j+2];
					face[0] = Vector3<pfd>(mesh->_vertices[index0]);
					face[1] = Vector3<pfd>(mesh->_vertices[index1]);
					face[2] = Vector3<pfd>(mesh->_vertices[index2]);
				
					NewtonTreeCollisionAddFace(_collision, 3, &face->_arr[0], 3*sizeof(pfd), i + 1);
				}
			}
			NewtonTreeCollisionEndBuild(_collision, 1); 

			//NewtonTreeCollisionSetUserRayCastCallback(collision, UserMeshCollisionCallback); //called per face intersected by ray
		}

		//create rigid body
		Matrix matrix(this->getRotation(), this->getPosition());
		_body = NewtonCreateBody(_nWorld, _collision, matrix._mat);
		NewtonBodySetDestructorCallback (_body, NewtonCleanupCallback);
		NewtonBodySetUserData (_body, this);
		Vector4<pfd> inertia;
		NewtonConvexCollisionCalculateInertialMatrix (_collision, &inertia._arr[0], &origin._arr[0]);

		NewtonBodySetMassMatrix (_body, _mass, _mass * inertia.X(), _mass * inertia.Y(), _mass * inertia.Z());
		NewtonBodySetCentreOfMass (_body, &origin._arr[0]);

		NewtonBodySetForceAndTorqueCallback (_body, NewtonApplyForceAndTorqueCallback);
		NewtonBodySetTransformCallback (_body, NewtonApplyTransformCallback);

		//release reference
		NewtonReleaseCollision (_nWorld, _collision);
		Matrix orien(this->getRotation(), this->getPosition());
		NewtonBodySetMatrix (_body, &orien._mat[0]);

		Vector4<pfd> minBox;
		Vector4<pfd> maxBox;
		NewtonCollisionCalculateAABB (_collision, &orien._mat[0], &minBox._arr[0], &maxBox._arr[0]);
		delete _boundingVolume;
		_boundingVolume = new aabbox3<pfd>(minBox, maxBox);
	}
}

void PhysicsObject::setWorldSizeFromBody(pfd x_pad, pfd y_pad, pfd z_pad)
{
	// now we will use the properties of this body to set a proper world size.
	Vector4<pfd> minBox = static_cast<aabbox3<pfd>*>(_boundingVolume)->getMin();
	Vector4<pfd> maxBox = static_cast<aabbox3<pfd>*>(_boundingVolume)->getMax();

	// add some extra padding
	minBox._arr[0] -=  x_pad;
	minBox._arr[1] -= y_pad;
	minBox._arr[2] -=  z_pad;

	maxBox._arr[0] +=  x_pad;
	maxBox._arr[1] +=  y_pad;
	maxBox._arr[2] +=  z_pad;

	_pmgr->setMinBox(minBox);
	_pmgr->setMaxBox(maxBox);

	// set the new world size
	NewtonSetWorldSize (_nWorld, &minBox._arr[0], &maxBox._arr[0]);
}

const Vector3<pfd> PhysicsObject::getVelocity()
{
	Vector3<pfd> vel;
	NewtonBodyGetVelocity(_body, vel._arr);
	return vel;
}

void PhysicsObject::setVelocity(const Vector3<pfd>& vel)
{
	NewtonBodySetVelocity(_body, &vel._arr[0]);
}

void PhysicsObject::setAngularVelocity(const Vector3<pfd>& vel)
{
	NewtonBodySetOmega(_body, &vel._arr[0]);
}

void PhysicsObject::applyForce(const Vector3<pfd>& force)
{
	_force += force*_mass;
}

void PhysicsObject::setRotation(const Quaternion<pfd>& q)
{
	Object::setRotation(q);
	Matrix matrix(this->getRotation(), this->getPosition());
	NewtonBodySetMatrix (_body, &matrix._mat[0]);
}
void PhysicsObject::setPosition(const Vector3<pfd>& pos)
{
	Object::setPosition(pos);
	Matrix matrix(this->getRotation(), this->getPosition());
	NewtonBodySetMatrix (_body, &matrix._mat[0]);
}
void PhysicsObject::setScale(const Vector3<pfd>& scale)
{
	Object::setScale(scale);
	Matrix matrix(this->getRotation(), this->getPosition());
	NewtonBodySetMatrix (_body, &matrix._mat[0]);
	//need to recalculate the bounding volume

	Vector3<pfd> origin = this->_boundingVolume->getCenter();
	const aabbox3<pfd> * ptr = static_cast<const aabbox3<pfd>*>(this->_boundingVolume);
	Vector3<pfd> size = ptr->getExtent()*scale;
	Matrix mat(origin);
	_collision = NewtonCreateBox (_nWorld, size._arr[0], size._arr[1], size._arr[2], 0, mat._mat);
	NewtonBodySetCollision(_body, _collision);
}

std::deque<Object*>::iterator PhysicsObject::Update(Camera& cam, double ref_time, std::deque<Object*>::iterator k)
{
	return Object::Update(cam, ref_time, k);
}

pfd PhysicsObject::rayCastPlacement (const NewtonBody* body, const pfd* normal, int collisionID, void* userData, pfd intersetParam)
{
	pfd* paramPtr;
	paramPtr = (pfd*)userData;
	if (intersetParam < paramPtr[0]) {
		paramPtr[0] = intersetParam;
	}
	return paramPtr[0];
}

pfd PhysicsObject::findFloorRayCast(pfd x, pfd z)
{
	pfd parameter;
	pfd Max = _pmgr->getMaxAltitude();
	pfd Min = _pmgr->getMinAltitude();
	// shoot a vertical ray from a high altitude and collect the intersection parameter.
	Vector3<pfd> p0 (x, Max, z); 
	Vector3<pfd> p1 (x, Min, z); 

	parameter = 1.2f;
	NewtonWorldRayCast (_nWorld, &p0._arr[0], &p1._arr[0], rayCastPlacement, &parameter, NULL);
	
	//y=mx+b
	pfd b = Max;
	pfd m = Min - b;

	// the intersection is the interpolated value
	return m*parameter + b;
}

unsigned PhysicsObject::convexCastCallback (const NewtonBody* body, const NewtonCollision* collision, void* userData)
{
	// this convex cast have to skip the casting body
	NewtonBody* me = (NewtonBody*) userData;
	return (me == body) ? 0 : 1;
}

pfd PhysicsObject::findFloorConvexCast(bool& good)
{
	pfd param;
	Matrix matrix;
	NewtonWorldConvexCastReturnInfo info[16];

	// get the Body Matrix;
	NewtonBodyGetMatrix (_body, matrix._mat);

	// add some search for a floor surface withe it +20 unit up and -20 units down
	Vector3<pfd> vec_high(matrix.GetTranslation());
	pfd height = _height/2 + _padding;
	vec_high._arr[1] += _height;
	matrix.SetTranslation(vec_high);

	Vector3<pfd> vec_low = vec_high;
	vec_low._arr[1] -= 2*height;

	// cast the collision shape of the body
	_nWorld = NewtonBodyGetWorld(_body);
	NewtonWorldConvexCast (_nWorld, matrix._mat, vec_low._arr, _collision, &param, _body, convexCastCallback, info, 16, 0);

	// the point at the intersection param is the floor 
	vec_high._arr[1] += (vec_low._arr[1] - vec_high._arr[1]) * param;
	if(vec_high._arr[1] > vec_low._arr[1])
	{
		good = true;
	}
	else
	{
		good = false;
	}
	return vec_high._arr[1];
}

pfd PhysicsObject::getRadius()
{
	return _radius + _padding;
}

pfd PhysicsObject::getHeight()
{
	return _height;
}

void PhysicsObject::enableForceAndTorqueCallback(bool state)
{
	_enableForceAndTorque = state;
}

void PhysicsObject::setForceAndTorqueCallback(void (*f)(pfd timestep, int threadIndex))
{
	_ForceAndTorqueCallback = f;
}

void PhysicsObject::enableTransformCallback(bool state)
{
	_enableTransform = state;
}

void PhysicsObject::setTransformCallback(void (*f)(const pfd* matrix, int threadIndex))
{
	_TransformCallback = f;
}

NewtonBody*  PhysicsObject::getNewtonBody()
{
	return _body;
}