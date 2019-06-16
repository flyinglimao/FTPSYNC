#pragma once
#include <QString>
#include "provider.h"
#include "listener.h"
#include "watcher.h"
Provider::Provider(QObject* parent) : QThread(parent) {

}

void Provider::run() {
	try {
		entry->on('R', [&](unsigned char* input) {
			unsigned char temp[2049] = "S";
			QString filename = monitor->getNextFile();
			if (filename.length()) {
				QString path = monitor->base + filename;
				memcpy(temp + 1, path.toStdString().c_str(), path.length());
			}
			entry->send(temp);
		});
		while (1) {
			this_thread::sleep_for(chrono::microseconds(5));
		}
	} catch(...) {
		emit done();
	}
}
