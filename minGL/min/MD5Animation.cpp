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
#include "MD5Animation.h"
#include <fstream>
#include <sstream>
#include <algorithm>

MD5Animation::MD5Animation()
{
}


MD5Animation::~MD5Animation()
{
}

bool MD5Animation::LoadAnimation(const std::string& filename)
{
	 std::ifstream file (filename, std::ios::in);
	 if(file.is_open())
	 {
		 std::string param;
		 std::string junk;
		 std::string line;
		 std::istringstream s;

		 _jointInfoList.clear();
		 _boundList.clear();
		 _baseFrameList.clear();
		 _frameDataList.clear();
		 _animatedSkeleton._joints.clear();
		 _numFrames = 0;

		 while(std::getline(file, line))
		 {
			 param = "";
			 s = std::istringstream(line);
			 s >> param;
			 if(param == "")
			 {
				 continue;
			 }
			 else if(param == "MD5Version")
			 {
				 s >> _md5Version;
				 if (_md5Version != 10)
				 {
					 return false;
				 }
			 }
			 else if(param == "commandline")
			 {
				 continue;
			 }
			 else if(param == "numFrames")
			 {
				 s >> _numFrames;
			 }
			 else if(param == "numJoints")
			 {
				 s >> _numJoints;
			 }
			 else if(param == "frameRate")
			 {
				 s >> _frameRate;
			 }
			 else if( param == "numAnimatedComponents")
			 {
				 s >> _numAnimatedComponents;
			 }
			 else if(param == "hierarchy")
			 {
				 for(size_t i=0; i < _numJoints; i++)
				 {
					 JointInfo joint;
					 std::getline(file, line);
					 s = std::istringstream(line);
					 s >> joint._name;
					 s >> joint._parentId;
					 s >> joint._flag;
					 s >> joint._startIndex;
					 std::string& name = joint._name;
					 name.erase( std::remove( name.begin(), name.end(), '\"' ), name.end());

					 _jointInfoList.push_back(joint);
				 }
			 }
			 else if( param == "bounds")
			 {
				 for(size_t i=0; i < _numFrames; i++)
				 {
					 Bound bound;
					 std::getline(file, line);
					 s = std::istringstream(line);
					 s >> junk;
					 s >> bound._min._arr[0];
					 s >> bound._min._arr[1];
					 s >> bound._min._arr[2];
					 s >> junk; s >> junk;
					 s >> bound._max._arr[0];
					 s >> bound._max._arr[1];
					 s >> bound._max._arr[2];

					 _boundList.push_back(bound);
				 }
			 }
			 else if(param == "baseframe")
			 {
				 for(size_t i=0; i < _numJoints; i++)
				 {
					 BaseFrame baseFrame;
					 std::getline(file, line);
					 s = std::istringstream(line);
					 s >> junk;
					 s >> baseFrame._position._arr[0];
					 s >> baseFrame._position._arr[1];
					 s >> baseFrame._position._arr[2];
					 s >> junk; s >> junk;
					 s >> baseFrame._rotation._arr[0];
					 s >> baseFrame._rotation._arr[1];
					 s >> baseFrame._rotation._arr[2];

					 _baseFrameList.push_back(baseFrame);
				 }
			 }
			 else if(param == "frame")
			 {
				 FrameData frame;
				 s >> frame._frameId;

				 for(size_t i=0; i < _numAnimatedComponents; )
				 {
					 std::getline(file, line);
					 s = std::istringstream(line);
					 pfd frameData;

					 while(s >> frameData)
					 {
						frame._frameData.push_back(frameData);
						i++;
					 }
				 }

				 _frameDataList.push_back(frame);
				 BuildFrameSkeleton(_skeletonList, _jointInfoList, _baseFrameList, frame);
			 }
		 }

		 _animatedSkeleton._joints.assign(_numJoints, SkeletonJoint() );
		 _animatedSkeleton._boneMatrices.assign(_numJoints, Matrix(IDENTITY_MATRIX));

		 _frameDuration = (pfd)1.0 / _frameRate;
		 _animDuration = (_frameDuration * (pfd)_numFrames);
		 _animTime = (pfd)0.0;

		 if (_jointInfoList.size() != _numJoints || _boundList.size() != _numFrames || _baseFrameList.size() != _numJoints || _frameDataList.size() != _numFrames || _skeletonList.size() != _numFrames)
		 {
			 return false;
		 }
		 return true;
	 }
	 else
	 {
		 return false;
	 }
}

void MD5Animation::BuildFrameSkeleton( FrameSkeletonList& skeletons, const JointInfoList& jointInfos, const BaseFrameList& baseFrames, const FrameData& frameData )
{
	FrameSkeleton skeleton;
	for(size_t i=0; i < jointInfos.size(); i++)
	{
		size_t j=0;

		const JointInfo& jointInfo = jointInfos[i];

		SkeletonJoint animatedJoint = baseFrames[i];

		animatedJoint._parent = jointInfo._parentId;

		if( jointInfo._flag & 1)
		{
			animatedJoint._position._arr[0] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}
		if( jointInfo._flag & 2)
		{
			animatedJoint._position._arr[1] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}
		if( jointInfo._flag & 4)
		{
			animatedJoint._position._arr[2] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}
		if( jointInfo._flag & 8)
		{
			animatedJoint._rotation._arr[1] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}
		if( jointInfo._flag & 16)
		{
			animatedJoint._rotation._arr[2] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}
		if( jointInfo._flag & 32)
		{
			animatedJoint._rotation._arr[3] = frameData._frameData[jointInfo._startIndex + j];
			j++;
		}

		animatedJoint._rotation.calculateW();

		if(animatedJoint._parent >=0)
		{
			SkeletonJoint& parentJoint = skeleton._joints[animatedJoint._parent];
			
			Vector3<pfd> rotPos = parentJoint._rotation.transform(animatedJoint._position);

			animatedJoint._position = parentJoint._position + rotPos;
			animatedJoint._rotation = parentJoint._rotation * animatedJoint._rotation;

			animatedJoint._rotation.normalize();
		}

		skeleton._joints.push_back(animatedJoint);

		Matrix boneMatrix(IDENTITY_MATRIX);
		boneMatrix.SetRotation(animatedJoint._rotation);
		boneMatrix.SetTranslation(animatedJoint._position);

		skeleton._boneMatrices.push_back(boneMatrix);
	}
	skeletons.push_back(skeleton);
}

void MD5Animation::Update( pfd deltaTime)
{
	if(_numFrames < 1)
		return;
	_animTime += deltaTime;

	while( _animTime > _animDuration )
	{
		_animTime -= _animDuration;
	}
	while(_animTime < (pfd) 0.0)
	{
		_animTime += _animDuration;
	}

	pfd frameNum = _animTime * (pfd)_frameRate;
	size_t frame0 = (size_t)floor(frameNum);
	size_t frame1 = (size_t)ceil (frameNum);
	frame0 = frame0 % _numFrames;
	frame1 = frame1 % _numFrames;

	pfd interpolate = fmod(_animTime, _frameDuration) / (_frameDuration);

	InterpolateSkeletons(_animatedSkeleton, _skeletonList[frame0], _skeletonList[frame1], interpolate);
}

void MD5Animation::InterpolateSkeletons( FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, pfd interpolate )
{
	for(size_t i=0; i < _numJoints; i++)
	{
		SkeletonJoint& finalJoint = finalSkeleton._joints[i];
		Matrix& finalMatrix = finalSkeleton._boneMatrices[i];
		const SkeletonJoint& joint0 = skeleton0._joints[i];
		const SkeletonJoint& joint1 = skeleton1._joints[i];

		finalJoint._parent = joint0._parent;

		finalJoint._position = Vector3<pfd>::lerp(joint0._position, joint1._position, interpolate);
		finalJoint._rotation = Quaternion<pfd>::lerp(joint0._rotation, joint1._rotation, interpolate); //get a slerp routine

		finalMatrix.SetTranslation(finalJoint._position);
		finalMatrix.SetRotation(finalJoint._rotation);
	}
}