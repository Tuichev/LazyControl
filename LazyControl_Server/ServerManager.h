#pragma once
#include "Def.h"
#include <fstream>
#include <stdio.h>
#include <vector>
#define CURL_STATICLIB
#include "curl/curl.h"
#ifdef _DEBUG
#pragma comment (lib,"curl/libcurl_a_debug.lib")
#else
#pragma comment (lib,"curl/libcurl_a.lib")
#endif
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
#include "iostream"
#include "string"
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#pragma comment (lib, "Ole32.lib")

class ServerManager
{
public:
	static ServerManager* getInstance();
	static void destroyInstance();

private:
	bool Configure();
	int createSocket(int port, int qlen);
	bool ChangeVolume(double nVolume, bool bScalar);

	float volume;
	char revbuf[LENGTH];
	int msock, csock = 0;
	struct sockaddr_in  remaddr;
	int remaddrs = sizeof(remaddr);
	char msg[21];

	ServerManager();
	~ServerManager();
	static ServerManager* sm;
};