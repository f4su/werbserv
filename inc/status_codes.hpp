#ifndef STATUS_CODES_HPP
#define STATUS_CODES_HPP

//	1xx - Informational
#define STATUS_100 "100 Continue"
//	#define STATUS_101 "101 Switching Protocols"								NOT IMPLEMENTED
//	#define STATUS_103 "103 Early Hints"												NOT IMPLEMENTED


//	2xx - Success
#define STATUS_200 "200 OK"
#define STATUS_201 "201 Created"
#define STATUS_202 "202 Accepted"
//	#define STATUS_203 "203 Non-Authoritative Information"		NOT IMPLEMENTED
#define STATUS_204 "204 No Content"
#define STATUS_205 "205 Reset Content"
//	#define STATUS_206 "206 Partial Content"									NOT IMPLEMENTED


//	3xx - Redirection
//	#define STATUS_300 "300 Multiple Choices"									NOT IMPLEMENTED
#define STATUS_301 "301 Moved Permanently"
//	#define STATUS_302 "302 Found"														NOT IMPLEMENTED
//	#define STATUS_303 "303 See Other"												NOT IMPLEMENTED
//	#define STATUS_304 "304 Not Modified"											NOT IMPLEMENTED
#define STATUS_307 "307 Temporary Redirect"
//	#define STATUS_308 "308 Permanent Redirect"								NOT IMPLEMENTED

//	4xx - Client Error
#define STATUS_400 "400 Bad Request"
//	#define STATUS_401 "401 Unauthorized"											NOT IMPLEMENTED
//	#define STATUS_402 "402 Payment Required"									NOT IMPLEMENTED									
#define STATUS_403 "403 Forbidden"
#define STATUS_404 "404 Not Found"
#define STATUS_405 "405 Method Not Allowed"
#define STATUS_406 "406 Not Acceptable"
//	#define STATUS_407 "407 Proxy Authentication Required"		NOT IMPLEMENTED
//	#define STATUS_408 "408 Request Timeout"									NOT IMPLEMENTED
#define STATUS_409 "409 Conflict"
//	#define STATUS_410 "410 Gone"
#define STATUS_411 "411 Length Required"
//	#define STATUS_412 "412 Precondition Failed"							NOT IMPLEMENTED
#define STATUS_413 "413 Request Entity Too Large"
#define STATUS_414 "414 URI Too Long"
#define STATUS_415 "415 Unsupported Media Type"
//	#define STATUS_416 "416 Range Not Satisfiable"						NOT IMPLEMENTED
#define STATUS_417 "417 Expectation Failed"
//	#define STATUS_418 "418 I'm a teapot"											NOT IMPLEMENTED
//	#define STATUS_422 "422 Unprocessable Entity"							NOT IMPLEMENTED
//	#define STATUS_425 "425 Too Early"												NOT IMPLEMENTED
//	#define STATUS_426 "426 Upgrade Required"									NOT IMPLEMENTED
//	#define STATUS_428 "428 Precondition Required"						NOT IMPLEMENTED
//	#define STATUS_429 "429 Too Many Requests"								NOT IMPLEMENTED
//	#define STATUS_431 "431 Request Header Fields Too Large"	NOT IMPLEMENTED
//	#define STATUS_451 "451 Unavailable For Legal Reasons"		NOT IMPLEMENTED


//	5xx - Server Error
#define STATUS_500 "500 Internal Server Error"
#define STATUS_501 "501 Not Implemented"
#define STATUS_502 "502 Bad Gateway"
//	#define STATUS_503 "503 Service Unavailable"							NOT IMPLEMENTED
//	#define STATUS_504 "504 Gateway Timeout"									NOT IMPLEMENTED
#define STATUS_505 "505 HTTP Version Not Supported"
//	#define STATUS_506 "506 Variant Also Negotiates"					NOT IMPLEMENTED
#define STATUS_507 "507 Insufficient Storage"
//	#define STATUS_508 "508 Loop Detected"										NOT IMPLEMENTED
//	#define STATUS_510 "510 Not Extended"											NOT IMPLEMENTED
//	#define STATUS_511 "511 Network Authentication Required"	NOT IMPLEMENTED


#endif
