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
#include "LightManager.h"
#include "OpenGLContext.h"
#include "minutils.h"

LightManager::LightManager(OpenGLContext* context)
{
	_glContext = context;
	_isUpdated = false;
}


LightManager::~LightManager()
{
	std::vector<Light*>::iterator j = _lights.begin();
	for(; j != _lights.end(); )
	{
		delete (*j);
		j = _lights.erase(j);
	}
}

Light* LightManager::addLight(const Vector3<pfd>& pos, const Vector3<pfd>& color, pfd power, size_t type)
{
	_isUpdated = false;
	_lightBuffer.push_back(pos);
	_lightColorBuffer.push_back(color);
	_lightPowerBuffer.push_back(power);
	Light* light = new Light(_glContext, &_lightBuffer.back(), &_lightColorBuffer.back(), power, _lightBuffer.size() - 1, type);
	_lights.push_back(light);
	return light;
}

void LightManager::setLightPosition(size_t index, const Vector3<pfd>& pos)
{
	_isUpdated = false;
	_lightBuffer[index] = pos;
}

void LightManager::setLightColor(size_t index, const Vector3<pfd>& color)
{
	_isUpdated = false;
	_lightColorBuffer[index] = color;
}

void LightManager::SetLight()
{
	if(_lights.size() > 0)
	{
		glUniform3fv(LIGHT_LOCATION, _lightBuffer.size(), (pfd*)&_lightBuffer[0]);
		glUniform3fv(LIGHT_COLOR_LOCATION, _lightColorBuffer.size(), (pfd*)&_lightColorBuffer[0]);
		glUniform1fv(LIGHT_POWER_LOCATION, _lightPowerBuffer.size(), (pfd*)&_lightPowerBuffer[0]);
	}
}

void LightManager::setProgram(GLuint progId)
{
	LIGHT_LOCATION = glGetUniformLocation(progId, "Lights");
	if(LIGHT_LOCATION == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get Light Uniform.");
	}
	LIGHT_COLOR_LOCATION = glGetUniformLocation(progId, "LightColor");
	if(LIGHT_COLOR_LOCATION == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get LightColor Uniform.");
	}
	LIGHT_POWER_LOCATION = glGetUniformLocation(progId, "LightPower");
	if(LIGHT_POWER_LOCATION == -1)
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Could not get LightPower Uniform.");
	}

	check_glError(_glContext, L"OpenGL error", L"Error: Light Manager error.");
}