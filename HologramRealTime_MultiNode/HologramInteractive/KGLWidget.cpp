#include <math.h>
#include <GL/glew.h>
#include <QtGui/QMouseEvent>
#include "KGLWidget.h"
#include "graphics/gl_extension.h"

static const real wheel_size = 250;
//! [0]
KGLWidget::KGLWidget(QWidget *parent, QWidget* shareWidget)
	: QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel), parent, (QGLWidget*)shareWidget), texture_(0), image_(0), need_update_(false), w_(0), h_(0)
{
	flip_ = false;
}
//! [0]


//! [1]
KGLWidget::~KGLWidget()
{
	if (image_) free(image_);
	image_ = 0;
	need_update_ = false;
}
//! [1]

//! [4]

//! [6]
void KGLWidget::initializeGL()
{
	makeCurrent();
	glExtensionInitialize();
}
//! [6]

void KGLWidget::setTexture(uchar* data, int w, int h, bool flip)
{
	flip_ = flip;
	if (!image_) image_ = (unsigned char*)malloc(w*h * 4);
	memcpy(image_, data, w*h * 4);
	w_ = w;
	h_ = h;

	need_update_ = true;
}

void KGLWidget::updateTexture()
{
	if (!need_update_) return;
	need_update_ = false;

	//if (!texture_) {
	if (texture_) glDeleteTextures(1, &texture_);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture_);
	//}
	glBindTexture(GL_TEXTURE_2D, texture_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w_,
		h_, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		image_);

}
//! [7]
void KGLWidget::paintGL()
{
	makeCurrent();

	updateTexture();
	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width(), 0, height(), -1000, 1000);

	glMatrixMode(GL_MODELVIEW);

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.875, 0.875, 0.875, 1.0);
	glClearColor(1, 1, 1, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	gl_color(vec3(1, 1, 1));

	if (texture_) {
		glActiveTexture(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, texture_);
	}

	if (!flip_) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		gl_vertex(vec2(0, 0));
		glTexCoord2f(1.0, 0.0);
		gl_vertex(vec2(width(), 0));
		glTexCoord2f(1.0, 1.0);
		gl_vertex(vec2(width(), height()));
		glTexCoord2f(0.0, 1.0);
		gl_vertex(vec2(0, height()));
		glEnd();
	}
	else {
		glBegin(GL_QUADS);
		glTexCoord2f(1.0, 1.0);
		gl_vertex(vec2(0, 0));
		glTexCoord2f(0.0, 1.0);
		gl_vertex(vec2(width(), 0));
		glTexCoord2f(0.0, 0.0);
		gl_vertex(vec2(width(), height()));
		glTexCoord2f(1.0, 0.0);
		gl_vertex(vec2(0, height()));
		glEnd();
	}

	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
	if (texture_) {
		glActiveTexture(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	//doneCurrent();
}
//! [7]

//! [8]
void KGLWidget::resizeGL(int width, int height)
{
	makeCurrent();
	update();
}
//! [8]
