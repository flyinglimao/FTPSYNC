#pragma once
/*
* manager 上傳任務管理
* 上傳任務的分配邏輯等。
*/
#include<thread>
#include<vector>
#include<functional>

enum JOB{
	NEW, WAIT, RUN, DONE, DEAD
};

using namespace std;

class Job {
public:
	function<void (void)> object;
	function<void (void)> resolve;
	function<void (void)> reject;
	JOB status = JOB::NEW;
};

class Manager {
private:
	void Worker(int id);

	vector<Job*> jobs;
	unsigned short maxThreads;
	bool waitBeforeDelete = false;
	vector<thread*> workers;

public :
	Manager(unsigned short maxThreads);
	Manager(unsigned short maxThreads, bool wait);
	~Manager();
	JOB* exec(Job job, unsigned short* idptr);
	JOB* exec(function<void (void)> func, function<void (void)> resolve, function<void(void)> reject, unsigned short* idptr);
	JOB* how(unsigned short id);
	bool kill(unsigned short id);
	void adjustMax(unsigned short threads);
};