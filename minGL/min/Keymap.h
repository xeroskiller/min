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
#ifndef __KEYMAP__
#define __KEYMAP__

#include "stdafx.h"
#include <Windows.h>
#include <list>
class Keymap
{
public:
	static const unsigned char MAX_KEYS = 0xFE; //virtual key code table on MSN 254
	typedef void (* KeyFunc)(void*);
	Keymap();
	~Keymap();
	bool RegisterKeyCallback(WPARAM wParam, KeyFunc f, void*);
	void Keydown(WPARAM wParam);
	void Keyup(WPARAM wParam);
	void Update();
	void Call(WPARAM wParam);
protected:
	KeyFunc _map[MAX_KEYS];
	void* _data[MAX_KEYS];
	std::list<WPARAM> _keysDown;
};

#endif