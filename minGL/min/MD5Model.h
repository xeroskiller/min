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
#ifndef __MD5MODEL__
#define __MD5MODEL__

class OpenGLContext;

#include "model.h"
#include "Vector3.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "MD5Animation.h"
#include "Matrix.h"

class MD5Model : public Model
{
public:
	struct Joint
    {
        std::string _name;
        int _parentID;
        Vector3<pfd> _position;
        Quaternion<pfd> _rotation;
    };

	MD5Model(OpenGLContext& glContext);
	~MD5Model();
	bool LoadModel(const std::string& filename);
	bool LoadAnim (const std::string& filename);
	void setProgram(GLuint progId);
	void Update(pfd deltaTime);
protected:
	void PrepareMesh(Mesh& mesh);
	void PrepareMesh(Mesh& mesh, const std::vector<Matrix>& skel); //deprecated
	void PrepareNormals(Mesh& mesh );
	bool CheckAnimation( const MD5Animation& animation ) const;
	void BuildBindPose( const std::vector<Joint>& joints );
	void setBones();

	size_t _md5Version;
    size_t _numJoints;
    bool _hasAnimation;
	MD5Animation _animation;
	std::vector<Joint> _jointBuffer;
	std::vector<Matrix> _bindPose;
	std::vector<Matrix> _InverseBindPose;
	std::vector<Matrix> _animatedBones;
	
	GLuint _progId;
	GLuint BONEMATRIX_LOCATION;
	OpenGLContext* _glContext;
};

#endif