
#include <QtWidgets/QApplication>
#include "holo_server/renderwidget.h"
#include "holo_server/hologram_server.h"

QApplication* kQtApplication;
HologramServer* kHologramServer;

int main(int argc, char *argv[])
{

	kQtApplication = new QApplication(argc, argv);

    kHologramServer = new HologramServer();
    if (!kHologramServer->listen(QHostAddress::Any, 6178)) {
        return 1;
    }

	return kQtApplication->exec();
}
