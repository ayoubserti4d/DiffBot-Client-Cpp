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

#include "JSONNumber.h"

// RFC 4627 application/json
//
// * Octal and hex forms are not allowed.  Leading zeros are not allowed.
// * A fraction part is a decimal point followed by one or more digits.
// * An exponent part begins with the letter E in upper or lowercase,
//   which may be followed by a plus or minus sign.  The E and optional
//   sign are followed by one or more digits.
//
// * number = [ minus ] int [ frac ] [ exp ]
size_t JSONNumber::parse( const char* buf, size_t len )
{
	char ch;
	size_t pos = 0;

	if( status != INCOMPLETE )
		return 0;

	if( buf == NULL || len == 0 )
		return 0;

	// SIGN & INT indicates the coming one is the first letter
	if( (lexis & SIGN) && (lexis & INT) )
	{
		// skip whitespaces
		ch = *( buf + pos );
		while( ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' )
		{
			if( ++pos == len )
				return pos;

			ch = *( buf + pos );
		}

		if( ch == '-' )
		{
			pos++;
			lexis |= NEGATIVE;
		}
		// the first letter can only be either - or digit
		else if( ch > '9' || ch < '0' )
		{
			status = ERROR;
			return pos;
		}

		lexis = lexis & ~SIGN;
	}

	for( ; pos < len ; pos++ )
	{
		ch = *( buf + pos );

		if( lexis & INT )
		{
			switch( ch )
			{
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					data = data * 10 + ch - '0';
					lexis &= ~NUM; // set NUM_MUST_COME(off)

					break;

				case '.':
					if( lexis & NUM )
					{
						status = ERROR;
						return pos;
					}

					// get into FRAC
					// set NUM_MUST_COME(on)
					lexis |= NUM;
					lexis = (( lexis & ~INT ) & ~EXP) | FRAC;

					break;

				case 'e':
				case 'E':
					if( lexis & NUM )
					{
						status = ERROR;
						return pos;
					}

					// get into EXP!
					// set SIGN_OR_NUM_MUST_COME(on)
					// In fact, there is no such a case that SIGN is on, but NUM is off.
					// if SIGN is on, NUM is also always on in JSON NUM Syntax.
					lexis |= SIGN;
					lexis = (( lexis & ~INT ) & ~FRAC) | EXP;

					break;

				default:
					finalize();
					return pos;
			}
		}
		else if( lexis & FRAC )
		{
			switch( ch )
			{
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					data = data * 10 + ch - '0';
					lexis &= ~NUM; // set NUM_MUST_COME(off)
					fraction--;

					break;

				case 'e':
				case 'E':
					if( lexis & NUM )
					{
						status = ERROR;
						return pos;
					}

					// get into EXP!
					// set SIGN_OR_NUM_MUST_COME(on)
					lexis |= SIGN;
					lexis = (lexis & ~FRAC) | EXP;

					break;

				default:
					finalize();
					return pos;
			}
		}
		else if( lexis & EXP )
		{
			switch( ch )
			{
				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':
					if( lexis & SIGN )
						lexis &= ~SIGN;

					if( lexis & NUM )
						lexis &= ~NUM;

					exponent = exponent * 10 + ch - '0';
					break;

				case '-':
				case '+':
					if( lexis & SIGN )
					{
						if( ch == '-' )
							lexis |= EXP_NEGATIVE;

						// the next must be a num
						lexis = (lexis & ~SIGN) | NUM;
					}
					else
					{
						status = ERROR;
						return pos;
					}
					break;

				default:
					finalize();
					return pos;
			}
		}
	}
	return pos;
}


void JSONNumber::dump() const
{
	if( status != COMPLETE )
		return;

	fprintf( stdout, "%.20g", get() );
}

// Weak implementation.
// This logic assumes that the type of variable data is int and
// the size is always 32 bits.
// Since C++ 98 does not support byte-specified types, such as uint32_t
// and int64_t, this implementation was chosen and may have to be improved later.
//
// The point of this design - stores a number as an integer, but applies the fraction
// value into the number when returning the actual double number - is related to serializing issues.
//
// To convert a double value into a string, we need to count the number of significant digits
// in a given number. Otherwise, a parser, like itoa(), sprintf(), should take a fixed-precision value, 
// which may bring trailing zeroes or discard some digits. It is OK if the number is only used as a number,
// but trailing zeroes increase the length of each number string by a serializer.
//
// printf() with %g may avoid "trailing zero" problem, but cannot avoid "discarding digit" problem
// since it has to know the number of significant digits.
//
// Thus, to get the optimal size of a number string, counting the number of significant digits is needed
// and seperation of significant digits and fraction value helps solving this issue.
//
// For more information, read the code part of JSONSerializer::serialize( JSONNumber );
double JSONNumber::get() const
{
	static const unsigned int powerArray[] =
	{ 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 }; // 10 digits are enough for uint32_t

	double ret = data;

	if( fraction > 0 )
		ret *= powerArray[ (int)fraction ];
	else if( fraction < 0 )
		ret /= powerArray[ (int)fraction * -1 ];
	return ret;
}

char JSONNumber::countSD() const
{
	static const unsigned int powerArray[] =
	{ 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 }; // 10 digits are enough for uint32_t

	short min = 0;
	short max = sizeof( powerArray ) / sizeof( powerArray[0] );
	short mid = max / 2;

	while( min < max )
	{
		mid = (max + min) / 2;
		if( data > powerArray[ mid ] )
			min = mid + 1;
		else if( data < powerArray[ mid ] )
			max = mid - 1;
		else
			break;

	}; 

	if( fraction < 0 && mid <= fraction * -1 )
		return fraction + 1;
	else
		return mid + fraction;
}

