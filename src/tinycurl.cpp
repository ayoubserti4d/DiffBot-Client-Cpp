#include "tinycurl.h"

using namespace std;

TinyCurl::TinyCurl(string url) throw (CurlException)
{
	m_handle = curl_easy_init();
	if ( m_handle == NULL )
		throw CurlException("Unable to initialize curl handler");
	if ( url.length() == 0 )
		throw CurlException("URL can't be of zero length");
	m_url = url;
}

string TinyCurl::Fetch( const FieldsMap& mapFields ) throw (CurlException)
{
	
	SetOptions(mapFields);
	SendGetRequest();
	return m_data;
}

TinyCurl::~TinyCurl()
{
	curl_easy_cleanup(m_handle);
}

void TinyCurl::SetOptions(const FieldsMap& mapFields) throw (CurlException)
{
	CURLcode res= CURLE_OK;
	

	FieldsMap::const_iterator it= mapFields.begin() , end = mapFields.end();
	string param = "";
	while(it!=end && res == CURLE_OK)
	{
			
		param+=it->first;
		param+="=";
		param+=it->second;
		
		
		it++;
		if(it!=end)
			param+="&";
	}
	//set the url
	string url = m_url + param;
	res = curl_easy_setopt(m_handle, CURLOPT_URL, url.c_str());
	if ( res != CURLE_OK)
		throw CurlException(res);

	//res = curl_easy_setopt(m_handle,CURLOPT_POSTFIELDS,param.c_str() );
	//progress bar is not require
	res = curl_easy_setopt(m_handle, CURLOPT_NOPROGRESS, 1L);
	if ( res != CURLE_OK )
		throw CurlException(res);
	
	//set the callback function
	res = curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION,
		TinyCurl::WriteDataCallback);
	if ( res != CURLE_OK )
		throw CurlException(res);
	
	//set pointer in call back function
	res = curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, this);
	if ( res != CURLE_OK )
		throw CurlException(res);
	

}


void TinyCurl::SendGetRequest() throw (CurlException)
{
	CURLcode res;
	res = curl_easy_perform(m_handle);
	if ( res != CURLE_OK )
		throw CurlException(res);
	
}
size_t TinyCurl::WriteDataCallback(void *ptr, size_t size,
										  size_t nmemb, void* pInstance)
{
	return (static_cast<TinyCurl*>(pInstance))->write_data(ptr, size, nmemb);
}

size_t TinyCurl::write_data(void* ptr, size_t size, size_t nmemb)
{
	size_t numOfBytes = size * nmemb;

	char *iter = (char*)ptr;
	char *iterEnd = iter + numOfBytes;

	//while ( iter != iterEnd )
	//{
	//   cout<<*iter;
	//   iter ++;
	//}

	m_data += string(iter, iterEnd);

	return numOfBytes;
}

/*
	Curl Exception Implementation
*/
CurlException::CurlException(std::string message):m_message(message)
{
}

CurlException::CurlException(CURLcode error)
{
  m_message = curl_easy_strerror(error);
}
const char* CurlException::what() throw()
{
  return m_message.c_str();
}

CurlException::~CurlException() throw()
{}