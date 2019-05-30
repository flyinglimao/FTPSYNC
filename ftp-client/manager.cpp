/*
* manager 上傳任務管理
* 上傳任務的分配邏輯等。
*/
#include "manager.h"
#include <windows.h>
#include <iostream>
void Manager::Worker(int id) {
	while (1) {
		if (jobs.size()) {
			Job* toHandle = jobs.back();
			jobs.pop_back();
			toHandle->status = JOB::RUN;
			try {
				toHandle->object();
			}
			catch (exception e) {
				toHandle->reject();
				toHandle->status = JOB::DEAD;
				continue;
			}
			toHandle->status = JOB::DONE;
			toHandle->resolve();
		}
		if (id >= this->maxThreads) {
			break;
		}
	}
}

Manager::Manager(unsigned short maxThreads)
	: Manager(maxThreads, false)
{
};

Manager::~Manager() {
	while(workers.size()) {
		if (waitBeforeDelete)
			workers.at(workers.size()-1)->join();
		workers.pop_back();
	}
}

Manager::Manager(unsigned short maxThreads, bool wait) {
	this->maxThreads = maxThreads;
	this->waitBeforeDelete = wait;
	while (workers.size() < maxThreads) {
		thread* worker = new thread(&Manager::Worker, this, workers.size());
		workers.push_back(worker);
	}
}

void Manager::adjustMax(unsigned short threads) {
	maxThreads = threads;
	while (workers.size() < maxThreads) {
		thread* worker = new thread(&Manager::Worker, this, workers.size());
		workers.push_back(worker);
	}
	while (workers.size() > maxThreads) {
		workers.pop_back();
	}
}