#pragma once
#include <QThread>
#include "listener.h"
#include "watcher.h"

class Provider : public QThread {
	Q_OBJECT
public:
	Provider(QObject* parent = 0);
	Listener* entry;
	Watcher* monitor;
protected:
	void run();
signals:
	void done();
};
