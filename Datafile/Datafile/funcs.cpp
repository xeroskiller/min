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
#include "funcs.h"

#ifdef __BUILD_XP__

_int64 htond(double d)
{
	assert(sizeof(double) == sizeof(_int64));
	unsigned long buff[2];
	memcpy(&buff, &d, sizeof(double));
	buff[0] = htonl(buff[0]);
	buff[1] = htonl(buff[1]);
	_int64 result;
	memcpy(&result, &buff, sizeof(_int64));
	return result;
}

double ntohd(_int64 i)
{
	assert(sizeof(double) == sizeof(_int64));
	unsigned long buff[2];
	memcpy(&buff, &i, sizeof(_int64));
	buff[0] = ntohl(buff[0]);
	buff[1] = ntohl(buff[1]);
	double result;
	memcpy(&result, &buff, sizeof(double));
	return result;
}

unsigned long htonf(float f)
{
	assert(sizeof(float) == sizeof(unsigned long));
	unsigned long buff;
	memcpy(&buff, &f, sizeof(float));
	return htonl(buff);
}

float ntohf(unsigned long f)
{
	assert(sizeof(float) == sizeof(unsigned long));
	f = ntohl(f);
	float buff;
	memcpy(&buff, &f, sizeof(float));
	return buff;
}

_int64 htonll(_int64 i)
{
	unsigned long buff[2];
	memcpy(&buff, &i, sizeof(_int64));
	buff[0] = htonl(buff[0]);
	buff[1] = htonl(buff[1]);
	_int64 result;
	memcpy(&result, &buff, sizeof(_int64));
	return result;
}

_int64 ntohll(_int64 i)
{
	unsigned long buff[2];
	memcpy(&buff, &i, sizeof(_int64));
	buff[0] = ntohl(buff[0]);
	buff[1] = ntohl(buff[1]);
	_int64 result;
	memcpy(&result, &buff, sizeof(_int64));
	return result;
}

#endif