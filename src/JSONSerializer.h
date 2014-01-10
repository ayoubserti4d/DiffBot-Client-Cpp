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

#ifndef __TALKSON_SERIALIZER_H__
#define __TALKSON_SERIALIZER_H__
#if WIN32
	#define snprintf sprintf_s
#endif

#include "JSONValue.h"
#include "JSONArray.h"
#include "JSONBoolean.h"
#include "JSONNull.h"
#include "JSONNumber.h"
#include "JSONObject.h"
#include "JSONString.h"

class JSONSerializer
{
	friend class JSONObjectSerializer;

	public:
		JSONSerializer()
		: target( NULL )
		{
		}

		virtual ~JSONSerializer()
		{
		}

		virtual void open( const JSONValue* obj )
		{
			target = obj;
		}

		virtual bool isOpen() const
		{
			return target != NULL;
		}

		virtual void close()
		{
			target = NULL;
		}
		virtual size_t encode( char* buf, size_t len ) = 0;

	protected:
		const JSONValue* target;
};

class JSONArraySerializer : public JSONSerializer
{
	public:
		JSONArraySerializer()
		: JSONSerializer(), memberEncoder( NULL ), isBegin( 1 )
		{
		}

		~JSONArraySerializer()
		{
			close();
		}

		void open( const JSONValue* obj )
		{
			isBegin = 1;
			JSONSerializer::open( obj );
		}

		void close()
		{
			if( memberEncoder != NULL )
				delete memberEncoder;

			memberEncoder = NULL;
			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	private:
		JSONSerializer* memberEncoder;
		JSONArray::mapType::const_iterator memberIterator;
		char isBegin;
};

class JSONBooleanSerializer : public JSONSerializer
{
	public:
		JSONBooleanSerializer()
		: JSONSerializer(), dataValue( NULL ), dataOffset( 0 )
		{
		}

		~JSONBooleanSerializer()
		{
			close();
		}

		void close()
		{
			dataValue = NULL;
			dataOffset = 0;

			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	private:
		const char* dataValue;
		size_t dataOffset;
};

class JSONNullSerializer : public JSONSerializer
{
	public:
		JSONNullSerializer()
		: JSONSerializer(), dataValue( NULL ), dataOffset( 0 )
		{
		}

		~JSONNullSerializer()
		{
			close();
		}

		void close()
		{
			dataValue = NULL;
			dataOffset = 0;

			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	private:
		const char* dataValue;
		size_t dataOffset;
};

class JSONNumberSerializer : public JSONSerializer
{
	public:
		JSONNumberSerializer()
		: JSONSerializer(), dataValue( NULL ), dataOffset( 0 )
		{
		}

		~JSONNumberSerializer()
		{
			close();
		}

		void close()
		{
			if( dataValue != NULL )
				delete[] dataValue;

			dataValue = NULL;
			dataOffset = 0;

			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	private:
		char* dataValue;
		size_t dataSize;
		size_t dataOffset;
};

class JSONStringSerializer : public JSONSerializer
{
	friend class JSONObjectSerializer;

	public:
		JSONStringSerializer()
		: JSONSerializer(), dataValue( NULL ), dataSize( 0 ), dataOffset( 0 ),
		escapeValue( NULL ), escapeSize( 0 ), escapeOffset( 0 )
		{
		}

		~JSONStringSerializer()
		{
			close();
		}

		void close()
		{
			dataValue = NULL;
			dataSize = 0;
			dataOffset = 0;

			escapeValue = NULL;
			escapeSize = 0;
			escapeValue = 0;
			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	protected:
		virtual void encodeEscape( char character );

	private:
		const char* dataValue;
		size_t dataSize;
		size_t dataOffset;

		const char* escapeValue;
		size_t escapeSize;
		size_t escapeOffset;

		char unicodeBuffer[6];
};

class JSONObjectSerializer : public JSONSerializer
{
	public:
		JSONObjectSerializer()
		: JSONSerializer(), valueEncoder( NULL ), isBegin( 1 )
		{
		}

		~JSONObjectSerializer()
		{
			close();
		}

		void open( const JSONValue* obj )
		{
			isBegin = 1;
			JSONSerializer::open( obj );
		}

		void close()
		{
			if( valueEncoder != NULL )
			{
				delete valueEncoder;
				valueEncoder = NULL;
			}

			JSONSerializer::close();
		}

		size_t encode( char* buf, size_t len );

	private:
		JSONStringSerializer nameEncoder;
		JSONSerializer* valueEncoder;
		JSONObject::mapType::const_iterator memberIterator;
		char isBegin;
};
#endif /* __TALKSON_JSONSERIALIZER_H__ */
