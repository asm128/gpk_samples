#define GPK_CONSOLE_LOG_ENABLED

// client.c -- a stream socket client demo
// http://beej.us/guide/bgnet/html/single/bgnet.html#simpleclient
#include "gpk_stdsocket.h"
#include "gpk_udp_server.h"
#include "gpk_find.h"
#include "gpk_parse.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <string.h>
//#include <sys/types.h>
#if defined GPK_WINDOWS
#	include <WinSock2.h>
#	include <iphlpapi.h>
#	include <WS2tcpip.h>
#else
#	include <unistd.h>
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#endif

static	::gpk::error_t			httpRequestChunkedJoin			(const ::gpk::view_const_byte & body, ::gpk::array_pod<byte_t> & joined)		{
	uint32_t							iBegin							= 0;
	uint32_t							iStop							= 0;
	while(iBegin < (int32_t)body.size()) {
		iBegin							= iStop;
		iStop							= (uint32_t)::gpk::find('\n', body, iBegin);
		++iStop;	// skip \n
		::gpk::view_const_string			strSize;
		if(iStop <= body.size())
			strSize						= {&body[iBegin], (uint32_t)iStop - iBegin};
		else 
			break;
		uint64_t							sizeChunk						= 0;
		::gpk::parseArbitraryBaseInteger(16, "0123456789abcdef", strSize, &sizeChunk);
		if(0 == sizeChunk)
			break;
		joined.append(::gpk::view_const_byte{&body[iStop], (uint32_t)sizeChunk});
		iStop							+= (uint32_t)sizeChunk;
		iStop							+= 2;	// skip \n
	}
	return 0;
}

// get sockaddr, IPv4 or IPv6:
void *							get_in_addr						(sockaddr *sa)			{ return (sa->sa_family == AF_INET) ? &(((struct sockaddr_in*)sa)->sin_addr) : (void*)&(((struct sockaddr_in6*)sa)->sin6_addr); }
int								main							()						{
	::gpk::tcpipInitialize();
	::gpk::SIPv4						address							= {{192,168,0,2}, 80};
	addrinfo							hints							= {};
    hints.ai_family					= AF_UNSPEC;
    hints.ai_socktype				= SOCK_STREAM;
	char								addr[32]						= {};
	char								port[32]						= {};
	sprintf_s(addr, "%u.%u.%u.%u", GPK_IPV4_EXPAND_IP(address));
	sprintf_s(port, "%u", address.Port);
	//sprintf_s(temp, "www.tutorialspoint.com");//, GPK_IPV4_EXPAND_IP(address));

	addrinfo							* servinfo						= 0;
    int32_t								rv								= getaddrinfo(addr, port, &hints, &servinfo);
	ree_if(0 != rv, "getaddrinfo: %s\n", gai_strerror(rv));

	SOCKET								sockfd							= INVALID_SOCKET;
    // loop through all the results and connect to the first we can
	addrinfo							* currentServinfo				= 0;
    for(currentServinfo = servinfo; currentServinfo != NULL; currentServinfo = currentServinfo->ai_next) {
        ce_if((sockfd = socket(currentServinfo->ai_family, currentServinfo->ai_socktype, currentServinfo->ai_protocol)) == -1, "%s", "client: socket");
        if (connect(sockfd, currentServinfo->ai_addr, (int)currentServinfo->ai_addrlen) == -1) {
            gpk_safe_closesocket(sockfd);
            error_printf("%s", "client: connect");
            continue;
        }
        break;
    }

    ree_if(currentServinfo == NULL, "%s", "client: failed to connect\n");
	char								strAddress	[INET6_ADDRSTRLEN]	= {};
    inet_ntop(currentServinfo->ai_family, get_in_addr((struct sockaddr *)currentServinfo->ai_addr), strAddress, ::gpk::size(strAddress));
    printf("client: connecting to %s\n", strAddress);
    freeaddrinfo(servinfo); // all done with this structure

	const char							http_request	[]				= 
		"GET /users.exe HTTP/1.1"
		"\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)"
		"\r\nHost: www.tutorialspoint.com"
		"\r\nAccept-Language: en-us"
		"\r\nAccept-Encoding: gzip, deflate"
		//"\r\nConnection: Keep-Alive"
		"\r\n"
		"\r\n"
		;
    int									numbytes						= 0;  
    ree_if(-1 == (numbytes = send(sockfd, http_request, ::gpk::size(http_request), 0)), "%s", "send");

	::gpk::array_pod<char>				buf								= {};
	buf.resize(1024*1024*64);
	uint32_t							totalBytes						= 0;
    while(-1 != (numbytes = recv(sockfd, &buf[totalBytes], buf.size() - totalBytes - 1, 0)) && 0 != numbytes) {
		totalBytes						+= numbytes;
		continue;
	}
	if(totalBytes < buf.size()) {
		buf[totalBytes]					= '\0';
		buf.resize(totalBytes);
	}

	uint32_t							stopOfHeader					= (uint32_t)::gpk::find_sequence_pod(::gpk::view_const_string{"\r\n\r\n"}, {buf.begin(), buf.size()});
	::gpk::view_const_byte				httpheaderReceived				= buf;
	::gpk::view_const_byte				contentReceived					= {};
	if(stopOfHeader >= buf.size() - 4) 
		stopOfHeader					= buf.size();

	for(uint32_t iByte = 0, sizeHeader = stopOfHeader; iByte < sizeHeader; ++iByte)
		buf[iByte]						= (byte_t)::tolower(buf[iByte]);

	::gpk::array_obj<::gpk::view_const_byte> headerLines;
	httpheaderReceived				= {buf.begin(), (uint32_t)stopOfHeader};
	::gpk::split(httpheaderReceived, '\n', headerLines);
	bool								bChunked						= false;
	for(uint32_t iLine = 0; iLine < headerLines.size(); ++iLine) {
		::gpk::array_pod<char_t>			strLine = headerLines[iLine];
		strLine.push_back(0);
		printf("\n%s", strLine.begin());
		if(0 <= ::gpk::find_sequence_pod(::gpk::view_const_string{"chunked"}, ::gpk::view_const_char{strLine})) 
			bChunked					= true;
	}

	if(stopOfHeader + 4 < buf.size())
		contentReceived					= {&buf[stopOfHeader + 4], buf.size() - stopOfHeader + 4};

	::gpk::array_pod<byte_t>			joined;
	if(bChunked) {
		printf("\nChunked: ");
		::httpRequestChunkedJoin(contentReceived, joined);
		contentReceived					= joined;
	}

	if(contentReceived.size()) {
		printf("\nclient: received '%s'\n", contentReceived.begin());
		OutputDebugStringA(contentReceived.begin());
	}

	info_printf("Header stop at position %u.", (uint32_t)stopOfHeader);
    gpk_safe_closesocket(sockfd);
 	::gpk::tcpipShutdown();
   return 0;
}
