/*
*  main.cpp �{���D�n�i�J�I
* �S����S�O���A�N�O�Ұʨ�L�\�઺�a��C
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