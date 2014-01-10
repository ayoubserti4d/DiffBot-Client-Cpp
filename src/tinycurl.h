#include <exception>

extern "C"
{
#include <curl/curl.h>
#include <stdlib.h>

}
#include <string>
#include <map>
#include <vector>

typedef std::vector<std::pair<std::string,std::string> > FieldsMap;
const FieldsMap emptyMap; 

/*
	CurlException class
*/
class CurlException : std::exception
{
public:
   CurlException(std::string message);
   CurlException(CURLcode error);
   const char* what() throw();
   ~CurlException() throw();

private:
   std::string m_message;
};

class TinyCurl
{
public:
	TinyCurl(std::string url)throw (CurlException) ;
	std::string Fetch( const FieldsMap& mapFields = emptyMap) throw (CurlException);
    ~TinyCurl();
   
private:
	void SetOptions(const FieldsMap& mapFields = emptyMap )throw (CurlException) ;
		 void SendGetRequest()throw (CurlException) ;
		 static size_t WriteDataCallback(void *ptr, size_t size,
                                    size_t nmemb, void* pInstance);
		 size_t write_data(void* ptr, size_t size, size_t nmemb);
		CURL *m_handle;
		std::string m_url;
		std::string m_data;


};
