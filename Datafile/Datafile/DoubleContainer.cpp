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
#include "DoubleContainer.h"
#include "funcs.h"

using namespace DataFile;

DoubleContainer::DoubleContainer(DatabaseTable::DatabaseTableEnum type) : DatabaseContainer<double>(type)
{
}


DoubleContainer::~DoubleContainer()
{

}

void DoubleContainer::Write(std::vector<char>* vec)
{
	unsigned long bytes=0;
	DataContainer<double>* val = 0;
	//Writes Header and writes all values to disk -- WRITES BACKWARDS
	if(_table.size() > 0)
	{
		std::deque<DataContainer<double>*>::reverse_iterator i = _table.rbegin();
		for(;i!= _table.rend(); i++)
		{
			val = *i;
			unsigned long key = val->Key();
			key = htonl(key);
			vec->insert(vec->end(), (char*)&key, (char*)&key + sizeof(unsigned long));
			bytes += sizeof(unsigned long);
			double* d = val->Item();
			_int64 data;
			data = htond(*d);
			vec->insert(vec->end(), (char*)&data, (char*)&data + sizeof(_int64));
			bytes += sizeof(_int64);
		}	
	}
	bytes = htonl(bytes);
	vec->insert(vec->end(), (char*)&bytes, (char*)&bytes + sizeof(unsigned long));
}

void DoubleContainer::Read(std::vector<char>* vec, size_t* pos)
{
	//Reads Header and reads all values from disk -- READS BACKWARDS
	assert(*pos > 0);
	unsigned long bytes;
	*pos = *pos - sizeof(unsigned long);
	memcpy(&bytes, &vec->operator[](*pos), sizeof(unsigned long));
	bytes = ntohl(bytes);

	size_t count=0;
	char* cstr = 0;
	_int64 val;
	double d;
	unsigned long key;
	while(bytes > count)
	{
		*pos = *pos - sizeof(double);
		assert(*pos > 0);
		memcpy(&val, &vec->operator[](*pos), sizeof(double));
		d = ntohd(val);
		count += sizeof(double);

		*pos = *pos - sizeof(unsigned long);
		assert(*pos >= 0);
		memcpy(&key, &vec->operator[](*pos), sizeof(unsigned long));
		key = ntohl(key);
		Add(d, key); 
		count += sizeof(unsigned long);
	}
}

