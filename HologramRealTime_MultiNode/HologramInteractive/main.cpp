#include "holograminteractive.h"
#include <QtWidgets/QApplication>
#include <GL/glut.h>

QApplication* kQtApplication;
QMainWindow* kMainWindow;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	AllocConsole();
	kQtApplication = new QApplication(argc, argv);

	HologramInteractive w;
	kMainWindow = &w;
	w.show();

	return kQtApplication->exec();
}
