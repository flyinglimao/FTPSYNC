/*
*  main.cpp 程式主要進入點
* 沒什麼特別的，就是啟動其他功能的地方。
*/

#include <iostream>
#include <string>
#include "manager.h"
#include "uploader.h"

using namespace std;

int main(int argc, char* argv[]) {
	Uploader up("localhost", "test", "test");
	Uploader::Task demo("C:/kms10.log", "test/test/xd.log");
	up.create(&demo)();
	cout << demo.status;
}