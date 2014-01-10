/* 
    This file is part of the Quickson project

    Copyright 2010 Seok Lee <ddoman@iused.net>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __TALKSON_BOOLEAN_H__
#define __TALKSON_BOOLEAN_H__

#include "JSONValue.h"

class JSONBoolean: public JSONValue
{
	public:
		JSONBoolean();

		size_t parse( const char* buf, size_t len );
		void dump() const;

		bool get() const
		{
			return (lexis & TRUE);
		}

		static char* strTrue()
		{
			return const_cast<char*>("true");
		}

		static char* strFalse()
		{
			return const_cast<char*>("false");
		}

	private:
		enum
		{
			MASK = 0x7, /* 111 in binary */
			TRUE = 0x8 /* 1000 in binary */
		};
		char lexis;
};

#endif /* __TALKSON_BOOLEAN_H__ */
