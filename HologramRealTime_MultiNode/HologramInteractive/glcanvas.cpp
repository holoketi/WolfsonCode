#include "GLCanvas.h"
#include "view_controller/View3D.h"
#include "view_controller/Controller.h"

#include <QtWidgets/qwidget.h>
#include <QtGui/QPainter>
#include <QtWidgets/qmainwindow.h>

extern QApplication* kQtApplication;
extern QMainWindow* kMainWindow;

bool kFullScreenMode;

GLCanvas::GLCanvas(QWidget *parent)
	: QGLWidget(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba | QGL::AlphaChannel | QGL::SampleBuffers), parent)
{
	glewInit();
	timer_ = new QTimer((QObject*)kQtApplication);

	connect(timer_, SIGNAL(timeout()), this, SLOT(idle()));
	timer_->start();

	keystate_.set_alt_pressed(false);
	keystate_.set_shift_pressed(false);
	keystate_.set_ctrl_pressed(false);

	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

	viewer_ = new View3D();
	
}

GLCanvas::~GLCanvas()
{
}

void GLCanvas::idle()
{
	makeCurrent();
	viewer_->Update();
	updateGL();
}

void GLCanvas::initializeGL()
{
	makeCurrent();
	viewer_->Initialize();
}

QSize GLCanvas::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLCanvas::sizeHint() const
{
	return QSize(400, 400);
}

void GLCanvas::paintGL()
{
	makeCurrent();
	viewer_->Render();
}

void GLCanvas::resizeGL(int width, int height)
{
	makeCurrent();
	viewer_->Resize(width, height);
}

void GLCanvas::mousePressEvent(QMouseEvent *event)
{
	makeCurrent();
	Event ev;
	ev.x = event->x();
	ev.y = event->y();

	if (event->buttons() == Qt::LeftButton)
		ev.event_raised_by_ = kMouse_Left_Button_Down;
	else if (event->buttons() == Qt::RightButton)
		ev.event_raised_by_ = kMouse_Right_Button_Down;
	//else if (event->buttons() == Qt::MiddleButton)
	//	ev.event_raised_by_ = kMouse_Middle_Button_Down;

	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	updateGL();
}

void GLCanvas::mouseReleaseEvent(QMouseEvent *event)
{
	makeCurrent();
	Event ev;
	ev.x = event->x();
	ev.y = event->y();

	ev.event_raised_by_ = kMouse_Button_Up;
	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	
	updateGL();
	

}

void GLCanvas::mouseMoveEvent(QMouseEvent *event)
{
	//LOG("mouse move event\n");
	makeCurrent();
	Event ev;
	ev.x = event->x();
	ev.y = event->y();

	ev.event_raised_by_ = kMouse_Move;
	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	updateGL();
}

void GLCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
	/*
	makeCurrent();
	Event ev;
	ev.x = event->x();
	ev.y = event->y();


	ev.event_raised_by_ = kMouse_Left_Button_Double_Click;
	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	updateGL();
	*/
}

void GLCanvas::wheelEvent(QWheelEvent* event)
{
	
	makeCurrent();
	Event ev;
	ev.x = event->x();
	ev.y = event->y();
	ev.event_raised_by_ = kMouse_Wheel;
	ev.wheel_delta_ = event->delta();

	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	updateGL();

	//LOG("Wheel delta=%d\n", ev.wheel_delta_);
	
}


void GLCanvas::keyPressEvent(QKeyEvent* key)
{
	makeCurrent();

	Event ev;
	if (key->key() >= Qt::Key_Space && key->key() <= Qt::Key_AsciiTilde)	 // handle ASCII char like keys
	{
		ev.character_code_ = static_cast<wchar_t>(key->key());

	}
	else {

		ev.character_code_ = static_cast<wchar_t>(key->nativeVirtualKey());

	}
	if (ev.character_code_ == 27) exit(0);
	//LOG("key down : %i\n", ev.character_code_);

	if (ev.character_code_ == 32) {	// space
		kFullScreenMode = kFullScreenMode ? false : true;
		if (kFullScreenMode)
		{
			kMainWindow->showFullScreen();
		}
		else {
			kMainWindow->showNormal();
		}
	}

	setSpecialKey(key);
	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	ev.event_raised_by_ = kChar;
	viewer_->ProcessEvent(&ev);

	ev.event_raised_by_ = kKeyboard_Down;
	viewer_->ProcessEvent(&ev);

	updateGL();
}

void GLCanvas::keyReleaseEvent(QKeyEvent* key)
{
	makeCurrent();
	//LOG("key up : %i\n", key->nativeVirtualKey());

	Event ev;
	ev.event_raised_by_ = kKeyboard_Up;
	ev.character_code_ = static_cast<wchar_t>(key->nativeVirtualKey());

	setSpecialKey(key);
	ev.set_key_state(keystate_.get_shift_pressed(), keystate_.get_alt_pressed(), keystate_.get_ctrl_pressed());

	viewer_->ProcessEvent(&ev);
	updateGL();

}

void GLCanvas::setSpecialKey(QKeyEvent* key)
{
	if (key == 0)	return;

	keystate_.set_alt_pressed(key->modifiers() & Qt::AltModifier);
	keystate_.set_shift_pressed(key->modifiers() & Qt::ShiftModifier);
	keystate_.set_ctrl_pressed(key->modifiers() & Qt::ControlModifier);

}


