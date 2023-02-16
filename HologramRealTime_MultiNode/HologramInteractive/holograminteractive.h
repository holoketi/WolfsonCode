#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_holograminteractive.h"
#include "glcanvas.h"

class HologramInteractive : public QMainWindow
{
	Q_OBJECT

public:
	HologramInteractive(QWidget *parent = Q_NULLPTR);
	~HologramInteractive();

public slots:

	void OpenModel();
	void GenCGH();
	void Recon();

private:

	Ui::HologramInteractiveClass ui;
	GLCanvas* canvas;
};
