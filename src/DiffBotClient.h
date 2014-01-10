
/*
TODO: Add license header
*/

#ifndef __DIFF_BOT_ClIENT__
#define __DIFF_BOT_CLIENT__

#include "JSONValue.h"
#include "JSONArray.h"
#include "JSONBoolean.h"
#include "JSONNull.h"
#include "JSONNumber.h"
#include "JSONObject.h"
#include "JSONString.h"

#include "diffbot.h"
#include "tinycurl.h"

using namespace std;

/*
DiffBotClient is our API entry
*/

class DiffBotClient
{
public :
	/*
		@function constructor 
		@params  
				inToken is a string : DiffBot token
	*/
	DiffBotClient(const string& inToken);
	
	
	/*
		@function diffbot
		@abstract query DiffBot Server with url and fields to fetch
		@param	
				inURL: string which is the url article's to fetch
				inFields: JSONArray*  which contains fields to fetch ( exp : title, url, author,..)
				inAPI:	 string not used at the moment
				inVersion: (1 or 2) the DiffBot version; not used
		@return JSONObject* contains a binary representation of the received data.
	*/
	JSONObject*	diffbot(const string& inURL,JSONArray* inFields = NULL ,const string& inAPI=string("analyze"),enum version inVersion = VERSION_2);
	
	

private:
	static std::vector<string> JSONArrayToVector(JSONArray* inJsonArray);
	string fToken;
	static string fApiURL;
};


#endif