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
#include "MD5Model.h"
#include "OpenGLContext.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Mesh.h"
#include "minutils.h"

MD5Model::MD5Model(OpenGLContext& glContext)
{
	_glContext = &glContext;
	_loaded = false;
	_md5Version= 0;
    _numJoints= 0;
    _numMeshes = 0;
    _hasAnimation = false;
	_progId = -1;
}

MD5Model::~MD5Model()
{
}

bool MD5Model::LoadModel(const std::string& filename)
{
	 std::ifstream file (filename, std::ios::in);
	 if(file.is_open())
	 {
		 _jointBuffer.clear();
		 std::vector<Mesh*>::iterator k = _meshList.begin();
		 std::vector<Mesh*>::iterator end = _meshList.begin();
		 for(; k != end; k++)
		 {
			 delete (*k);
		 }
		 _meshList.clear();

		 std::string line;
		 std::string param;
		 std::string junk;
		 std::istringstream s;
		 while(std::getline(file, line))
		 {
			 param = "";
			 s = std::istringstream(line);
			 s >> param;
			 if(param == "")
			 {
				 continue;
			 }
			 if(param == "MD5Version")
			 {
				 s >> _md5Version;
				 assert(_md5Version == 10);
			 }
			 else if( param == "commandline")
			 {
				 continue;
			 }
			 else if( param == "numJoints")
			 {
				 s >> _numJoints;
				 _jointBuffer.reserve(_numJoints);
				 _animatedBones.assign(_numJoints, Matrix(IDENTITY_MATRIX));
			 }
			 else if( param == "numMeshes")
			 {
				 s >> _numMeshes;
				 _meshList.reserve(_numMeshes);
			 }
			 else if(param == "joints")
			 {
				 Joint joint;
				 for(size_t i=0; i < _numJoints; i++)
				 {
					 std::getline(file, line);
					 s = std::istringstream(line);
					 s >> joint._name;
					 s >> joint._parentID;
					 s >> junk;
					 s >> joint._position._arr[0];
					 s >> joint._position._arr[1];
					 s >> joint._position._arr[2];
					 s >> junk;
					 s >> junk;
					 s >> joint._rotation._arr[1];
					 s >> joint._rotation._arr[2];
					 s >> joint._rotation._arr[3];

					 std::string& name = joint._name;
					 name.erase( std::remove( name.begin(), name.end(), '\"' ), name.end());
					 joint._rotation.calculateW();
					 
					 _jointBuffer.push_back(joint);
				 }
				 BuildBindPose( _jointBuffer);
			 }
			 else if(param == "mesh")
			 {
				Mesh* mesh = new Mesh();
				size_t numVerts, numTris, numWeights;
				std::getline(file, line);
				s = std::istringstream(line);
				s >> param;
				while(param != "}")
				{
					if(param != "")
					{
						if(param == "shader")
						{
							s >> mesh->_name;
							std::string& name = mesh->_name;
							name.erase( std::remove( name.begin(), name.end(), '\"' ), name.end());

							//possibly load texture shader
						}
						else if(param == "numverts")
						{
							s >> numVerts;
							for(size_t i=0; i < numVerts; i++)
							{
								std::getline(file, line);
								s = std::istringstream(line);
								Vector2<pfd> uv;

								s >> junk; s >> junk; s >> junk;
								s >> uv._arr[0];
								s >> uv._arr[1];
								s >> junk;

								Mesh::VertexWeight vw;
								s >> vw.startWeight;
								s >> vw.weightCount;

								mesh->_vertexWeights.push_back(vw);
								mesh->_uvcoords.push_back(uv);
							}
						}
						else if( param == "numtris")
						{
							s >> numTris;
							for (size_t i=0; i < numTris; i++)
							{
								std::getline(file, line);
								s = std::istringstream(line);
								size_t index1, index2, index3;
								s >> junk; s >> junk;
								s >> index1; s >> index2; s >> index3;

								mesh->_indices.push_back(index1);
								mesh->_indices.push_back(index2);
								mesh->_indices.push_back(index3);
							}
						}
						else if( param == "numweights")
						{
							s >> numWeights;
							for (size_t i=0; i < numWeights; i++)
							{
								std::getline(file, line);
								s = std::istringstream(line);
								Mesh::Weight weight;
								s >> junk; s >> junk;
								s >> weight._jointID; s >> weight._bias; s >> junk;
								s >> weight._position._arr[0];
								s >> weight._position._arr[1];
								s >> weight._position._arr[2];
								mesh->_weights.push_back(weight);
							}
						}
						else
						{
							//do nothing
						}
					}
					std::getline(file, line);
					param = "";
					s = std::istringstream(line);
					s >> param;
				}
				PrepareMesh(*mesh);
				PrepareNormals(*mesh);
				_meshList.push_back(mesh);
			 }

		 } // while
		 assert(_jointBuffer.size() == _numJoints);
		 assert(_meshList.size() == _numMeshes);
		 _loaded = true;
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}

bool MD5Model::LoadAnim (const std::string& filename)
{
	if(_animation.LoadAnimation(filename))
	{
		_hasAnimation = CheckAnimation(_animation);
	}

	return _hasAnimation;
}

bool MD5Model::CheckAnimation( const MD5Animation& animation ) const
{
	if(_numJoints != animation.getNumJoints())
	{
		return false;
	}

	for(size_t i = 0; i < _jointBuffer.size(); i++)
	{
		const Joint& meshJoint = _jointBuffer[i];
		const MD5Animation::JointInfo& animJoint = animation.GetJointInfo(i);

		if(meshJoint._name != animJoint._name || meshJoint._parentID != animJoint._parentId)
		{
			return false;
		}
	}
	return true;
}

void MD5Model::PrepareMesh(Mesh& mesh)
{
	mesh._vertices.clear();
	mesh._boneindices.clear();
	mesh._boneweights.clear();
	const size_t size = mesh._vertexWeights.size();
	for(size_t i=0; i < size; i++)
	{
		Vector3<pfd> vert;
		Vector4<pfd> boneWeight;
		Vector4<pfd> boneIndex;
		Mesh::VertexWeight& vw = mesh._vertexWeights[i];
		
		const size_t weightCount = vw.weightCount;
		for(size_t j=0; j < weightCount; j++)
		{
			assert(j < 4); //max weights per vertex

			Mesh::Weight& weight = mesh._weights[vw.startWeight + j];
			Joint& joint = _jointBuffer[weight._jointID];

			Vector3<pfd> rotPos = joint._rotation.transform(weight._position); 
			
			vert += (joint._position + rotPos) * weight._bias;
			boneIndex._arr[j] = (pfd)weight._jointID; 
			boneWeight._arr[j] = weight._bias;
		}

		mesh._vertices.push_back(vert);
		mesh._bindposevertices.push_back(vert);
		mesh._boneindices.push_back(boneIndex);
		mesh._boneweights.push_back(boneWeight);
	}
}

void MD5Model::PrepareNormals(Mesh& mesh)
{
	mesh._bindposenormals.clear();
	mesh._normals.clear();

	const size_t vertSize = mesh._bindposevertices.size();
	mesh._bindposenormals.resize(vertSize);
	mesh._normals.resize(vertSize);
	
	for(size_t i=0; i < vertSize; i+=3)
	{
		Vector3<pfd> v0 = mesh._bindposevertices[mesh._indices[i]];
		Vector3<pfd> v1 = mesh._bindposevertices[mesh._indices[i+1]];
		Vector3<pfd> v2 = mesh._bindposevertices[mesh._indices[i+2]];

		Vector3<pfd> normal = (v2 - v0).crossProduct(v1 - v0);
		mesh._bindposenormals[mesh._indices[i]] += normal;
		mesh._bindposenormals[mesh._indices[i+1]] += normal;
		mesh._bindposenormals[mesh._indices[i+2]] += normal;
	}
	
	for(size_t i=0; i < vertSize; i++)
	{
		Vector3<pfd>& normal = mesh._bindposenormals[i];
		normal.normalize();

		mesh._normals.push_back(normal);
	}
}

//moved to shader
void MD5Model::PrepareMesh(Mesh& mesh, const std::vector<Matrix>& skel)
{
	const size_t size = mesh._vertices.size();
	for(size_t i =0; i < size; i++)
	{
		Vector4<pfd>& position = mesh._vertices[i];
		Vector3<pfd>& normal = mesh._normals[i];
		
		Vector3<pfd>& vertNormal = mesh._bindposenormals[i];
		Vector4<pfd>& vertPos    = mesh._bindposevertices[i];
		
		position = Vector4<pfd>(0,0,0,0);
		normal   = Vector3<pfd>(0,0,0);
		
		Mesh::VertexWeight& vw = mesh._vertexWeights[i];
		const size_t vwSize = vw.weightCount;
		for(size_t j=0; j < vwSize; j++)
		{
			const Mesh::Weight& weight = mesh._weights[vw.startWeight + j];
			const Matrix boneMatrix = skel[weight._jointID];

			position += Vector4<pfd>( ( boneMatrix.Transform(vertPos) ) * weight._bias );
			normal += Vector4<pfd>( ( boneMatrix.Transform(Vector4<pfd>(vertNormal, (pfd)0.0) ) ) * weight._bias );
		}
	}
}

void MD5Model::Update(pfd deltaTime)
{
	if(_hasAnimation)
	{
		_animation.Update(deltaTime);
		const MD5Animation::FrameSkeleton& skeleton = _animation.getSkeleton();
		const std::vector<Matrix>& animatedSkeleton = _animation.getSkeletonBoneMatrices();

		for(size_t i=0; i < _numJoints; i++)
		{
			_animatedBones[i] = animatedSkeleton[i];
			_animatedBones[i].MultiplyMatrices(&_InverseBindPose[i]);
		}
	}
	else
	{
		_animatedBones.assign(_numJoints, Matrix(IDENTITY_MATRIX));
	}
	setBones();
	//moved to shader
	/*const size_t meshCount = _meshList.size();
	for(size_t i=0; i < meshCount; i++)
	{
		PrepareMesh(*_meshList[i], _animatedBones); 
	}
	*/
}

void MD5Model::BuildBindPose( const std::vector<Joint>& joints )
{
	_bindPose.clear();
	_InverseBindPose.clear();

	std::vector<Joint>::const_iterator k = joints.begin();
	std::vector<Joint>::const_iterator end = joints.end();
	for( ;k != end; k++)
	{
		const Joint& joint = (*k);
		Matrix boneMatrix(IDENTITY_MATRIX);
		boneMatrix.SetTranslation(joint._position);
		boneMatrix.SetRotation(joint._rotation);

		Matrix invBoneMatrix(boneMatrix);
		bool test = invBoneMatrix.Inverse();
		assert(test);
		_bindPose.push_back(boneMatrix);
		_InverseBindPose.push_back(invBoneMatrix);
	}
}

void MD5Model::setProgram(GLuint progId)
{
	_progId = progId;
	BONEMATRIX_LOCATION = glGetUniformLocation(progId, "BoneMatrix");
	if(BONEMATRIX_LOCATION == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get BoneMatrix Uniform.");
	}
	check_glError(_glContext, L"OpenGL error", L"Error: MD5 Model could not set program.");
}

void MD5Model::setBones()
{
	glUseProgram(_progId);
	glUniformMatrix4fv(BONEMATRIX_LOCATION, _animatedBones.size(), GL_FALSE, (pfd*)&_animatedBones[0]);
	glUseProgram(0);
	check_glError(_glContext, L"OpenGL error", L"Error: MD5 Model could not set bones.");
}