#include	"GL/glew.h"
#include    "view_controller/View3D.h"
#include    "view_controller/Controller.h"

#include    <math.h>
#include    <graphics/sys.h>
#include	<graphics/gl_extension.h>
#include    <graphics/gl.h>
#include    <graphics/misc.h>
#include    <graphics/gl_draw.h>
#include	<graphics/geom.h>
#include    <graphics/Camera.h>
#include	<graphics/camera_map.h>
#include	<graphics/event.h>
#include	<graphics/font.h>

#include	<IL/il.h>
#include	<IL/ilu.h>
#include	<IL/ilut.h>

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/QStatusBar>

extern QMainWindow* kMainWindow;
bool View3D::first = true;
void InitializeModelingKernel();

View3D::View3D(int w, int h) :
	View(w, h),
	state(VIEW_IDLE),
	draw_axes_(true)
{
}

void View3D::Initialize()
{
	::InitializeModelingKernel();

	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	glExtensionInitialize();
	//glViewportIndexedf();

	View::Initialize();
	   
	if (first) {
		first = false;
		kMainController->InitializeInteractiveModelingSession(this);

	}

	//camera_.set_camera_mode(Camera::kParallel);
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

void View3D::Render()
{
	if (!need_redraw()) return;

	graphics::gl_stat stat;
	stat.save_stat();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.875, 0.875, 0.875, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	camera_.DrawSkyView();

	glClear(GL_DEPTH_BUFFER_BIT);

	camera_.set_view();

	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	SetFrontLight();

	gl_color(vec4(1.0, 0.6, 0.0, 0.2));
	glLineWidth(1.0);
	glPointSize(1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	kMainController->Draw();
	
	DrawAxes();

	//kMainController->DrawTools();
	
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);

	//Render2D();

	clear_need_redraw();
	glPopAttrib();

	stat.restore_stat();
}

void View3D::Render2D()
{
	glViewport(camera_.getViewport().GetX(),
		camera_.getViewport().GetY(),
		camera_.getViewport().GetWidth(),
		camera_.getViewport().GetHeight());
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, camera_.getViewport().GetWidth(), 0, camera_.getViewport().GetHeight());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(0, 0, 0);

	glEnable(GL_TEXTURE_2D);
	kMainController->Draw2D();
	glEnable(GL_TEXTURE_2D);

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void View3D::KeyboardDownEventProc(graphics::Event *ev)
{
	/*
	LOG("keydown %d\n", ev->character_code_);

	if (ev->character_code_ == 46) {
		kMainController->MenuCommand(Controller::DELETE_SELECTED);
		kMainController->CharacterInput(ev->character_code_);
	}
	if (ev->character_code_ == 37 || ev->character_code_ == 39) {
		kMainController->CharacterInput(ev->character_code_);
	}
	if (ev->character_code_ == 38 || ev->character_code_ == 40) {
		kMainController->CharacterInput(ev->character_code_);
	}
	if (ev->character_code_ == 0x1B) //esc button
	{
		//DrawTool *dt = kMainController->GetDrawTool();
		//if(dt)
		//	dt->init();
	}

	if (ev->get_key_state().get_ctrl_pressed())
	{
		if (ev->character_code_ == L'Z' || ev->character_code_ == L'z')
		{
			kMainController->MenuCommand(Controller::UNDO);
		}
		else if (ev->character_code_ == L'Y' || ev->character_code_ == L'y')
		{
			kMainController->MenuCommand(Controller::REDO);
		}
		else if (ev->character_code_ == L'C' || ev->character_code_ == L'c')
		{
			kMainController->MenuCommand(Controller::COPY);
		}
		else if (ev->character_code_ == L'X' || ev->character_code_ == L'x')
		{
			kMainController->MenuCommand(Controller::CUT);
		}
		else if (ev->character_code_ == L'V' || ev->character_code_ == L'v')
		{
			kMainController->MenuCommand(Controller::PASTE);
		}
	}
	*/
	set_need_redraw();
	
}

void View3D::KeyboardUpEventProc(graphics::Event *ev)
{
	set_need_redraw();
}

void View3D::CharacterInputEventProc(graphics::Event *ev)
{
	kMainController->CharacterInput(ev->character_code_);
}

void View3D::FitViewToBoundingBox(const box3& box)
{
	camera_.FitViewToBoundingBox(box);
}

void View3D::SetTopView(const box3& box)
{
	camera_.SetTopView(box);
	camera_.update_center_of_look();
}

void View3D::Update()
{
	kMainController->Update();
}

void View3D::WheelEventProc(int zdelta, const ivec2& pos)
{
	camera_.focusZoomCamera(zdelta, pos);

	kMainController->GenCGH();

	set_need_redraw();
	
	kMainWindow->centralWidget()->activateWindow();

	LOG("State IDLE\n");
	kMainWindow->statusBar()->showMessage("State IDLE");
	
}

void View3D::DoubleClickEventProc(const ivec2& p, int shift, int cont, int alt)
{
}

void View3D::MoveEventProc(const ivec2& p, int shift, int cont, int alt)
{
	switch (state) {
	case VIEW_TRANSLATE:
		camera_.panCamera(pre_p, p);
		set_need_redraw();
		break;

	case VIEW_ROTATE:
		camera_.rotateCamera(pre_p, p);
		set_need_redraw();
		break;

	default:
		break;
	} 

	pre_p = p;
}


void View3D::RightDownEventProc(const ivec2& p, int shift, int cont, int alt)
{
	org_p = pre_p = p;
	if (state != VIEW_IDLE) return;

	int key_state;
	switch (key_state = MakeKeyState(shift, cont, alt)) {
	case ks_no:
		state = VIEW_ROTATE;
		break;
	case ks_cont:
	default:
		break;

	} 
}

void View3D::DownEventProc(const ivec2& p, int shift, int cont, int alt)
{
	org_p = pre_p = p;

	int key_state;
	switch (key_state = MakeKeyState(shift, cont, alt)) {
	case ks_cont:

		state = VIEW_TRANSLATE;
		break;

	case ks_alt:
	case ks_shift:
	case ks_no:
	default:
		state = VIEW_IDLE;
		break;
	}
}

void View3D::MiddleDownEventProc(const ivec2& p, int shift, int cont, int alt)
{
}

void View3D::UpEventProc(const ivec2& p, int shift, int cont, int alt)
{
	switch (state) {
	case VIEW_TRANSLATE:
	case VIEW_ROTATE:
		kMainController->GenCGH();
		break;
	default:
		break;
	}

	camera_.update_center_of_look();
  
	state = VIEW_IDLE;

	kMainWindow->centralWidget()->activateWindow();

	LOG("State IDLE\n");
	kMainWindow->statusBar()->showMessage("State IDLE");


}



void View3D::DrawAxes() const
{
	if (!draw_axes_) return;

	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glLineWidth(1.0);

	glDisable(GL_LINE_STIPPLE);

	glBegin(GL_LINES);
	gl_color(vec3(1.0, 0.0, 0.0));
	gl_vertex(vec3(1.0e+15, 0, 0));
	gl_vertex(vec3(0.0, 0, 0));

	gl_color(vec3(0.0, 1.0, 0.0));
	gl_vertex(vec3(0, 1.0e+15, 0));
	gl_vertex(vec3(0, 0.0, 0));

	gl_color(vec3(0.0, 0.0, 1.0));
	gl_vertex(vec3(0, 0, 1.0e+15));
	gl_vertex(vec3(0, 0, 0.0));
	glEnd();

	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0xAAAA);

	glBegin(GL_LINES);
	gl_color(vec3(1.0, 0.0, 0.0));

	gl_vertex(vec3(-1.0e+15, 0, 0));
	gl_vertex(vec3(0.0, 0, 0));

	gl_color(vec3(0.0, 1.0, 0.0));

	gl_vertex(vec3(0, -1.0e+15, 0));
	gl_vertex(vec3(0, 0.0, 0));

	gl_color(vec3(0.0, 0.0, 1.0));

	gl_vertex(vec3(0, 0, -1.0e+15));
	gl_vertex(vec3(0, 0, 0.0));
	glEnd();

	glDisable(GL_LINE_STIPPLE);

	glPopAttrib();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDisable(GL_POLYGON_OFFSET_FILL);
}
