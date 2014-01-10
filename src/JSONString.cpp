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

#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "JSONString.h"

JSONString::JSONString()
: JSONValue( JSONValue::String ), lexis( START ), hexDigitOffset( 0 ), maxSize( 32 ), length( 0 )
{
	memset( hexDigitBuffer, '\0', 4 );
	data = static_cast<char*>( malloc( sizeof(char) * maxSize + 1 ) );
	data[ sizeof(char) * maxSize ] = '\0';
}

JSONString::JSONString( const char* buf )
: JSONValue( JSONValue::String ), lexis( START ), maxSize( 0 ), length( 0 )
{
	maxSize = length = strlen( buf );
	data = strdup( buf );
}

JSONString::~JSONString()
{
	free( data );
}

bool JSONString::operator==( const char* buf ) const
{
	if( buf == NULL )
		return false;

	return !strncmp( data, buf, strlen( buf ) );
}

bool JSONString::operator<( const char* buf ) const
{
	if( buf == NULL )
		return false;

	return strncmp( data, buf, strlen( buf ) ) < 0; 
}

bool JSONString::operator==( const JSONString& rhs ) const
{
	return !strncmp( data, rhs.data, rhs.length );
}

bool JSONString::operator<( const JSONString& rhs ) const
{
	return strncmp( data, rhs.data, rhs.length ) < 0;
}

size_t JSONString::parse( const char* buf, size_t len )
{
	size_t pos = 0;
	size_t start = 0;
	size_t written = 0;
	size_t oldSize = 0;
	char ch;

	if( status != INCOMPLETE )
		return 0;
	
	if( buf == NULL || len == 0 )
		return 0;

	if( lexis == START )
	{
		ch = *( buf + pos );
		while( ch == ' ' || ch ==  '\t' || ch == '\n' || ch == '\r' )
		{
			if( ++pos == len )
				return pos;
			ch = *( buf + pos );
		}

		if( ch != '"' )
		{
			status = ERROR;
			return pos;
		}
		lexis = NONE;
		pos++;
	}

	for( start = pos ; pos < len ; pos++ )
	{
		ch = *( buf + pos );

		switch( lexis )
		{
			case ESCAPE:
				lexis = NONE;
				switch( ch )
				{
					case '"':	// quotation mark
					case '\\':	// reverse solidus
					case '/':	// solidus
						*( data + length++ ) = *( buf + pos );
						break;

					case 'u':	// 4-hex-digit character
						lexis = HEXDIGIT;
						hexDigitOffset = 0;
						break;

					case 'f':	// form feed
						*( data + length++ ) = '\f';
						break;

					case 'n':	// line feed
						*( data + length++ ) = '\n';
						break;

					case 'r':	// carrige return
						*( data + length++ ) = '\r';
						break;

					case 't':	// tab
						*( data + length++ ) = '\t';
						break;

					default:
						break;
					// unknown escape character?
				}

				start = pos + 1;
				break;

			case HEXDIGIT:
				written = (len - pos) >= (static_cast<size_t>(4) - hexDigitOffset) ?
				(4 - hexDigitOffset) : len - pos;

				memcpy( hexDigitBuffer + hexDigitOffset, buf + pos, written );
				hexDigitOffset += written;
				pos += written - 1;
				start = pos + 1;

				if( hexDigitOffset == 4 )
				{
					// decode 4-hex-digit character
					#define hexToDigit( ch ) (( ch < 'A') ? (ch - '0') : ((ch & ~0x20) - 7 - '0'))
					if( hexDigitBuffer[ 0 ] == '0' && hexDigitBuffer[ 1 ] == '0' )
						// ascii
						*( data + length++ ) = ( hexToDigit(hexDigitBuffer[ 2 ]) << 4 ) |
								       hexToDigit(hexDigitBuffer[ 3 ]);
					else
					{
						*( data + length++ ) = ( hexToDigit(hexDigitBuffer[ 0 ]) << 4 ) |
								       hexToDigit(hexDigitBuffer[ 1 ]);
						*( data + length++ ) = ( hexToDigit(hexDigitBuffer[ 2 ]) << 4 ) |
								       hexToDigit(hexDigitBuffer[ 3 ]);
					}
					#undef hexToDigit

					lexis = NONE;
				}
				break;

			case NONE:
				if( ch == '\\' )
				{
					// copy upto the previous element
					oldSize = maxSize;
					while( pos - start + 2 /* for 4-hex-digit character */ > maxSize - length )
						maxSize *= 2;

					if( oldSize != maxSize )
						data = static_cast<char*>( realloc( data, maxSize ) );

					memcpy( data + length, buf + start, pos - start );
					length += (pos - start);

					start = pos + 1;

					lexis |= ESCAPE;
					continue;
				}
				else if( ch == '"' )
				{
					// finalize
					oldSize = maxSize;
					while( pos - start + 1 /*for \0 */ > maxSize - length /* for \0 */ )
						maxSize *= 2;

					if( oldSize != maxSize )
						data = static_cast<char*>( realloc( data, maxSize ) );

					memcpy( data + length, buf + start, pos - start );
					length += (pos - start);
					pos++;

					status = COMPLETE;
					*( data + length ) = '\0';

					return pos;
				}
		}
	}

	if( start < pos )
	{
		oldSize = maxSize;
		while( pos - start > maxSize - length /* for \0 */ )
			maxSize *= 2;

		if( oldSize != maxSize )
			data = static_cast<char*>( realloc( data, maxSize ) );

		memcpy( data + length, buf + start, pos - start );
		length += (pos - start);
	}

	return pos;
}

void JSONString::dump() const
{
	if( status == COMPLETE )
		fprintf( stdout, "\"%s\"", data );
}

