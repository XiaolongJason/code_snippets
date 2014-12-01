#ifndef _TSTRING_H_
#define _TSTRING_H_

#include <string>

#ifdef UNICODE 
#    define tstring std::wstring
#else
#    define tstring std::string
#endif

#endif //_TSTRING_H_

