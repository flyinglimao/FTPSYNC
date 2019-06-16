/*
* uploader.cpp 上傳器
* FTP 相關的上傳邏輯
*/

#include "uploader.h"

Uploader::Uploader(string host, unsigned short port, string username, string password, string root) {
	this->host = host;
	this->port = port;
	this->username = username;
	this->password = password;
	this->root = root;
	res = ftp.connect(host, port);
	if (res.isOk()) {
		cout << "Connected\n";
	}
	else {
		BadStatus();
	}
	
	res = ftp.login(username, password);
	if (res.isOk()) {
		cout << "Authed\n";
	}
	else {
		BadStatus(602);
	}

	res = ftp.keepAlive();
	if (res.isOk()) {
		cout << "Keep Alive enabled\n";
		keepAlive = true;
	}
	else {
		cout << "Keep Alive failed\n";
		cout << "Response (Ref: sf::Ftp::Response) : " << res.getStatus() << endl;
		cout << "It may cause upload fail\n";
	};

	res = ftp.changeDirectory(root);
}

Uploader::Uploader()
{
}

Uploader::Uploader(string host, string username, string password, string root)
	: Uploader::Uploader(host, 21, username, password, root) {};

Uploader::~Uploader() {
}

void Uploader::BadStatus(short code) {
	cout << "Auth failed\n";
	cout << "Response (Ref: sf::Ftp::Response) : " << res.getStatus() << endl;
	throw code;
}

function<void(void)> Uploader::create(Task* upload) {
	return [=] {
		upload->status = Task::Status::PROGRESS;
		string filename;
		string folder;
		string error = "Unknown Failed in Upload";
		short tmp;

		// Create Folder if not exist
		for (tmp = upload->path.length() - 1; tmp >= 0; tmp--) {
			if (upload->path[tmp] == '/') break;
		}
		filename = upload->path.substr(tmp + 1);

		for (tmp = upload->dist.length() - 1; tmp >= 0; tmp--) {
			if (upload->dist[tmp] == '/') break;
		}
		folder = upload->dist.substr(0, tmp + 1);

		res = ftp.getDirectoryListing(folder);
		if (!res.isOk()) {
			res = ftp.createDirectory(folder);
			if (!res.isOk()) {
				error = "Create folder failed";
			}
		}

		// Start Upload
		upload->res = ftp.upload(upload->path, "./", sf::Ftp::TransferMode(upload->mode), upload->append);
		if (upload->res.isOk()) {
			upload->res = ftp.renameFile(filename, upload->dist);
			if (upload->res.isOk()) {
				upload->status = Task::Status::SUCCESS;
				return;
			}
			else if (upload->res.getStatus() == 553){
				upload->res = ftp.deleteFile(upload->dist);
				if (upload->res.isOk()) {
					upload->res = ftp.renameFile(filename, upload->dist);
					if (upload->res.isOk()) {
						upload->status = Task::Status::SUCCESS;
						return;
					}
					else {
						error = "Rename file failed";
					}
				}
				else {
					error = "Delete old file failed";
				}
			}
			else {
				error = "Rename file failed";
			}
		}
		else {
			error = "Upload file failed";
		}


		upload->status = Task::Status::FAILED;
		throw error;
	};
}

Uploader::Task::Task(string path, string dist, char mode, bool append) {
	this->path = path;
	this->dist = dist;
	this->mode = mode;
	this->append = append;
}
