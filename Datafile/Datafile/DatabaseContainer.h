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
#ifndef __DATABASECONTAINER__
#define __DATABASECONTAINER__

namespace DataFile
{
	class DatabaseContainerBase;
}
#include "DatabaseContainerBase.h"
#include "DataContainer.h"

#include <assert.h>
#include <string>
#include <list>
#include <deque>

namespace DataFile
{

	template <class T> class DatabaseContainer : public DatabaseContainerBase
	{
	public:
		DatabaseContainer(DatabaseTable::DatabaseTableEnum type);
		virtual ~DatabaseContainer();
		size_t Add(const T& item, size_t index);
		void Purge();
		void Remove(const T& item, unsigned long key);
		void Find(const T& item, std::list<DataContainer<T>*>* returnList);
		void Find(const T& item, size_t key, std::list<DataContainer<T>*>* returnList);
		DataContainer<T>* Find(const T& item, size_t loc);
		size_t Index(DataContainer<T>* item);
		T operator[](size_t n);

		virtual void Write(std::vector<char>*) = 0;
		virtual void Read(std::vector<char>*, size_t*) = 0;
	protected:
		std::deque<DataContainer<T>*> _table;
	};

	template <class T> DatabaseContainer<T>::DatabaseContainer(DatabaseTable::DatabaseTableEnum type) : DatabaseContainerBase(type)
	{
	
	}

	template <class T> DatabaseContainer<T>::~DatabaseContainer()
	{
		std::deque<DataContainer<T>*>::iterator i = _table.begin();
		for(; i!= _table.end();)
		{
			delete (*i);
			i = _table.erase(i);
		}
	}

	template <class T> size_t DatabaseContainer<T>::Add(const T& ref, size_t key)
	{
		DataContainer<T>* item = new DataContainer<T>(ref, key);
		size_t n = Index(item);
		_length++;
		return n;
	}

	template <class T> void DatabaseContainer<T>::Purge()
	{
		std::deque<DataContainer<T>*>::iterator i = _table.begin();
		for(; i!= _table.end();)
		{
			delete (*i);
			i = _table.erase(i);
		}
	}

	template <class T> void DatabaseContainer<T>::Find(const T& item, std::list<DataContainer<T>*>* returnList)
	{
		std::deque<DataContainer<T>*>::iterator k = _table.begin();
		for(; k!= _table.end(); k++)
		{
			if((*(*k)->Item()) == item)
			{
				returnList->push_back((*k));
			}
		}
	}

	template <class T> void DatabaseContainer<T>::Find(const T& item, size_t key, std::list<DataContainer<T>*>* returnList)
	{
		std::deque<DataContainer<T>*>::iterator k = _table.begin();
		for(; k!= _table.end(); k++)
		{
			if((*(*k)->Item()) == item && (*k)->Key() == key)
			{
				returnList->push_back((*k));
			}
		}
	}

	template <class T> DataContainer<T>* DatabaseContainer<T>::Find(const T& item, size_t loc)
	{
		std::deque<DataContainer<T>*>::iterator k = _table.begin();
		std::advance(k, loc);
		return (*k);
	}

	template <class T> void DatabaseContainer<T>::Remove(const T& item, unsigned long key)
	{
		std::deque<DataContainer<T>*>::iterator k = _table.begin();
		for(; k!= _table.end(); k++)
		{
			if(key == (*k)->Key())
			{
				delete (*k);
				k = _table.erase(k);
				break;
			}
			else
			{
				k++;
			}
		}
		_length--;
	}

	template <class T> size_t DatabaseContainer<T>::Index(DataContainer<T>* item)
	{
		_table.push_back(item);
		return _table.size() - 1;
	}

	template <class T> T DatabaseContainer<T>::operator[](size_t n)
	{
		std::deque<DataContainer<T>*>::iterator k = _table.begin();
		std::advance(k, n);
		DataContainer<T>* container = *k;
		return *container->Item();
	}
}

#endif

