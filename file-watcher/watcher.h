#pragma once
#include <QtCore/QCoreApplication>
#include <QFileSystemWatcher>
#include <QQueue>
#include <QDebug>
#include <QString>
#include <QSet>
#include <QDir>

using namespace std;

class Watcher {
public:
	Watcher(QString& path);
	class FileSystemWatcher;
	QString base;
	QString getNextFile();
private:
	FileSystemWatcher* watcher;
	QSet<QString> compareSet;
	QQueue<QString> fileQueue;
};

class Watcher::FileSystemWatcher : public QObject{
	Q_OBJECT
public:
	FileSystemWatcher(Watcher* father, QObject* parent);
	void addPath(QString& path);
	QFileSystemWatcher fsw;
	Watcher* parent;
public slots:
	void update(const QString& path);
};
