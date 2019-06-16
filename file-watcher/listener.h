#pragma once
/*
* listener 指令監聽
* 啟動指令監聽，並註冊回呼
*/
#include <functional>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>
#include "ipc.h"

using namespace std;

class Listener {
private:
	ipc_sharedmemory data;
	ipc_sharedmemory feedback;
	string name;
	string back;
	size_t size = 0;
	thread* worker = 0;
	void Worker();
	class Event;
	class Callback;
	vector<Event*> events;
public:
	Listener(string name, string back, size_t size);
	~Listener();
	Callback on(char op, function<void (unsigned char*)> callback);
	void send(unsigned char* data);
};

class Listener::Event {
public:
	char op = 0;
	vector<function<void(unsigned char*)>> callbacks;
};

class Listener::Callback {
public:
	Callback(function<void(void)> off);
	function<void(void)> off;
};