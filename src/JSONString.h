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

#ifndef __TALKSON_STRING_H__
#define __TALKSON_STRING_H__

#include "JSONValue.h"

class JSONString: public JSONValue
{
	friend class JSONStringSerializer;

	public:
		JSONString();
		JSONString( const char* );
		~JSONString();

		size_t parse( const char* buf, size_t len );
		char* get() const
		{
			return static_cast<char*>( data );
		}

		size_t len() const
		{
			return length;
		}


		bool operator==( const char* buf ) const;
		bool operator==( const JSONString& buf ) const;
		bool operator<( const char* buf ) const;
		bool operator<( const JSONString& buf ) const;

		void dump() const;

	private:
		enum
		{
			NONE = 0x0,
			START = 0x1,
			ESCAPE = 0x2,
			HEXDIGIT = 0x3
		};
		char lexis;
		char hexDigitBuffer[ 4 ];
		size_t hexDigitOffset: 3;
		size_t maxSize;
		size_t length;

		char* data;
};

#endif /* __TALKSON_STRING_H__ */
