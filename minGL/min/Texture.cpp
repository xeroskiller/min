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
#include "Texture.h"
#include <fstream>

Texture::Texture()
{
	_data = 0;
	_width = 0;
	_height = 0;
	_imageSize = 0;
	_format=0;
	_mipMapCount=0;
}

//RGB Only
Texture::Texture(unsigned char* data, int width, int height, int imageSize)
{
	if(data == 0)
		return;
	if(imageSize != width*height)
		return;
	_data = new unsigned char[imageSize];
	memcpy(_data, data, imageSize);
	_width = width;
	_height = height;
}


Texture::~Texture(void)
{
	if(_data)
		delete [] _data;
}

bool Texture::LoadTexture(const std::string& absFilePath)
{
	this->WriteExtension(absFilePath);
	if(_extension == "bmp" || _extension == "BMP")
	{
		return this->LoadBMPTexture(absFilePath);
	}
	else if(_extension == "dds" || _extension == "DDS")
	{
		return this->LoadDDSTexture(absFilePath);
	}
	return false;
}

bool Texture::LoadBMPTexture(const std::string& absFilePath)
{
	std::ifstream file (absFilePath, std::ios::in|std::ios::binary|std::ios::ate);
	if(file.is_open())
	{
		char header[2];
		file.seekg(0, std::ios::beg);
		file.read(header, 2);

		if(header[0] != 'B' && header[1] != 'M')
		{
			return false;
		}

		int size; 
		file.read((char*)&size, 4);

		int dataPos;
		file.seekg(10, std::ios::beg);
		file.read((char*)&dataPos, 4); //should be 54

		int header_size;
		file.read((char*)&header_size, 4); // should be 40

		assert(header_size == 40);

		file.read((char*)&_width, 4);

		file.read((char*)&_height, 4);

		file.seekg(34, std::ios::beg);
		file.read((char*)&_imageSize, 4);

		if(_imageSize == 0)
		{
			_imageSize = 3*_width*_height;
			dataPos = 54;
		}
		_imageSize = 3*_imageSize;
		_format = GL_RGB8;

		file.seekg(dataPos, std::ios::beg);
		if(_data)
			delete [] _data;
		_data = new unsigned char[_imageSize+1];
		_data[_imageSize] = 0;
		file.read((char*)_data, _imageSize);
		file.close();
		return true;
	}
	return false;
}

bool Texture::LoadDDSTexture(const std::string& absFilePath)
{
	std::ifstream file (absFilePath, std::ios::in|std::ios::binary|std::ios::ate);
	if(file.is_open())
	{
		char init[5];
		file.seekg(0, std::ios::beg);
		file.read(init, 4);
		init[4] = 0;
		std::string str("DDS ");
		if(str.compare(init) != 0)
		{
			return false;
		}

		unsigned char header[124];
		file.read((char*)header, 124);
		
		memcpy(&_height, &header[8], sizeof(unsigned int)); //unsigned
		memcpy(&_width, &header[12], sizeof(unsigned int)); //unsigned
		unsigned int linearSize;
		memcpy(&linearSize, &header[16], sizeof(unsigned int));
		memcpy(&_mipMapCount, &header[24], sizeof(unsigned int));
		unsigned int fourCC;
		memcpy(&fourCC, &header[80], sizeof(unsigned int));
		
		_imageSize = _mipMapCount > 1 ? linearSize * 2 : linearSize;
		if(_data)
			delete [] _data;
		_data = new unsigned char[_imageSize+1];
		_data[_imageSize] = 0;

		unsigned int components = (fourCC == FOURCC_DXT1) ? 3:4; //what does this do

		switch(fourCC)
		{
			case FOURCC_DXT1:
			{
				_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			}
			case FOURCC_DXT3:
			{
				_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			}
			case FOURCC_DXT5:
			{
				_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			}
			default:
			{
				return false;
			}
		}

		file.read((char*)_data, _imageSize);
		file.close();
		return true;
	}
	return false;
}

int Texture::Width()
{
	return _width;
}

int Texture::Height()
{
	return _height;
}

int Texture::Size()
{
	return _imageSize;
}

unsigned char* Texture::Data()
{
	return _data;
}

unsigned int Texture::Format()
{
	return _format;
}

unsigned int Texture::MipMapCount()
{
	return _mipMapCount;
}

void Texture::WriteExtension(const std::string& texturePath)
{
	size_t dot;
	char ch;
	for(size_t i=0; i< texturePath.size(); i++)
	{
		ch = texturePath[i];
		if(ch == '.')
		{
			dot = i;
			dot++;
			break;
		}
	}
	if(_extension.size() > 0)
	{
		_extension = std::string("");
	}
	for(size_t i = dot; i<texturePath.size(); i++)
	{
		_extension += texturePath[i];
	}
}