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
#include "StringContainer.h"
#include "funcs.h"

using namespace DataFile;

StringContainer::StringContainer(DatabaseTable::DatabaseTableEnum type) : DatabaseContainer<std::string>(type)
{
}


StringContainer::~StringContainer()
{
}

void StringContainer::Write(std::vector<char>* vec)
{
	unsigned long bytes=0;
	DataContainer<std::string>* str = 0;
	//Writes Header and writes all values to disk -- WRITES BACKWARDS
	if(_table.size() > 0)
	{
		std::deque<DataContainer<std::string>*>::reverse_iterator i = _table.rbegin();
		for(;i!= _table.rend(); i++)
		{
			str = *i;
			unsigned long key = str->Key();
			key = htonl(key);
			vec->insert(vec->end(), (char*)&key, (char*)&key + sizeof(unsigned long));
			bytes += sizeof(unsigned long);

			std::string* data = str->Item();
			vec->insert(vec->end(), (char*)data->c_str(), (char*)data->c_str() + data->size());
			bytes += data->size();

			unsigned long size = str->Item()->size();
			size = htonl(size);
			vec->insert(vec->end(), (char*)&size, (char*)&size + sizeof(unsigned long));
			bytes += sizeof(unsigned long);
		}
	}
	bytes = htonl(bytes);
	vec->insert(vec->end(), (char*)&bytes, (char*)&bytes + sizeof(unsigned long));

}

void StringContainer::Read(std::vector<char>* vec, size_t* pos)
{
	//Reads Header and reads all values from disk -- READS BACKWARDS
	assert(*pos > 0);
	unsigned long bytes;
	*pos = *pos - sizeof(unsigned long);
	memcpy(&bytes, &vec->operator[](*pos), sizeof(unsigned long));
	bytes = ntohl(bytes);

	size_t count=0;
	char* cstr = 0;
	unsigned long size;
	unsigned long key;
	while(bytes > count)
	{
		*pos = *pos - sizeof(unsigned long);
		assert(*pos > 0);
		memcpy(&size, &vec->operator[](*pos), sizeof(unsigned long));
		size = ntohl(size);
		count += sizeof(unsigned long);

		cstr = new char[size + 1];
		cstr[size] = 0;
		*pos = *pos - size;
		assert(*pos > 0);
		memcpy(cstr, &vec->operator[](*pos), size);
		std::string str(cstr);
		count += size;
	
		*pos = *pos - sizeof(unsigned long);
		assert(*pos >= 0);
		memcpy(&key, &vec->operator[](*pos), sizeof(unsigned long));
		key = ntohl(key);
		Add(str, key);
		count += sizeof(unsigned long);
		delete [] cstr;
	}
}
