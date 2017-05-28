#include "stdafx.h"
#include "ServerManager.h"

ServerManager *ServerManager::sm = 0;

ServerManager * ServerManager::getInstance()
{
	if (!sm)
		sm = new ServerManager;
	return sm;
}

void ServerManager::destroyInstance()
{
	if (sm)
	{
		delete sm;
		sm = 0;
		WSACleanup();
	}
}

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer)
{
	int result = 0;
	if (buffer)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

ServerManager::ServerManager()
{
	WSADATA WsaData;
	if (WSAStartup(0x202, &WsaData))
	{
		std::cout << "WSAStartup error" << WSAGetLastError() << std::endl;
		exit(1);
	}

	volume = 0;
	std::string content;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = nullptr;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://2ip.ru");	
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);		
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	auto tmp = content.substr(content.find("d_clip_button\">"));
	auto end = tmp.find_first_of("</");
	auto beg = tmp.find_first_of(">");
	content = tmp.substr(beg,end);
	tmp = "";
	for (int i = 1; i < content.size(); i++)
	{
		if (content[i] == '<')
			break;
		else
			tmp += content[i];
	}

	content = tmp;	
	std::cout << "Welcome to LazyControl\n" << std::endl;

	std::cout <<"Your global IP: "<< content.c_str()<<std::endl;

	char szHostName[255];
	int nAdapter = 0;
	std::vector<std::string> localIP;
	gethostname(szHostName, 255);
	auto host_entry = gethostbyname(szHostName);

	while (host_entry->h_addr_list[nAdapter])
	{
		struct sockaddr_in adr;
		memcpy(&adr.sin_addr, host_entry->h_addr_list[nAdapter], host_entry->h_length);
		auto out = inet_ntoa(adr.sin_addr);
		nAdapter++;
		localIP.push_back(out);	
	}

	std::cout << "Your local IP: " << localIP[localIP.size() - 1] << std::endl << std::endl;

	for (size_t i=0; i<localIP.size()-1; i++)
		std::cout << "Your other local IP: " << localIP[i] << std::endl;
	
	msock = createSocket(PORT, 5);

	if (msock < 0)
		exit(1);

	csock = accept(msock, (struct sockaddr*) &remaddr, &remaddrs);

	while (1)
	{
		if(!Configure())
			csock = accept(msock, (struct sockaddr*) &remaddr, &remaddrs);
	}
}

int ServerManager::createSocket(int port, int qlen)
{
	struct protoent *ppe;
	struct sockaddr_in sin;
	int s;

	memset(&sin, 0, sizeof(sin));

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	if ((ppe = getprotobyname("tcp")) == 0)
	{
		std::cout << "Convert transport protocol error\n";
		return -1;
	}

	s = socket(PF_INET, SOCK_STREAM, ppe->p_proto);
	if (s < 0)
	{
		std::cout << "Error creating socket\n";
		return -1;
	}

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		std::cout << "Socket Binding Error\n";
		return -1;
	}

	if (listen(s, qlen) <0)
	{
		std::cout << "Error listening socket\n";
		return -1;
	}
	return s;
}

bool ServerManager::ChangeVolume(double nVolume, bool bScalar)
{
	HRESULT hr = NULL;
	bool decibels = false;
	bool scalar = false;
	double newVolume = nVolume;

	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;

	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);

	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

	if (bScalar == false)
		hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
	else if (bScalar == true)
		hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
	endpointVolume->Release();

	CoUninitialize();
	return FALSE;
}

bool ServerManager::Configure()
{
	if (csock < 0)
	{
		std::cout << "Connection Accept Error \n";
		return false;
	}
	else
	{
		memset(&msg, 0, sizeof(msg));
		recv(csock, msg, sizeof(msg), 0);

		if (sizeof(msg) > 0)
		{
			if (strstr(msg, "volume_plus"))
			{
				memset(&revbuf, 0, sizeof(revbuf));
				
				volume += 0.05f;

				ChangeVolume(volume, true);

				return true;
			}
			else if (strstr(msg, "volume_minus"))
			{
				memset(&revbuf, 0, sizeof(revbuf));
				volume -= 0.05f;

				if (volume < 0)
					volume = 0;

				ChangeVolume(volume, true);

				return true;
			}
			else if (strstr(msg, "power_off"))
			{
				system("shutdown -s -t 0");
				return true;
			}
		}
	}
	return false;
}

ServerManager::~ServerManager()
{
	closesocket(msock);
	destroyInstance();
}
