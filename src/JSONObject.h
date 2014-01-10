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

#ifndef __TALKSON_OBJECT_H__
#define __TALKSON_OBJECT_H__

#include <map>
#include <functional>

#include "JSONValue.h"
#include "JSONString.h"

class JSONObject: public JSONValue
{
	friend class JSONObjectSerializer;

	public:
		typedef JSONString KeyType;
		typedef JSONValue ValueType;
		class KeyComparator: public std::binary_function< const KeyType*, const ValueType*, bool >
		{
			public:
				// this is indented to be used as a less, <, operator
				bool operator()( const KeyType* key1, const KeyType* key2 ) const
				{
					return *key1 < *key2;
				}
		};
		typedef std::map< KeyType*, ValueType*, KeyComparator > mapType;

		JSONObject()
		: JSONValue( JSONValue::Object ),
		  lexis( START ),
		  keyObj( NULL ),
		  valueObj( NULL )
		{
		}

		~JSONObject()
		{
			if( keyObj != NULL )
				delete keyObj;
			if( valueObj != NULL )
				delete valueObj;

			mapType::iterator it;
			for( it = data.begin() ; it != data.end() ; it++ )
			{
				delete it->first;
				delete it->second;
			}
		}

		size_t parse( const char* buf, size_t len );
		bool add( KeyType* key, ValueType* value );
		const ValueType* get( KeyType* key ) const;
		void remove( const KeyType* key );

		void dump() const;

	private:
		enum
		{
			KEY = 0x1,    // 001
			NAME_SEPERATOR = 0x2, // 010
			VALUE = 0x3,  // 011
			VALUE_SEPERATOR = 0x4,  // 100

			MASK = 0x7, // 111
			START = 0x8, // 1000
			TERMINATE = 0x10 // 10000

		};

		// Associative container for key/value pairs

		char lexis;
		KeyType* keyObj;
		ValueType* valueObj;
		mapType data;
};

#endif /* __TALKSON_OBJECT_H__ */
