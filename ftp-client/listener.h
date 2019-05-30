#pragma once
/*
* listener 指令監聽
* 啟動指令監聽，並回呼
*/
#include <functional>
#include <fstream>
#include <string>
#include <SFML/Network.hpp>
#define IPC_IMPLEMENTATION
#include "ipc.h"

/*
* IPC 說明
* name: ftpsync_ftp_client
* 128 * 1024 char 字串空間
* 2 char 標誌頭尾
* 1 char 標誌狀態
* 共 1028*1024 + 3 char 寬
*/

class Listener {
private:
	
public:
	function<void (void)> on(string name, function<void (void)> callback);
};