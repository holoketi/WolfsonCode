#pragma once

#include <GL/glew.h>
#include <QtOpenGL/qgl.h>
#include <graphics/gl.h>

using namespace graphics;

class KGLWidget : public QGLWidget
{
	Q_OBJECT


public:
	KGLWidget(QWidget *parent = 0, QWidget* shareWidget = 0);
	~KGLWidget();

	void setTexture(uchar* data, int w, int h, bool flip = false);
	void setTexture(unsigned int tx) {
		texture_ = tx;
	}
protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void updateTexture();
protected:


private:

	unsigned int texture_;
	unsigned char* image_;
	bool need_update_;
	int w_, h_;
	bool flip_;
};