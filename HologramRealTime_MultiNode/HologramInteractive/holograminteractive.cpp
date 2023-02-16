#include "holograminteractive.h"
#include "KFullScreenDialog.h"
#include "view_controller/Controller.h"

#include <QtWidgets/QFileDialog>

HologramInteractive::HologramInteractive(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	canvas = new GLCanvas();
	setCentralWidget(canvas);

	connect(ui.actionOpen_Model, SIGNAL(triggered()), this, SLOT(OpenModel()));
	connect(ui.actionGeneration, SIGNAL(triggered()), this, SLOT(GenCGH()));
	connect(ui.actionReconstruction, SIGNAL(triggered()), this, SLOT(Recon()));

}
HologramInteractive::~HologramInteractive()
{
}


void HologramInteractive::OpenModel()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Model Files"), ".", tr("Collada Files (*.dae);;All Files (*.*)"));

	if (filename.isEmpty())	return;

	kMainController->OpenModel(filename);

}

void HologramInteractive::GenCGH()
{
	kMainController->GenCGH();
	   
}

void HologramInteractive::Recon()
{
	kMainController->Recon();
}
