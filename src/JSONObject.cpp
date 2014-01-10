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

#include <cstdio>
#include <map>

#include "JSONObject.h"
#include "JSONString.h"
#include "JSONValue.h"

bool JSONObject::add( KeyType* key, ValueType* value )
{
	mapType::iterator lb = data.lower_bound( key );
	if( lb != data.end() && (!data.key_comp()( key, lb->first )) )
	{
		status = ERROR;
		return false;
	}

	data.insert( lb, mapType::value_type( key, value ) );
	return true;
}

const JSONObject::ValueType* JSONObject::get( KeyType* key ) const
{
	mapType::const_iterator target = data.find( key );
	return ( target == data.end() ) ? NULL : target->second;
}

void JSONObject::remove( const KeyType* key )
{
}


size_t JSONObject::parse( const char* buf, size_t len )
{
	size_t pos = 0;
	char ch;

	if( status != INCOMPLETE )
		return 0;
	
	if( buf == NULL || len == 0 )
		return 0;

	if( lexis & START )
	{
		// skip whitespaces
		ch = *( buf + pos );
		while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
		{
			if( ++pos == len )
				return pos;
			ch = *( buf + pos );
		}

		if( ch != '{' )
		{
			status = ERROR;
			return pos;
		}
		pos++;

		lexis = KEY | TERMINATE;
	}

	while( pos < len )
	{
		ch = *( buf + pos );

		switch( lexis & MASK )
		{
			case KEY:
				if( keyObj == NULL )
				{
					// skip whitespaces
					while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
					{
						if( ++pos == len )
							return pos;
						ch = *( buf + pos );
					}

					if( (lexis & TERMINATE) && ch == '}' )
					{
						status = COMPLETE;
						return ++pos;
					}

					keyObj = new KeyType;
				}

				pos += keyObj->parse( buf + pos, len - pos );
				switch( keyObj->status )
				{
					case COMPLETE:
						// Next, NAME-SEPERATOR should follow.
						lexis = NAME_SEPERATOR;
						break;

					case ERROR:
						status = ERROR;
						return pos;
				}
				break;
				
			case NAME_SEPERATOR:
				// skip whitespaces
				while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
				{
					if( ++pos == len )
						return pos;
					ch = *( buf + pos );
				}

				if( ch == ':' )
					lexis = VALUE;
				else
				{
					status = ERROR;
					return pos;
				}
				pos++;
				break;

			case VALUE:
				if( valueObj == NULL )
				{
					// determine the type of an incoming element 
					// and then allocate it
					if( !JSONValue::create( buf + pos, len - pos, valueObj ) )
					{
						status = ERROR;
						return pos;
					}
					else if( valueObj == NULL ) // remains are just a chunk of whitespaces
					{
						pos = len;
						continue;
					}
				}

				pos += valueObj->parse( buf + pos, len - pos );
				switch( valueObj->status )
				{
					case COMPLETE:
						// add a key/value pair into a map
						if( !this->add( keyObj, valueObj ) )
						{
							status = ERROR;
							return pos;
						}

						valueObj = keyObj = NULL;

						// Next, VALUE-SEPERATOR should follow after
						lexis = VALUE_SEPERATOR;
						break;

					case ERROR:
						status = ERROR;
						return pos;
				}
				break;
			
			case VALUE_SEPERATOR:
				// skip whitespaces
				while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
				{
					if( ++pos == len )
						return pos;
					ch = *( buf + pos );
				}

				switch( ch )
				{
					case '}':
						status = COMPLETE;
						return ++pos;

					case ',':
						lexis = KEY; // set TERMINATE flag off
						break;

					default:
						status = ERROR;
						return pos;
				}
				pos++;
				break;
		}
	}

	return pos;
}

void JSONObject::dump() const
{
	mapType::const_iterator iter = data.begin();

	fprintf( stdout, "{\n" );

	for( ; iter != data.end() ; iter++ )
	{
		// print a key
		fprintf( stdout, "\t" );
		iter->first->dump();

		// value seperator
		fprintf( stdout, " : " );

		// print a value
		iter->second->dump();

		fprintf( stdout, "\n" );
	}

	fprintf( stdout, "}\n" );
}

