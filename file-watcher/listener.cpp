/*
* listener.cpp ���O��ť
* �Ұʫ��O��ť�A�õ��U�^�I
*/

#define IPC_IMPLEMENTATION
#include "ipc.h"
#include "listener.h"

Listener::Listener(string name, string back, size_t size) {
	this->name = name;
	this->back = back;
	this->size = size;
	ipc_mem_init(&data, const_cast<char*>(name.c_str()), size);
	ipc_mem_init(&feedback, const_cast<char*>(back.c_str()), size);

	// ���O�Ŷ�
	if (ipc_mem_open_existing(&data)) {
		cout << "IPC input not exists, should be create by speaker\n";
		throw "IPC input create failed";
	}
	else {
		cout << "IPC input exist\n";
	}

	// �^�X�Ŷ�
	if (ipc_mem_open_existing(&feedback)) {
		cout << "IPC output create failed, should be create by speaker\n";
		throw "IPC output create failed";
	}
	else {
		cout << "IPC output exist\n";
	}

	worker = new thread(&Listener::Worker, this);
}

Listener::~Listener() {
	cout << "Listener Die";
	worker->join();
	delete worker;
}

Listener::Callback Listener::on(char op, function<void(unsigned char*)> callback) {
	bool flag = false;
	Event* targetEvent = 0;
	for (unsigned int i = 0; i < events.size(); i++) {
		if (events.at(i)->op == op) {
			targetEvent = events.at(i);
			flag = true;
			break;
		}
	}

	if (!flag) {
		targetEvent = new Event;
		events.push_back(targetEvent);
		targetEvent->op = op;
	}

	targetEvent->callbacks.push_back(callback);

	return Callback([=] {
		for (unsigned int i = 0; i < targetEvent->callbacks.size(); i++) {
			if (&(targetEvent->callbacks.at(i)) == &(callback)) {
				targetEvent->callbacks.erase(targetEvent->callbacks.begin() + i);
			}
		}
	});
}

Listener::Callback::Callback(function<void(void)> off) {
	this->off = off;
}

void Listener::send(unsigned char* data) {
	memcpy(feedback.data, data, size);
}

void Listener::Worker() {
	while (1) {
		this_thread::sleep_for(chrono::microseconds(5)); // ��� CPU ����
		try {
			if (data.data[0]) {
				for (unsigned int i = 0; i < events.size(); i++) {
					if (events.at(i)->op == data.data[0]) {
						for (function<void(unsigned char*)> callback : events.at(i)->callbacks) {
							unsigned char* temp = new unsigned char[size-1];
							memcpy(temp, data.data + 1, size-1);
							memset(data.data, 0, size);
							callback(temp);
							delete[] temp;
						}
					}
				}
			}
		}
		catch (...) {
			cout << "Listener worker died\n";
			throw "Listener worker died";
		}
	}
}