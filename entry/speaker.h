#pragma once
/*
* Speaker 發信器
* 透過 IPC 傳遞訊息
*/
#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <thread>
#include "ipc.h"

using namespace std;

class Speaker {
public:
	class Channel;
	class Command;
	Speaker(size_t size);
	~Speaker();
	Channel* createChannel(string name, string back);
private:
	vector<Channel*> channels;
	size_t size;
};


class Speaker::Channel {
private:
	ipc_sharedmemory data;
	ipc_sharedmemory feedback;
	string name;
	string back;
	size_t size;
public:
	Channel(string name, string back, size_t size);
	~Channel();
	unsigned char* retrieve();
	unsigned char* send(unsigned char* data);
};
