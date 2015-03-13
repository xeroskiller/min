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
#ifndef __MD5ANIMATION__
#define __MD5ANIMATION__

#include "Vector3.h"
#include "Matrix.h"
#include "Quaternion.h"
#include <string>
#include <vector>

class MD5Animation
{
public:
	struct JointInfo
	{
		std::string _name;
		int _parentId;
		int _flag;
		size_t _startIndex;
	};
	struct Bound
	{
		Vector3<pfd> _min;
		Vector3<pfd> _max;
	};
	struct BaseFrame
	{
		Vector3<pfd> _position;
		Vector3<pfd> _rotation;
	};
	struct FrameData
	{
		int _frameId;
		std::vector<pfd> _frameData;
	};
	struct SkeletonJoint
	{
		SkeletonJoint() : _parent(-1)
		{}
		SkeletonJoint( const BaseFrame& copy) : _position(copy._position), _rotation(copy._rotation)
		{}
		int _parent;
		Vector3<pfd> _position;
		Quaternion<pfd> _rotation;
	};

	struct FrameSkeleton //consider using a pointer
	{
		std::vector<Matrix> _boneMatrices;
		std::vector<SkeletonJoint> _joints;
	};

	typedef std::vector<FrameSkeleton> FrameSkeletonList;
	typedef std::vector<JointInfo> JointInfoList;
	typedef std::vector<BaseFrame> BaseFrameList; 

	MD5Animation();
	~MD5Animation();
	bool LoadAnimation(const std::string& filename);
	void Update(pfd deltaTime);
	const FrameSkeleton& getSkeleton() const
    {
        return _animatedSkeleton;
    }
    int getNumJoints() const
    {
        return _numJoints;
    }
    const JointInfo& GetJointInfo( size_t index ) const
    {
        assert( index < _jointInfoList.size() );
        return _jointInfoList[index];
    }
	const std::vector<Matrix>& getSkeletonBoneMatrices() const
	{
		return _animatedSkeleton._boneMatrices;
	}
protected:
	void BuildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfo, const BaseFrameList& baseFrames, const FrameData& frameData );
    void InterpolateSkeletons( FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, pfd interpolate );
	
	std::vector<JointInfo> _jointInfoList;
	std::vector<Bound> _boundList;
	std::vector<BaseFrame> _baseFrameList;
	std::vector<FrameData> _frameDataList;
	std::vector<FrameSkeleton> _skeletonList;
	FrameSkeleton _animatedSkeleton;
private:
	size_t _md5Version;
	size_t _numFrames;
	size_t _numJoints;
	size_t _frameRate;
	size_t _numAnimatedComponents;
	pfd _animDuration;
	pfd _frameDuration;
	pfd _animTime;
};

#endif