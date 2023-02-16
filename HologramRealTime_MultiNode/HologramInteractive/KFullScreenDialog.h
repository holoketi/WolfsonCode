#pragma once

#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qboxlayout.h>
#include <GL/glew.h>
#include "KGLWidget.h"

class KFullScreenDisplayDialog : public QDialog
{
	Q_OBJECT

public:

	KFullScreenDisplayDialog(QWidget *parent, QWidget* shareWidget = 0);

	virtual ~KFullScreenDisplayDialog();

	void resize();

	void setTexture(uchar* data, int w, int h, bool flip = false);
	void setTexture(unsigned int tx) { display_widget_->setTexture(tx); display_widget_->update(); }
	
private:
	
	KGLWidget* display_widget_;
	QHBoxLayout* box_layout_;
};