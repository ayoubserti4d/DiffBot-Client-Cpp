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

#ifndef __TALKSON_NUMBER_H__
#define __TALKSON_NUMBER_H__

#include <cmath>
#include "JSONValue.h"

class JSONNumber: public JSONValue
{
	friend class JSONNumberSerializer;

	public:
		JSONNumber()
		: JSONValue( JSONValue::Number ), 
		  fraction( 0 ),
		  exponent( 0 ),
		  lexis( SIGN | INT ),
		  data( 0 )
		{
		}

		size_t parse( const char* buf, size_t len );
		void dump() const;
		double get() const;
		char countSD() const;

	private:
		enum
		{
			// a data must come next
			NUM = 0x1,
			SIGN = 0x2,

			// sign data
			NEGATIVE = 0x4,
			EXP_NEGATIVE = 0x8,
		
			// parsing stages
			INT = 0x10,
			FRAC = 0x20,
			EXP = 0x40,
		};

		void finalize()
		{
			if( lexis & ( SIGN | NUM ) )
				status = ERROR;
			else
			{
				status = COMPLETE;

				// calculate frac and exp
				if( lexis & EXP_NEGATIVE )
					fraction -= exponent;
				else
					fraction += exponent;

				if( lexis & NEGATIVE )
					data *= -1;
			}
		}


		char fraction;
		unsigned short exponent;
		unsigned char lexis;
		unsigned int data;
};

#endif /* __TALKSON_NUMBER_H__ */
