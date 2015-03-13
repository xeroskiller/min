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
#define _CRTDBG_MAP_ALLOC

#include "Database.h"
#include "DoubleContainer.h"
#include "IntContainer.h"
#include "SocketContainer.h"
#include "StringContainer.h"
#include "UIntContainer.h"

int main(int argc, char* argv[])
{
	{
		Database db("C:\\db", "db.txt");
		size_t loc = db.AddTable();
		DatabaseTable* table = db.GetTable(0);
		size_t a = table->Add(DatabaseTable::DOUBLE);
		size_t b = table->Add(DatabaseTable::INT);
		size_t c = table->Add(DatabaseTable::SOCKET);
		size_t d = table->Add(DatabaseTable::STRING);
		size_t e = table->Add(DatabaseTable::UINT);
		double di = 13.0;
		double d1 = 14.0;
		double d2 = 15.0;
		double d3 = 13.0;
		int ii = -13;
		int i1 = -14;
		int i2 = -15;
		int i3 = 13;
		SOCKET s = 13;
		SOCKET s1 = 14;
		SOCKET s2 = 15;
		SOCKET s3 = 13;
		std::string st = "Aaron";
		std::string st1 = "Jake";
		std::string st2 = "David";
		std::string st3 = "John";
		unsigned long u = 13;
		unsigned long u1 = 14;
		unsigned long u2 = 15;
		unsigned long u3 = 13;
		for(size_t i=0; i<250000; i++)
		{
				DoubleContainer* ptr1 = static_cast<DoubleContainer*>(table->GetDatabaseContainer(a));
				ptr1->Add(di, 1);
				ptr1->Add(d1, 1);
				ptr1->Add(d2, 1);
				ptr1->Add(d3, 1);
	
				IntContainer* ptr2 = static_cast<IntContainer*>(table->GetDatabaseContainer(b));
				ptr2->Add(ii, 1);
				ptr2->Add(i1, 1);
				ptr2->Add(i2, 1);
				ptr2->Add(i3, 1);
		
				SocketContainer* ptr3 = static_cast<SocketContainer*>(table->GetDatabaseContainer(c));
				ptr3->Add(s, 1);
				ptr3->Add(s1, 1);
				ptr3->Add(s2, 1);
				ptr3->Add(s3, 1);
		
				StringContainer* ptr4 = static_cast<StringContainer*>(table->GetDatabaseContainer(d));
				ptr4->Add(st, 1);
				ptr4->Add(st1, 1);
				ptr4->Add(st2, 1);
				ptr4->Add(st3, 1);
		
				UIntContainer* ptr5 = static_cast<UIntContainer*>(table->GetDatabaseContainer(e));
			
				ptr5->Add(u, 1);
				ptr5->Add(u1, 1);
				ptr5->Add(u2, 1);
				ptr5->Add(u3, 1);
		}
		db.Save();
	}
	bool test = _CrtDumpMemoryLeaks();
	return 0;
}

