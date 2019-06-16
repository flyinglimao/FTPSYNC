/*
* manager 上傳任務管理
* 上傳任務的分配邏輯等。
*/
#include "manager.h"

void Manager::Worker(int id) {
	while (1) {
		this_thread::sleep_for(chrono::microseconds(5)); // 抑制 CPU 消耗
		if (jobs.size()) {
			Job* toHandle = jobs.back();
			jobs.pop_back();
			toHandle->status = JOB::RUN;
			try {
				toHandle->object();
			}
			catch (string e) {
				toHandle->reject(e);
				toHandle->status = JOB::DEAD;
				continue;
			}
			toHandle->status = JOB::DONE;
			toHandle->resolve();
		}
		else if (die && !waitBeforeDelete) {
			break;
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
	die = true;
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
		delete workers.back();
		workers.pop_back();
	}
}

JOB* Manager::exec(function<void(void)> func, function<void(void)> resolve, function<void(string)> reject, unsigned short* idptr) {
	Job* job = new Job;
	job->object = func;
	job->reject = reject;
	job->resolve = resolve;
	job->id = usedid++;
	return exec(job, idptr);
}

JOB* Manager::exec(Job* job, unsigned short* idptr) {
	jobs.push_back(job);
	*idptr = job->id;
	return &(job->status);
}

JOB Manager::how(unsigned short id) {
	for (unsigned int i = 0; i < jobs.size(); i++) {
		if (jobs.at(i)->id == id) {
			return jobs.at(i)->status;
		}
	}
	return JOB::NOTFOUND; // Not found
}

bool Manager::kill(unsigned short id) {
	for (unsigned int i = 0; i < jobs.size(); i++) {
		if (jobs.at(i)->id == id) {
			delete jobs.at(i);
			jobs.erase(jobs.begin() + i);
			return true;
		}
	}
	return false;
};