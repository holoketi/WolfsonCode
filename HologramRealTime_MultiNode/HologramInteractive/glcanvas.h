#pragma once

#include "GL/glew.h"
#include "view_controller/View.h"
#include "graphics/keystate.h"

#include <QtOpenGL/QGLWidget>
#include <QtGui/QKeyEvent> 
#include <QtGui/QMouseEvent>
#include <QtCore/QTimer>

class GLCanvas : public QGLWidget
{
	Q_OBJECT

public:
	GLCanvas(QWidget *parent = 0);
	~GLCanvas();

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void setSpecialKey(QKeyEvent* key);


public slots:

	void idle();


private:

	View*		viewer_;
	QTimer*		timer_;

	bool		initialized;

	bool		InitGL(GLvoid);

	KeyState	keystate_;


};