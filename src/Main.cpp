//Visualisierung  WS15 Carla Jancik und Bettina Schlager
#include "MainWindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	MainWindow mainWindow;
	mainWindow.show();

	return app.exec();

}
