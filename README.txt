README

This is an experemental Proof of Concept.
The project DiffBotClient ulistrate how we can develop a DiffBot Client Library with C++. 
In this scope, we tried to develop a useful example in how-to use our Client API.

The file main.cpp ulistrate how to use this API.

First you should include "DiffBotClient.h".
At this time, you can instantiate a DiffBotClient object by giving you DiffBot Token
	
	exp: 	DiffBotClient diffObj("9e09975cc36c4d7869ed80b51a3673fa");

You may construct the fields lists which is a JSONArray pointer
	
	exp:	//fields stringify array to use
			string fields("[\"title\",\"html\" ]");
	
			JSONArray* arrFields = new JSONArray();
			//parse fields into a JSONArray object
			arrFields->parse(fields.c_str(),fields.length());

Now, you can call diffbot methods to query DiffBot Server, this methode will return a JSONObject pointer
	
	exp:    	//call DiffBotClient::diffbot() function to query DiffBot server and receive fetched article into JSONObject format
				JSONObject* json = diffObj.diffbot("http://www.diffbot.com/our-apis/article",arrFields);
			
Finaly, you can use JSONObject::dump() to print json contain on the screen
	
	exp:		//dump binary json object into stdout
				json->dump();
