#include <QtCore/QCoreApplication>
#include <QNetworkAccessManager>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QNetworkAccessManager test;
	return a.exec();
}
