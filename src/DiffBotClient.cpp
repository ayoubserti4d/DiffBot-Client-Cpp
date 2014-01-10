/*
	  License Header
*/

#include "DiffBotClient.h"
#include <iostream>



string DiffBotClient::fApiURL = "http://api.diffbot.com/v2/article?";

DiffBotClient::DiffBotClient(const string &inToken)
: fToken(inToken)
{

}


JSONObject*  DiffBotClient::diffbot(const std::string &inURL, JSONArray *inFields, 
									 const std::string &inAPI, version inVersion)
{
	JSONObject* json = new JSONObject();
	
	//construct map to pass to TinyCurl object
	FieldsMap params;

	try{
	TinyCurl curlObj(fApiURL);
	

		
	//convert JSONArray to vector
	if (inFields != NULL)
	{
	vector<string> vFields = DiffBotClient::JSONArrayToVector(inFields);
	vector<string>::iterator it = vFields.begin(), end = vFields.end();
	while(it!=end)
	{
		params.push_back(::make_pair<string,string>("fields",*it));
		it++;
	}

	}
	//token & url in the url
	params.push_back(std::make_pair<string,string>("token",fToken));
	params.push_back(std::make_pair<string,string>("url",inURL));
	string fetchedJSON = curlObj.Fetch(params);

	
	json->parse(fetchedJSON.c_str(),fetchedJSON.length());
	}
	catch (CurlException e)
	{
		//implement new exception class 
		
		cout << e.what() << endl;
	}

	return json;

}


JSONObject* DiffBotClient::test(const string &inData)
{
	JSONObject* json = new JSONObject();
	JSONValue* jsonValue = json;
	const char* str =inData.c_str(); 
	size_t len = inData.length();
	
	json->parse(str, len);
	return json;
}

vector<string> DiffBotClient::JSONArrayToVector(JSONArray* inJsonArray)
{
	vector<string> Result;

	size_t len = inJsonArray->getLength();
	for ( size_t i =0; i < len ; i++)
	{
		const JSONValue* jsValue = inJsonArray->get(i);
		
		string elem;
		switch(jsValue->type)
		{	
			//At the moment we support only array with no Object nether Array
			//TODO: replace this code into JSONValue class (implement serialize() function into all direved classes)
		case JSONValue::Null :
			elem = "null";
			Result.push_back(elem);
			break;
		case JSONValue::String:
			elem=(static_cast<const JSONString* >(jsValue))->get();
			Result.push_back(elem);
			
			break;
		case JSONValue::Boolean:
			break;
		case JSONValue::Number:
			break;
		
		}
	}


	return Result;
}