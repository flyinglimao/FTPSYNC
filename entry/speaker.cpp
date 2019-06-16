/*
* Speaker 發信器
* 透過 IPC 傳遞訊息
*/

#define IPC_IMPLEMENTATION
#include "ipc.h"
#include "speaker.h"

Speaker::Speaker(size_t size) {
	this->size = size;
}

Speaker::~Speaker() {
	channels.clear();
}

Speaker::Channel* Speaker::createChannel(string name, string back) {
	Channel* newChannel = new Channel(name, back, this->size);
	channels.push_back(newChannel);
	return newChannel;
}

Speaker::Channel::Channel(string name, string back, size_t size) {
	this->name = name;
	this->back = back;
	this->size = size;
	ipc_mem_init(&data, const_cast<char*>(name.c_str()), size);
	ipc_mem_init(&feedback, const_cast<char*>(back.c_str()), size);

	// 指令空間
	if (ipc_mem_open_existing(&data)) {
		if (ipc_mem_create(&data)) {
			cout << "IPC " << name << " create failed\n";
			throw "IPC create failed";
		}
	}
	else {
		cout << "IPC " << name << " existed\n";
	}

	// 回傳空間
	if (ipc_mem_open_existing(&feedback)) {
		if (ipc_mem_create(&feedback)) {
			cout << "IPC " << name << " create failed\n";
			throw "IPC create failed";
		}
	}
	else {
		cout << "IPC " << name << " existed\n";
	}

}

Speaker::Channel::~Channel() {
	ipc_mem_close(&data);
	ipc_mem_close(&feedback);
}


unsigned char* Speaker::Channel::retrieve() {
	return feedback.data;
}

unsigned char* Speaker::Channel::send(unsigned char* data) {
	memcpy(this->data.data, data, size);
	while (1) {
		this_thread::sleep_for(chrono::microseconds(5)); // 抑制 CPU 消耗
		if (feedback.data[0]) {
			unsigned char* temp = new unsigned char[2049];
			memcpy(temp, feedback.data, size);
			memset(feedback.data, 0, size);
			return temp;
		}
	}
}