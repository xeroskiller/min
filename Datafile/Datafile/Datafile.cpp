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
#include "Datafile.h"
#include "DataContainerBase.h"
#include "funcs.h"
#include <limits>
#include <fstream>
#include <vector>
#include <cassert>

using namespace DataFile;

char * Datafile::_header = "db\n\r";
const size_t Datafile::_headerLen = 4;

Datafile::Datafile()
{
	currentID = 0;
	_is_new = false;
}

Datafile::Datafile(const std::string& dbPath, bool overwrite)
{
	currentID = 0;
	_path = dbPath;
	if(overwrite)
	{
		std::ofstream file(_path, std::ios::out|std::ios::binary|std::ios::ate);
	}
	std::ifstream file(_path, std::ios::in|std::ios::binary|std::ios::ate);
	if(file.is_open())
	{
		size_t size = file.tellg();
		if(size == 0 || overwrite)
		{
			_is_new = true;
			_is_open = true;
			return;
		}
		std::vector<char> data(size);
		file.seekg(0, std::ios::beg);
		file.read(&data[0], size);
		//decrypt
		size_t position = size;
		std::string header(data.end() - _headerLen, data.end());
		position -= _headerLen;
		if(header.compare(_header) != 0)
		{
			_is_open = false;
			return;
		}
		else
		{
			header = std::string(data.end() - _headerLen - sizeof(_int64), data.end() - _headerLen);
			position -= sizeof(_int64);
			const char* ID = header.c_str();
			currentID = ntohll(*ID);
		}

		//Load table info
		unsigned long tableSize;
		position = position - sizeof(unsigned long);
		assert(position > 0);
		memcpy(&tableSize, &data[position], sizeof(unsigned long));
		tableSize = ntohl(tableSize);
		for(size_t k=0; k<tableSize; k++)
		{
			AddTable();
		}

		std::list<DatabaseTable*>::iterator i = _table.begin();
		for(; i!=_table.end(); i++)
		{
			(*i)->Read(&data, &position);
		}
	}
	else
	{
		_is_open = false;
		return;
	}
	_is_open = true;
	file.close();
}

bool Datafile::Save()
{
	std::ofstream file(_path, std::ios::out|std::ios::binary);
	if(file.is_open())
	{
		std::vector<char> data;
		std::list<DatabaseTable*>::iterator i = _table.begin();
		for(; i!=_table.end(); i++)
		{
			(*i)->Write(&data);
		}

		currentID = htonll(currentID);
		char* ID = (char*)&currentID;
		//Save table info
		unsigned long type = _table.size();
		type = htonl(type);
		data.insert(data.end(), (char*)&type, (char*)&type + sizeof(unsigned long));
		
		data.insert(data.end(), ID, ID + sizeof(_int64));
		data.insert(data.end(), _header, _header + _headerLen);
		//encrypt
		file.write(&data[0], data.size());
	}
	else
	{
		return false;
	}
	file.close();
	return true;
}

Datafile::~Datafile()
{
	std::list<DatabaseTable*>::iterator i = _table.begin();
	for(; i != _table.end(); i++)
	{
		delete (*i);
	}
}

_int64 Datafile::GenerateKey()
{
	 _int64 temp = currentID;
	 currentID++;
	 #undef max
	 if(currentID == std::numeric_limits<std::size_t>::max())
	 {
		 Reformat();
	 }
	 return temp;
}

void Datafile::Reformat()
{

}

size_t Datafile::AddTable()
{
	DatabaseTable* table = new DatabaseTable();
	_table.push_back(table);
	return _table.size() - 1;
}

DataFile::DatabaseTable* Datafile::GetTable(size_t n)
{
	std::list<DatabaseTable*>::iterator i = _table.begin();
	std::advance(i, n);
	return *i;

}