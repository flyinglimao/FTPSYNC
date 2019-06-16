#pragma warning(disable : 4996) // to use sscanf
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/process.hpp>
#include "speaker.h"

using namespace std;

int main(int argc, char* argv[]) {
	ifstream config;
	if (argc >= 2) {
		config.open(argv[1]);

		if (config.fail()) {
			cout << argv[1] << " open failed\n";
			return -1;
		}
	}
	else {
		config.open("config.txt");

		if (config.fail()) {
			cout << "config.txt" << " open failed\n";
			return -1;
		}
	}
	string watchFolder, ftpHost, ftpAccount, ftpPassword, ftpPortRaw, ftpRoot, input;
	string ftpClient = "ftp-client.exe", fileMonitor = "file-watcher.exe";
	char key[15], value[1024];
	unsigned short ftpPort = 21;

	while (getline(config, input)) {
		sscanf(input.c_str(), "%s = %s", key, value);
		if (memcmp(key, "WATCH", 5) == 0) watchFolder = string(value);
		if (memcmp(key, "FTPHOST", 7) == 0) ftpHost = string(value);
		if (memcmp(key, "FTPACCOUNT", 10) == 0) ftpAccount = string(value);
		if (memcmp(key, "FTPPASSWORD", 11) == 0) ftpPassword = string(value);
		if (memcmp(key, "FTPPORT", 7) == 0) ftpPortRaw = string(value);
		if (memcmp(key, "FTPROOT", 7) == 0) ftpRoot = string(value);
		if (memcmp(key, "FTPCLIENT", 9) == 0) ftpClient = string(value);
		if (memcmp(key, "FILEMONITOR", 11) == 0) fileMonitor = string(value);
	}

	if (watchFolder.length() == 0) {
		cout << "Watching folder didn't set\n";
		return -2;
	}

	Speaker speaker(2049);
	Speaker::Channel* fm = speaker.createChannel("ftpsync_file_monitor", "ftpsync_file_monitor_back");
	Speaker::Channel* ftp = speaker.createChannel("ftpsync_ftp_client", "ftpsync_ftp_client_back");

	boost::process::spawn(ftpClient);
	boost::process::spawn(fileMonitor);

	unsigned char cmd[2049] = "S";
	unsigned char* res;
	memcpy(cmd + 1, watchFolder.c_str(), watchFolder.length());

	res = fm->send(cmd);
	if (res[0] == 'S') {
		cout << "connect to file monitor sucessed\n";
	}
	else {
		cout << "connect to file monitor failed\n";
		return -3;
	}
	delete[] res;
	memset(cmd, 0, sizeof(cmd));

	cmd[0] = 'S';
	memcpy(cmd + 1, ftpHost.c_str(), ftpHost.length());
	memcpy(cmd + 769, ftpAccount.c_str(), ftpAccount.length());
	memcpy(cmd + 1025, ftpPassword.c_str(), ftpAccount.length());
	memset(cmd + 1281, 0, 1);
	memset(cmd + 1282, 21, 1);
	memcpy(cmd + 1283, ftpRoot.c_str(), ftpRoot.length());
	res = ftp->send(cmd);
	if (res[0] == 'S') {
		cout << "connect to ftp succssed\n";
	}
	else {
		cout << "connect to ftp failed\n";
		return -4;
	}
	delete[] res;
	memset(cmd, 0, sizeof(cmd));

	while (1) {
		this_thread::sleep_for(chrono::microseconds(5));
		cmd[0] = 'R';
		res = fm->send(cmd);
		if (res[1]) {
			string toUpload = string((char*)(res + 1));
			string dest;
			short i;
			for (i = toUpload.length(); toUpload[i] != '/'; i--);
			dest = toUpload.substr(i+1);
			for (i = 0; i < dest.length(); i++) {
				if (dest[i] == '_') dest[i] = '/';
			}
			cmd[0] = 'U';
			memcpy(cmd + 1, toUpload.c_str(), toUpload.length());
			memcpy(cmd + 1025, dest.c_str(), dest.length());
			res = ftp->send(cmd);
			if (res[0] == 'E') { // ­«¶Ç
				delete[] res;
				res = ftp->send(cmd);
				if (res[0] == 'E') {
					delete[] res;
					cout << "Upload failed twice: " << toUpload << endl;
				}
			}
			else {
				delete[] res;
				memset(cmd, 0, sizeof(cmd));
			}
		}
	}
}
