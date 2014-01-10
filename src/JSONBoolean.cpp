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

#include "JSONBoolean.h"

JSONBoolean::JSONBoolean()
: JSONValue( JSONValue::Boolean ), lexis( 0 )
{
}

size_t JSONBoolean::parse( const char* buf, size_t len )
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

		switch( lexis & MASK )
		{
			case 0:
				// skip whitespaces
				while( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
				{
					if( ++pos == len )
						return pos;
					ch = *( buf + pos );
				}

				switch( ch )
				{
					case 't':
						lexis = TRUE | 1;
						break;
					case 'f':
						lexis |= 1;
						break;
					default:
						status = ERROR;
						return pos;
				}
				break;

			case 1:
				switch( ch )
				{
					case 'r':
						if( lexis & TRUE )
						{
							lexis = TRUE | 2;
							break;
						}
						
						status = ERROR;
						return pos;
					case 'a':
						if( lexis & TRUE )
						{
							status = ERROR;
							return pos;
						}

						lexis = 2;
						break;
					default:
						status = ERROR;
						return pos;
				}
				break;

			case 2:
				switch( ch )
				{
					case 'u':
						if( lexis & TRUE )
						{
							lexis = TRUE | 3;
							break;
						}
						
						status = ERROR;
						return pos;
					case 'l':
						if( lexis & TRUE )
						{
							status = ERROR;
							return pos;
						}

						lexis = 3;
						break;
					default:
						status = ERROR;
						return pos;
				}
				break;
			case 3:
				switch( ch )
				{
					case 'e':
						if( lexis & TRUE )
						{
							status = COMPLETE;
							return pos + 1;
						}
						
						status = ERROR;
						return pos;
					case 's':
						if( lexis & TRUE )
						{
							status = ERROR;
							return pos;
						}

						lexis = 4;
						break;
					default:
						status = ERROR;
						return pos;
				}
				break;
			case 4:
				if( ch == 'e' )
				{
					status = COMPLETE;
					return pos + 1;
				}

				status = INCOMPLETE;
				return pos;
		}
	}

	return pos;
}

void JSONBoolean::dump() const
{
	if( status == COMPLETE )
		fprintf( stdout, "%s", (get()) ? JSONBoolean::strTrue() : JSONBoolean::strFalse() );
}
