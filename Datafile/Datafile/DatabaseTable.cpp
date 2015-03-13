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
#include "DatabaseTable.h"
#include "funcs.h"
#include "DoubleContainer.h"
#include "IntContainer.h"
#include "UIntContainer.h"
#include "SocketContainer.h"
#include "StringContainer.h"
#include "FloatContainer.h"

using namespace DataFile;

DatabaseTable::DatabaseTable()
{

}


DatabaseTable::~DatabaseTable()
{
	std::list<DatabaseContainerBase*>::iterator i = _containerTable.begin();
	for(; i!= _containerTable.end(); i++)
	{
		delete (*i);
	}
}

size_t DatabaseTable::Add(DatabaseTable::DatabaseTableEnum type)
{
	switch(type)
	{
		case DOUBLE:
		{
			DoubleContainer* container = new DoubleContainer(type);
			_containerTable.push_back(container);
			break;
		}
		case INT:
		{
			IntContainer* container = new IntContainer(type);
			_containerTable.push_back(container);
			break;
		}
		case UINT:
		{
			UIntContainer* container = new UIntContainer(type);
			_containerTable.push_back(container);
			break;
		}
		case SOCKET:
		{
			SocketContainer* container = new SocketContainer(type);
			_containerTable.push_back(container);
			break;
		}
		case STRING:
		{
			StringContainer* container = new StringContainer(type);
			_containerTable.push_back(container);
			break;
		}
		case FLOAT:
		{
			FloatContainer* container = new FloatContainer(type);
			_containerTable.push_back(container);
			break;
		}
		default:
		{
			return -1;
		}
	}
	return _containerTable.size() - 1;
}

void DatabaseTable::Remove(size_t pos)
{
	std::list<DatabaseContainerBase*>::iterator i = _containerTable.begin();
	std::advance(i, pos);
	delete *i;
	i = _containerTable.erase(i);
}

void DatabaseTable::Read(std::vector<char>* vec, size_t* pos)
{
	unsigned long containers;
	*pos = *pos - sizeof(unsigned long);
	assert(*pos > 0);
	memcpy(&containers, &vec->operator[](*pos), sizeof(unsigned long));
	containers = ntohl(containers);

	unsigned long size;
	*pos = *pos - sizeof(unsigned long);
	assert(*pos > 0);
	memcpy(&size, &vec->operator[](*pos), sizeof(unsigned long));
	size = ntohl(size);

	char* cstr = new char[size + 1];
	cstr[size] = 0;
	*pos = *pos - size;
	assert(*pos > 0);
	memcpy(cstr, &vec->operator[](*pos), size);
	_name = std::string(cstr);

	unsigned long type;
	for(size_t k=0; k<containers; k++)
	{
		*pos = *pos - sizeof(unsigned long);
		assert(*pos > 0);
		memcpy(&type, &vec->operator[](*pos), sizeof(unsigned long));
		type = ntohl(type);
		Add((DatabaseTableEnum)type);
	}

	std::list<DatabaseContainerBase*>::iterator i = _containerTable.begin();
	for(; i!= _containerTable.end(); i++)
	{
		(*i)->Read(vec, pos);
	}
}

void DatabaseTable::Write(std::vector<char>* vec)
{
	std::list<DatabaseContainerBase*>::reverse_iterator i = _containerTable.rbegin();
	for(; i!= _containerTable.rend(); i++)
	{
		(*i)->Write(vec);
	}
	unsigned long type;
	i = _containerTable.rbegin();
	for(; i!= _containerTable.rend(); i++)
	{
		type = (*i)->GetType();
		type = htonl(type);
		vec->insert(vec->end(), (char*)&type, (char*)&type + sizeof(unsigned long));
	}

	std::string* data = &_name;
	vec->insert(vec->end(), (char*)data->c_str(), (char*)data->c_str() + data->size());

	unsigned long size = _name.size();
	size = htonl(size);
	vec->insert(vec->end(), (char*)&size, (char*)&size + sizeof(unsigned long));

	type = _containerTable.size();
	type = htonl(type);
	vec->insert(vec->end(), (char*)&type, (char*)&type + sizeof(unsigned long));
}

const std::string& DatabaseTable::getName()
{
	return _name;
}

void DatabaseTable::setName(const std::string& name)
{
	_name = name;
}

DatabaseContainerBase* DatabaseTable::GetDatabaseContainer(size_t n)
{
	std::list<DatabaseContainerBase*>::iterator i = _containerTable.begin();
	std::advance(i, n);
	return *i;
}