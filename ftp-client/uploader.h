#pragma once
/*
* uploader 上傳器
* FTP 相關的上傳邏輯
*/

#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <SFML/Network.hpp>

using namespace std;

class Uploader {
private:
	string host;
	unsigned short port;
	string username;
	string password;
	unsigned int timeout;
	sf::Ftp ftp;
	sf::Ftp::Response res;
	void BadStatus(short code = 601);
	bool keepAlive = false;
public:
	class Task;
	Uploader(string host, string username, string password, string root="/");
	Uploader(string host, unsigned short port, string username, string password, string root = "/");
	~Uploader();
	function<void (void)> create(Task* upload);
};

class Uploader::Task {
public:
	string path;
	string dist;
	char mode = 0;
	bool append = false;
	sf::Ftp::Response res;
	enum Status { WAIT, PROGRESS, FAILED, SUCCESS } status = WAIT;
	Task(string path, string dist, char mode = 0, bool append = false);
};

void uploadExample();