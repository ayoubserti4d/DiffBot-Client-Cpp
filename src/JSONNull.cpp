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

#include "JSONNull.h"

JSONNull::JSONNull()
: JSONValue( JSONValue::Null ), lexis( 0 )
{
}

JSONNull::~JSONNull()
{
}

size_t JSONNull::parse( const char* buf, size_t len )
{
	size_t pos = 0;
	char ch;

	if( status != INCOMPLETE )
		return 0;
	
	if( buf == NULL || len == 0 )
		return 0;

	for( ; pos < len ; pos++ )
	{
		ch = *( buf + pos );

		switch( lexis )
		{
			case 0:
				// skip whitespaces	
				while( ch == ' ' || ch == '\t' || ch == '\r'
					|| ch == '\r' )
				{
					if( ++pos == len )
						return pos;
					ch = *( buf + pos );
				}

				if( ch != 'n' )
				{
					status = ERROR;
					return pos;
				}
				lexis = 1;
				break;

			case 1:
				if( ch != 'u' )
				{
					status = ERROR;
					return pos;
				}
				lexis = 2;
				break;

			case 2:
				if( ch != 'l' )
				{
					status = ERROR;
					return pos;
				}
				lexis = 3;
				break;

			case 3:
				if( ch != 'l' )
				{
					status = ERROR;
					return pos;
				}
				status = COMPLETE;
				return ++pos;
		}
	}

	return pos;
}

void JSONNull::dump() const
{
	if( status == COMPLETE )
		fprintf( stdout, "null" );
}
