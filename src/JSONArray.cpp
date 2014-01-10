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

#include "JSONArray.h"
#include "JSONString.h"
#include "JSONValue.h"

void JSONArray::add( ValueType* value )
{
	// always add it to the end
	data.insert( mapType::value_type( count++, value ) );
}

const JSONArray::ValueType* JSONArray::get( size_t index ) const
{
	mapType::const_iterator target = data.find( index );
	return ( target == data.end() ) ? NULL : target->second;
}

void JSONArray::remove( const size_t key )
{
}


size_t JSONArray::parse( const char* buf, size_t len )
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

		if( ch != '[' )
		{
			status = ERROR;
			return pos;
		}
		pos++;

		// skip whitespaces
		ch = *( buf + pos );
		while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
		{
			if( ++pos == len )
				return pos;
			ch = *( buf + pos );
		}
		lexis = VALUE | TERMINATE;
	}

	while( pos < len )
	{
		ch = *( buf + pos );

		switch( lexis & MASK )
		{
			case VALUE:
				if( valueObj == NULL )
				{
					// skip whitespaces
					while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
					{
						if( ++pos == len )
							return pos;
						ch = *( buf + pos );
					}

					if( lexis & TERMINATE && ch == ']' )
					{
						status = COMPLETE;
						return ++pos;
					}
						
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
						break;
					}
				}

				pos += valueObj->parse( buf + pos, len - pos );
				switch( valueObj->status )
				{
					case COMPLETE:
						this->add( valueObj ); 
						valueObj = NULL;

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
					case ']':
						status = COMPLETE;
						return ++pos;

					case ',':
						lexis = VALUE; // set TERMINATE flag off
						++pos;

						break;

					default:
						status = ERROR;
						return pos;
				}
				break;
		}
	}

	return pos;
}

void JSONArray::dump() const
{
	mapType::const_iterator iter = data.begin();

	fprintf( stdout, "[\n" );

	for( ; iter != data.end() ; iter++ )
	{
		// print a value
		fprintf( stdout, "\t" );
		iter->second->dump();

		fprintf( stdout, "\n" );
	}

	fprintf( stdout, "]\n" );
}

size_t JSONArray::getLength() const
{
	size_t len = count;
	return len;
}
