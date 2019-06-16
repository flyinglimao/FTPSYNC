#include "watcher.h"

Watcher::FileSystemWatcher::FileSystemWatcher(Watcher* father, QObject* parent = 0) : QObject(parent) {
	this->parent = father;
};

void Watcher::FileSystemWatcher::addPath(QString& path) {
	fsw.addPath(path);
}

Watcher::Watcher(QString& path) {
	base = path;
	watcher = new FileSystemWatcher(this);
	watcher->addPath(path);
	QObject::connect(&(watcher->fsw), SIGNAL(directoryChanged(QString)), watcher, SLOT(update(QString)), Qt::DirectConnection);
	QObject::connect(&(watcher->fsw), SIGNAL(fileChanged(QString)), watcher, SLOT(update(QString)), Qt::DirectConnection);
	QDir monitored(path);
	QStringList files = monitored.entryList(QDir::Files);
	compareSet = QSet<QString>::fromList(files);
}

void Watcher::FileSystemWatcher::update(const QString& path) {
	QDir monitored(path);
	QStringList files = monitored.entryList(QDir::Files);
	QSet<QString> newSet = QSet<QString>::fromList(files);
	QStringList diff = (newSet - parent->compareSet).toList();
	foreach(QString file, diff) {
		parent->fileQueue.enqueue(file);
	}
	parent->compareSet = newSet;
}

QString Watcher::getNextFile() {
	if (fileQueue.size()) {
		return fileQueue.dequeue();
	}
	else {
		return "";
	}
}
