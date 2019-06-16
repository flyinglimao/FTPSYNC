/*
*  main.cpp 程式主要進入點
* 沒什麼特別的，就是啟動其他功能的地方。
*/

#include <iostream>
#include <string>
#include "manager.h"
#include "uploader.h"
#include "listener.h"

using namespace std;

/* OP
*   S setting ftp detail
*       768 host 256 account 256 password 2 port 766 root
*   U upload
*       1024 location 1024 remote path
*   T change thread max
*       2 count
*/

/* Back
*   S Success
*       2048 message
*   E Error
*       2048 message
*/

int main(int argc, char* argv[]) {
	Listener entry("ftpsync_ftp_client", "ftpsync_ftp_client_back", 2049);
	Manager manager(4);
	Uploader* uploader = 0;

	entry.on('S', [&](unsigned char* input) {
		if (uploader) delete uploader;
		string host = reinterpret_cast<const char*>(input);
		string account = reinterpret_cast<const char*>(input + 768);
		string password = reinterpret_cast<const char*>(input + 768 + 256);
		unsigned short port = input[768 + 256 + 256] * 256 + input[768 + 256 + 256 + 1];
		port = port ? port : 21;
		string root = reinterpret_cast<const char*>(input + 768 + 256 + 256 + 3);
		try {
			uploader = new Uploader(host, port, account, password, root);
			entry.send((unsigned char*)"S");
		}
		catch (short error) {
			unsigned char msg[2049] = "";
			msg[0] = 'E';
			string code = to_string(error);
			memcpy(&msg[1], code.c_str(), sizeof(code.c_str()));
			entry.send(msg);
		}
	});

	entry.on('U', [&](unsigned char* input) {
		string src = reinterpret_cast<const char*>(input);
		string dest = reinterpret_cast<const char*>(input + 1024);
		unsigned short id;
		Uploader::Task* task = new Uploader::Task(src, dest);
		manager.exec(
			uploader->create(task),
		[&, src, task]() {
			unsigned char msg[2049] = "";
			msg[0] = 'S';
			memcpy(&msg[1], src.c_str(), sizeof(src.c_str()));
			entry.send(msg);
			delete task;
		}, [&, src, task](string err) {
			unsigned char msg[2049] = "";
			msg[0] = 'E';
			memcpy(&msg[1], src.c_str(), sizeof(src.c_str()));
			entry.send(msg);
			delete task;
		}, &id);
	});

	entry.on('T', [&](unsigned char* input) {
		unsigned short max = input[0] * 256 + input[1];
		manager.adjustMax(max);
		entry.send((unsigned char*)"S");
	});


	while (1) 
		this_thread::sleep_for(chrono::microseconds(5));
}