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

#ifndef __TALKSON_ARRAY_H_
#define __TALKSON_ARRAY_H__

#include <map>
#include <functional>

#include "JSONValue.h"
#include "JSONString.h"

class JSONArray: public JSONValue
{
	friend class JSONArraySerializer;

	public:
		typedef JSONValue ValueType;
		typedef std::map< size_t, ValueType* > mapType;

		JSONArray()
		: JSONValue( JSONValue::Array ),
		  lexis( START ),
		  count( 0 ),
		  valueObj( NULL )
		{
		}

		~JSONArray()
		{
			if( valueObj != NULL )
				delete valueObj;

			mapType::iterator it;
			for( it = data.begin() ; it != data.end() ; it++ )
				delete it->second;
		}

		size_t parse( const char* buf, size_t len );
		void add( ValueType* value );
		const ValueType* get( size_t index ) const;
		void remove( const size_t index );
		size_t getLength(void) const;

		void dump() const;

	private:
		enum
		{
			VALUE = 0x1, // 001
			VALUE_SEPERATOR = 0x2, // 010

			MASK = 0x7, // 111
			START = 0x8, // 1000
			TERMINATE = 0x10 // 10000
		};

		// Associative container for key/value pairs

		char lexis;
		size_t count;
		ValueType* valueObj;
		mapType data;
};

#endif /* __TALKSON_ARRAY_H__ */
