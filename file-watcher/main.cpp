#include <QtCore/QCoreApplication>
#include <QThread>
#include "provider.h"
#include "watcher.h"
/* OP
*   S set monitor folder
*       2048 path
*   R retrieve queued file
*       0
*   A retrieve all file
*       0
*/

/* Back
*   S Success
*       2048 message
*   E Error
*       2048 message
*/

int main(int argc, char *argv[])
{
	QCoreApplication qt(argc, argv);
	Listener entry("ftpsync_file_monitor", "ftpsync_file_monitor_back", 2049);
	Provider provider;

	unsigned char path[2048] = { 0 };
	entry.on('S', [&](unsigned char* input) {
		memcpy(path, input, sizeof(path));
		entry.send((unsigned char*)"Success");
	});

	while (!path[0]) {
		this_thread::sleep_for(chrono::microseconds(5));
	}

	Watcher* monitor = new Watcher(QString::fromStdString((char*)path));

	provider.entry = &entry;
	provider.monitor = monitor;
	provider.start();
	qt.exec();
	return 0;
}