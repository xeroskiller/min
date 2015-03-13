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
#include "Keymap.h"


Keymap::Keymap()
{
	for(size_t i=0; i < static_cast<size_t>(MAX_KEYS); i++)
	{
		_map[i] = 0;
	}
}

Keymap::~Keymap()
{
}

bool Keymap::RegisterKeyCallback(WPARAM wParam, KeyFunc f, void* data)
{
	if(wParam < 255) //1-254 keys used
	{
		_map[wParam] = f;
		_data[wParam] = data;
		return true;
	}
	else
	{
		return false;
	}
}

void Keymap::Keydown(WPARAM wParam)
{
	_keysDown.push_back(wParam);
}

void Keymap::Keyup(WPARAM wParam)
{
	_keysDown.remove(wParam);
}

void Keymap::Update()
{
	std::list<WPARAM>::iterator k=_keysDown.begin();
	for(; k!= _keysDown.end(); k++)
	{
		KeyFunc f = _map[(*k)];
		if(f != 0)
		{
			f(_data[(*k)]);
		}
	}
}

void Keymap::Call(WPARAM wParam)
{
	KeyFunc f = _map[wParam];
	if(f != 0)
	{
		f(_data[wParam]);
	}
	
}