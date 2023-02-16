
#include    <math.h>
#include    <graphics/sys.h>
#include    <graphics/misc.h>
#include	<graphics/geom.h>
#include    <graphics/Camera.h>
#include	<graphics/camera_map.h>
#include	<graphics/font.h>

#include    "view_controller/View.h"

#include	<IL/il.h>
#include	<IL/ilu.h>
#include	<IL/ilut.h>
#include	<QtWidgets/qwidget.h>

View::View(int w, int h) :	need_redraw_(true),	camera_(w, h)
{
}

void View::Initialize()
{
	InitializeFreeType("c:/windows/fonts/arial.ttf");

	camera_.set_camera_mode(Camera::kPerspective);

	glViewport(camera_.getViewport().GetX(),
		camera_.getViewport().GetY(),
		camera_.getViewport().GetWidth(),
		camera_.getViewport().GetHeight());

	camera_.ResizeViewport(camera_.getViewport().GetWidth(),
		camera_.getViewport().GetHeight());
	
	camera_.set_view();

	set_need_redraw();
}

const Camera& View::GetCamera() const
{
	return camera_;
}

Camera& View::GetCamera()
{
	return camera_;
}

void View::set_need_redraw(bool val)
{
	need_redraw_ = val;
}

bool View::need_redraw() const
{
	return need_redraw_;
}

void View::clear_need_redraw()
{
	set_need_redraw(false);
}

static void SetFrontLight()
{
	int vport[4];
	double model[16];
	double proj[16];
	glGetIntegerv(GL_VIEWPORT, vport);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	double p[3];
	gluUnProject(vport[2] / 2, vport[3] / 2, 0, model, proj, vport,
		&p[0], &p[1], &p[2]);

	float light[4];
	light[0] = p[0];
	light[1] = p[1];
	light[2] = p[2];
	light[3] = 0;
	float diffuse[4] = { 0.5, 0.5, 0.5, 0.5 };
	float ambient[4] = { 0.2, 0.2, 0.2, 0.5 };
	float spec[4] = { 0.5, 0.5, 0.5, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
}


void View::Render()
{
	if (!need_redraw()) return;

	Render2D();
}

void View::Render2D()
{
}


void View::Resize(int w, int h)
{
	LOG("resize called %p\n", this);

	camera_.ResizeViewport(0, 0, w, h);
	camera_.set_view();
	camera_.need_update(true);

	set_need_redraw();

}

int View::GetWidth() const
{
	return camera_.GetWidth();
}

int View::GetHeight() const
{
	return camera_.GetHeight();
}

void View::Resize(int x, int y, int w, int h)
{
	camera_.ResizeViewport(x, y, w, h);
	camera_.set_view();
	camera_.need_update(true);

	set_need_redraw();

}

bool View::ProcessEvent(Event* ev)
{
	ev->y = camera_.height() - ev->y;

	ivec2 pos = ivec2(ev->x, ev->y);

	// convert window coordinates to viewport coordinates
	pos = camera_.getViewport().windowToViewPort(pos);

	if (ev->event_raised_by_ == kMouse_Move) {
		MoveEventProc(pos, ev->get_key_state().get_shift_pressed(), ev->get_key_state().get_ctrl_pressed(), ev->get_key_state().get_alt_pressed());
		return false;
	}

	if (ev->event_raised_by_ == kMouse_Left_Button_Down) {
		DownEventProc(pos, ev->get_key_state().get_shift_pressed(), ev->get_key_state().get_ctrl_pressed(), ev->get_key_state().get_alt_pressed());
		return false;
	}

	if (ev->event_raised_by_ == kMouse_Button_Up) {
		UpEventProc(pos, ev->get_key_state().get_shift_pressed(), ev->get_key_state().get_ctrl_pressed(), ev->get_key_state().get_alt_pressed());
		return false;
	}

	if (ev->event_raised_by_ == kMouse_Right_Button_Down) {
		RightDownEventProc(pos, ev->get_key_state().get_shift_pressed(), ev->get_key_state().get_ctrl_pressed(), ev->get_key_state().get_alt_pressed());
		return false;
	}

	//if (ev->event_raised_by_ == kMouse_Left_Button_Double_Click) {
	//	DoubleClickEventProc(pos, ev->get_key_state().get_shift_pressed(), ev->get_key_state().get_ctrl_pressed(), ev->get_key_state().get_alt_pressed());
	//	return false;
	//}

	if (ev->event_raised_by_ == kChar) {
		CharacterInputEventProc(ev);
		return false;
	}
	if (ev->event_raised_by_ == kMouse_Wheel) {
		WheelEventProc(ev->wheel_delta_, pos);
		return false;
	}
	if (ev->event_raised_by_ == kKeyboard_Down) {
		KeyboardDownEventProc(ev);
		return false;
	}
	if (ev->event_raised_by_ == kKeyboard_Up) {
		KeyboardUpEventProc(ev);
		return false;
	}

}

void View::KeyboardDownEventProc(graphics::Event *ev)
{
}

void View::KeyboardUpEventProc(graphics::Event *ev)
{
	set_need_redraw();
}

void View::CharacterInputEventProc(graphics::Event *ev)
{
	set_need_redraw();
}

void View::WheelEventProc(int zdelta, const ivec2& pos)
{
	set_need_redraw();
}

void View::DoubleClickEventProc(const ivec2& p, int shift, int cont, int alt)
{
	set_need_redraw();
}

void View::MoveEventProc(const ivec2& p, int shift, int cont, int alt)
{
	pre_p = p;
}

void View::RightDownEventProc(const ivec2& p, int shift, int cont, int alt)
{
}

void View::DownEventProc(const ivec2& p, int shift, int cont, int alt)
{
}

void View::MiddleDownEventProc(const ivec2& p, int shift, int cont, int alt)
{
}

void View::UpEventProc(const ivec2& p, int shift, int cont, int alt)
{
}
