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
#include "StdAfx.h"
#include "Shader.h"
#include "minutils.h"

Shader::Shader(void)
{
	_shaderType = GL_VERTEX_SHADER;
	_src=0;
	_glContext=0;
}

Shader::Shader(GLenum ShaderType, OpenGLContext* ptr)
{
	_shaderType = ShaderType;
	_src=0;
	_glContext = ptr;
}

Shader::~Shader(void)
{
	if(_src!=0)
		delete [] this->_src;
}

void Shader::LoadShader(char* filename)
{
	fstream file(filename, ios::in|ios::binary|ios::ate);
	if(file.is_open())
	{
		ifstream::pos_type size = file.tellg();
		_src = new char [(size_t)size+1];
		file.seekg(0, ios::beg);
		file.read((char*)_src, size);
		file.close();
		_src[static_cast<size_t>(size)] = '\0';
	}
	if(_src != 0)
	{
		_shaderId = glCreateShader(_shaderType);
		glShaderSource(_shaderId, 1, (const char**)&_src, NULL);
		delete [] _src;
		_src=0;
		glCompileShader(_shaderId);
		GLint compile_ok = GL_FALSE;
		glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &compile_ok);
		if(compile_ok == GL_FALSE)
		{
			printErrorLog();
			fatal_error(_glContext, L"OpenGL error", L"Error: Failed to compile shader.");
		}
	}
}

void Shader::printErrorLog()
{
	GLint log_length =0;
	if (glIsShader(_shaderId))
	{
		glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &log_length);
		char* log = new char[log_length];
		glGetShaderInfoLog(_shaderId, log_length, NULL, log);
		wchar_t* wc = new wchar_t[log_length];
		mbstowcs(wc,log,log_length);
		error(_glContext, L"OpenGL error", wc);
		delete [] log;
		delete [] wc;
		exit(-1);
	}
	else 
	{
		fatal_error(_glContext, L"OpenGL error", L"Error: Object is not a shader. Can't print error log.");
	}
}