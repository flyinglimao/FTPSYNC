/*
* uploader.cpp 上傳器
* FTP 相關的上傳邏輯
*/

#include "uploader.h"

Uploader::Uploader(string host, unsigned short port, string username, string password, string root) {
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

Uploader::Uploader(string host, string username, string password, string root)
	: Uploader::Uploader(host, 21, username, password, root) {};

Uploader::~Uploader() {
	res = ftp.disconnect();
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
		}

		// Start Upload
		upload->res = ftp.upload(upload->path, "./", sf::Ftp::TransferMode(upload->mode), upload->append);
		if (upload->res.isOk()) {
			upload->res = ftp.renameFile(filename, upload->dist);
			if (upload->res.isOk()) {
				upload->status = Task::Status::SUCCESS;
				return;
			}
		}
		upload->status = Task::Status::FAILED;
		throw 1;
	};
}

Uploader::Task::Task(string path, string dist, char mode, bool append) {
	this->path = path;
	this->dist = dist;
	this->mode = mode;
	this->append = append;
}

void uploadExample() {
	sf::Ftp ftp;
	cout << "connecting\n";
	sf::Ftp::Response res = ftp.connect("localhost", 990);
	cout << res.getStatus() << endl;

	res = ftp.login("test", "test");
	cout << res.getStatus() << endl;

	sf::Ftp::ListingResponse ls = ftp.getDirectoryListing();
	vector<string> lsv = ls.getListing();
	vector<string>::iterator lsv_h = lsv.begin();
	vector<string>::iterator lsv_e = lsv.end();
	vector<string>::iterator lsv_it;
	for (lsv_it = lsv_h; lsv_it != lsv_e; lsv_it++) cout << *lsv_it << endl;

}