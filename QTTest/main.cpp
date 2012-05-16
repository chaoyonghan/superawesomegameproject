#include <QApplication>
#include <QWidget>
#include "ogrewidget.h"
#include <QVBoxLayout>
#include "MyWidget.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MyMainWindow window;
	window.show();

	return app.exec();
}
