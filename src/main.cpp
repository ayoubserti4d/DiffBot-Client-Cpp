
/*
	main example.
	for testing purpose
*/
#include <iostream>
#include <iostream>


#include "DiffBotClient.h"

int main()
{
	//Create DiffBotClient object
	DiffBotClient diffObj("9e09975cc36c4d7869ed80b51a3673fa");
	
	//fields stringify array to use
	string fields("[\"title\",\"html\" ]");
	
	JSONArray* arrFields = new JSONArray();
	//parse fields into a JSONArray object
	arrFields->parse(fields.c_str(),fields.length());
	
	//call DiffBotClient::diffbot() function to query DiffBot server and receive fetched article into JSONObject format
	JSONObject* json = diffObj.diffbot("http://www.diffbot.com/our-apis/article",arrFields);

	//dump binary json object into stdout
	json->dump(); 
	
	//used in Windows only
	system("pause");
	return 0;

}

