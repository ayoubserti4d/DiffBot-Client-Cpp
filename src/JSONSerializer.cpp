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

#include "JSONSerializer.h"

template <typename Iter>
Iter nextIterator( Iter iter )
{
	return ++iter;
}

size_t JSONArraySerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;
	size_t dataSize = 0;

	if( target == NULL || len == 0 )
		return 0;

	if( isBegin )
	{
		isBegin = 0;
		*( buf + pos++ ) = '[';
		memberIterator = static_cast<const JSONArray*>(target)->data.begin();

		if( len == 1 )
			return 1;
	}

	for( ; memberIterator != static_cast<const JSONArray*>(target)->data.end() ; memberIterator++ )
	{
		// create an encoder if anything wasn't in progress.
		if( memberEncoder == NULL )
		{
			switch( memberIterator->second->type )
			{
				case JSONValue::Array:
					memberEncoder = new JSONArraySerializer;
					break;

				case JSONValue::Boolean:
					memberEncoder = new JSONBooleanSerializer;
					break;

				case JSONValue::Null:
					memberEncoder = new JSONNullSerializer;
					break;

				case JSONValue::Number:
					memberEncoder = new JSONNumberSerializer;
					break;

				case JSONValue::Object:
					memberEncoder = new JSONObjectSerializer;
					break;

				case JSONValue::String:
					memberEncoder = new JSONStringSerializer;
					break;
			}
			memberEncoder->open( memberIterator->second );
		}

		// start/continue encoding
		if( (dataSize = memberEncoder->encode( buf + pos, len - pos )) == len - pos )
			return pos + dataSize;

		pos += dataSize;

		// close an encoder
		delete memberEncoder;
		memberEncoder = NULL;

		if( nextIterator( memberIterator ) != static_cast<const JSONArray*>(target)->data.end() )
			*( buf + pos++ ) = ',';
	}

	*(buf + pos++) = ']';
	close();

	return pos;
}

size_t JSONBooleanSerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;
	size_t dataSize = 0;

	if( target == NULL || len == 0 )
		return 0;

	if( static_cast<const JSONBoolean*>( target )->get() )
	{
		dataValue = JSONBoolean::strTrue();
		dataSize = 4;
	}
	else
	{
		dataValue = JSONBoolean::strFalse();
		dataSize = 5;
	}

	// calculate the length
	pos = (len < (dataSize - dataOffset)) ? len : (dataSize - dataOffset);
	memcpy( buf, dataValue + dataOffset , pos );
	dataOffset += pos;

	// the end?
	if( dataOffset == dataSize )
		close();

	return pos;
}

size_t JSONNullSerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;
	//size_t dataSize = strlen( JSONNull::strNull() );
	size_t dataSize = 4;

	if( target == NULL || len == 0 )
		return 0;

	if( !dataValue )
		dataValue = JSONNull::strNull();

	// calculate the length
	pos = (len < (dataSize - dataOffset)) ? len : (dataSize - dataOffset);
	memcpy( buf, dataValue + dataOffset , pos );
	dataOffset += pos;

	// the end?
	if( dataOffset == dataSize )
		close();

	return pos;
}

size_t JSONNumberSerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;

	if( target == NULL || len == 0 )
		return 0;

	if( !dataValue )
	{
		// if the buffer is big enough, then copy directly onto the buffer
		if( len > 19 )
		{
			pos =  snprintf( buf, len, "%.19g", static_cast<const JSONNumber*>(target)->get() );

			// avoid further parsing
			close();
			return pos;
		}
		else
		{
			dataValue = new char[ 20 ];
			dataSize = snprintf( dataValue, 20, "%.19g", static_cast<const JSONNumber*>(target)->get() );
		}
	}

	// calculate the length
	pos = (len < (dataSize - dataOffset)) ? len : (dataSize - dataOffset);
	memcpy( buf, dataValue + dataOffset , pos );
	dataOffset += pos;

	// the end?
	if( dataOffset == dataSize )
		close();

	return pos;
}


size_t JSONObjectSerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;
	size_t dataSize = 0;

	if( target == NULL || len == 0 )
		return 0;

	if( isBegin )
	{
		isBegin = 0;
		*( buf + pos++ ) = '{';
		memberIterator = static_cast<const JSONObject*>(target)->data.begin();

		if( len == 1 )
			return 1;
	}

	for( ; memberIterator != static_cast<const JSONObject*>(target)->data.end() ; memberIterator++ )
	{
		// create an encoder if anything wasn't in progress.
		if( nameEncoder.target == NULL && valueEncoder == NULL )
		{
			// any available space to write?
			if( len == pos )
				return pos;

			if( memberIterator != static_cast<const JSONObject*>(target)->data.begin() )
				*(buf + pos++) = ',';

			nameEncoder.open( memberIterator->first );

			switch( memberIterator->second->type )
			{
				case JSONValue::Array:
					valueEncoder = new JSONArraySerializer;
					break;

				case JSONValue::Boolean:
					valueEncoder = new JSONBooleanSerializer;
					break;

				case JSONValue::Null:
					valueEncoder = new JSONNullSerializer;
					break;

				case JSONValue::Number:
					valueEncoder = new JSONNumberSerializer;
					break;

				case JSONValue::Object:
					valueEncoder = new JSONObjectSerializer;
					break;

				case JSONValue::String:
					valueEncoder = new JSONStringSerializer;
					break;
			}
		}

		// start/continue encoding for a name object
		if( nameEncoder.target != NULL )
		{
			if( (dataSize = nameEncoder.encode( buf + pos, len - pos )) == len - pos )
				return pos + dataSize;
	
			pos += dataSize;
		}

		if( valueEncoder->target == NULL )
		{
			*(buf + pos++) = ':';

			valueEncoder->open( memberIterator->second );

			// any space available to write?
			if( len == pos )
				return pos;
		}

		dataSize = valueEncoder->encode( buf + pos, len - pos );
		pos += dataSize;

		// done?
		if( valueEncoder->target == NULL )
		{
			delete valueEncoder;
			valueEncoder = NULL;
		}
		else
			return pos;
	}

	if( pos < len )
	{
		*(buf + pos++) = '}';
		close();
	}
	return pos;
}

void JSONStringSerializer::encodeEscape( char character )
{
	switch( character )
	{
		case '"':	// quotation mark
			escapeValue = "\\\"";
			escapeSize = 2;
			break;

		case '\\':	// reverse solidus
			escapeValue = "\\\\";
			escapeSize = 2;
			break;

		case '\f':	// form feed
			escapeValue = "\\f";
			escapeSize = 2;
			break;

		case '\n':	// line feed
			escapeValue = "\\n";
			escapeSize = 2;
			break;

		case '\r':	// carrige return
			escapeValue = "\\r";
			escapeSize = 2;
			break;

		case '\t':	// tab
			escapeValue = "\\t";
			escapeSize = 2;
			break;
	}
}

size_t JSONStringSerializer::encode( char* buf, size_t len )
{
	size_t pos = 0;
	size_t maximum = 0;
	size_t written = 0;
	const char* begin = 0;
	const char* end = 0;
	const char* last = 0;

	if( target == NULL || len == 0 )
		return 0;

	if( !dataValue )
	{
		*(buf + pos++) = '"';
		dataValue = static_cast<const JSONString*>(target)->data;
		dataSize = static_cast<const JSONString*>(target)->length;
	}

	// any escape character in progress?
	else if( escapeSize != 0 )
	{
		maximum = (len < (escapeSize - escapeOffset)) ? len : (escapeSize - escapeOffset);
		memcpy( buf, escapeValue + escapeOffset, maximum );

		// done?
		escapeOffset += maximum;
		if( escapeSize == escapeOffset )
		{
			escapeOffset = escapeSize = 0;
			pos += maximum;
		}
		else
			return maximum;
	}

	// calculate the maximum length
	maximum = (len - pos < (dataSize - dataOffset)) ? len - pos : (dataSize - dataOffset);

	last = begin = end = dataValue + dataOffset;
	last += maximum;

	while( end < last )
	{
		// encode escape characters
		encodeEscape( *end );
		if( escapeSize != 0 )
		{
			if( end - begin > 0 )
			{
				// copy upto the previous element
				memcpy( buf + pos, begin, end - begin );
				pos += ( end - begin );

			}

			// skip the current escape character
			// 1 is the size of **any** escape character.
			begin = end + 1;
			dataOffset = begin - dataValue;

			// now, process encoding escapes.
			written = (len - pos < (escapeSize - escapeOffset)) ? len - pos : (escapeSize - escapeOffset);
			memcpy( buf + pos, escapeValue + escapeOffset, written );
			
			// done?
			escapeOffset += written;
			pos += written;

			if( escapeSize == escapeOffset )
				escapeOffset = escapeSize = 0;
			else
				return pos;

			// if the buffer was too short to copy all the remain data of the target data
			// and the value of maximum was equal to the length of buffer available to write,
			// then move the address, pointed by last, backward as much as the length of an escape-string
			//
			// len - pos : currently available elements to write
			//
			if( (len - pos) < (dataSize - dataOffset) )
				last = begin + len - pos;
		}
		end++;
	}

	if( end - begin > 0 )
	{
		memcpy( buf + pos, begin, end - begin );
		pos += ( end - begin );
		dataOffset = end - dataValue;
	}

	// the end?
	if( dataSize <= dataOffset )
	{
		if( len - pos > 0 )
		{
			*(buf + pos++) = '"';
			close();
		}
	}

	return pos;
}
