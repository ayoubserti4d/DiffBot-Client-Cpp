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

#include "JSONValue.h"
#include "JSONString.h"
#include "JSONNumber.h"
#include "JSONObject.h"
#include "JSONArray.h"
#include "JSONBoolean.h"
#include "JSONNull.h"

bool JSONValue::create( const char* buf, size_t len, JSONValue*& obj )
{
	bool ret = true;

	switch( JSONValue::valueType( buf, len ) )
	{
		case JSONValue::String:
			obj = new JSONString;
			break;

		case JSONValue::Number:
			obj = new JSONNumber;
			break;

		case JSONValue::Object:
			obj = new JSONObject;
			break;

		case JSONValue::Array:
			obj = new JSONArray;
			break;

		case JSONValue::Boolean:
			obj = new JSONBoolean;
			break;

		case JSONValue::Null:
			obj = new JSONNull;
			break;

		case JSONValue::Unknown:
			ret = false;

		case JSONValue::None:
		default:
			obj = NULL;
			break;
	}

	return ret;
}

short JSONValue::valueType( const char* buf, size_t len )
{
	char ch;

	for( size_t i = 0 ; i < len ; i++ )
	{
		ch = *( buf + i );
		// skip whitespaces
		if( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' )
			continue;

		switch( ch )
		{
			case '"':
				return String;

			case '-': //num
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return Number;

			case '{':
				return Object;

			case '[':
				return Array;

			case 't': // boolean
			case 'T': 
			case 'f': 
			case 'F': 
				return Boolean;

			case 'n': // null
			case 'N':
				return Null;

			default:
				return Unknown;
		}
	}
	return None;
}

void JSONValue::dump() const
{
	
}

