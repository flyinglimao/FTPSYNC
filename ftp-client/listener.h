#pragma once
/*
* listener ���O��ť
* �Ұʫ��O��ť�A�æ^�I
*/
#include <functional>
#include <fstream>
#include <string>
#include <SFML/Network.hpp>
#define IPC_IMPLEMENTATION
#include "ipc.h"

/*
* IPC ����
* name: ftpsync_ftp_client
* 128 * 1024 char �r��Ŷ�
* 2 char �лx�Y��
* 1 char �лx���A
* �@ 1028*1024 + 3 char �e
*/

class Listener {
private:
	
public:
	function<void (void)> on(string name, function<void (void)> callback);
};