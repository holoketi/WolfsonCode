#include "KFullScreenDialog.h"


KFullScreenDisplayDialog::KFullScreenDisplayDialog(QWidget *parent, QWidget* shareWidget) : QDialog(parent)
{
	setGeometry(100, 100, 100, 100);

	setWindowTitle(tr("Display Image"));
	QHBoxLayout* box_layout_ = new QHBoxLayout(this);
	box_layout_->setContentsMargins(0, 0, 0, 0);
	display_widget_ = new KGLWidget();
	box_layout_->addWidget(display_widget_);
	display_widget_->update();
}

KFullScreenDisplayDialog::~KFullScreenDisplayDialog()
{
}

void KFullScreenDisplayDialog::setTexture(uchar* data, int w, int h, bool flip)
{
	display_widget_->setTexture(data, w, h, flip);
	display_widget_->update();
	display_widget_->activateWindow();
	display_widget_->raise();
}