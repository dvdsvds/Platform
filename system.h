#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <vector>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <sqlite3.h>
#include <openssl/evp.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")
class CREATE_ACCOUNT {
private:
	std::string USERNAME;
	std::string ID;
	std::string PW;
	std::string NEWUSERNAME;
	std::string NEWID;
	std::string NEWPW;
public:
	void initDataBase();
	CREATE_ACCOUNT();
	std::string sha256(const std::string& str);
	int SignUP(sqlite3* db);
	int SignIN(sqlite3* db);
	int checkName(sqlite3* db, const std::string& name);
	int checkID(sqlite3* db, const std::string& ID);
	int checkPW(sqlite3* db, const std::string& PW);
};
#endif