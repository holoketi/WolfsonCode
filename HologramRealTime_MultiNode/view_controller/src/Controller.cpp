#include    <graphics/sys.h>
#include	<graphics/string_manip.h>

#include	<algorithm>
#include	<fstream>
#include	<string>

#include    "Controller.h"
#include	"HologramRender.h"
#include	"ModelRender.h"

#include	<IL/IL.h>
#include	<IL/ILU.h>
#include	<IL/ILUT.h>

#include	"model/kernel.h"
#include	"model/ImportedModel.h"
#include	"model/AnimCharacter.h"
#include	"model/ColladaLoad.h"

#include	"HologramInteractive/KFullScreenDialog.h"

#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/QStatusBar>

Controller* kMainController = 0;

model_kernel::ImportedModel* kModel = 0;
model_kernel::AnimCharacter* kCharacter = 0;
HologramRender* kHRender = 0;
ModelRender* kMRender = 0;
KFullScreenDisplayDialog* device_ = 0;

extern QMainWindow* kMainWindow;


Controller::Controller()
{
	view_mode_ = single_view_;
	main_viewer_ = 0;
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	x_angle_ = 0;
	z_trans_ = 0;

	bGenCGH_ = false;
}


void CreateController()
{
	kMainController = new Controller;
}


void initModel()
{

}

void Controller::InitializeModeler()
{
}

void UpdateAllView()
{
}

void InitializeModelingKernel()
{
	__try {
		initKernel();
		initModel();
	}
	__except (MemAccessViolationSignalHandler(GetExceptionInformation())) {
	}
	
	CreateController();

}

void Controller::InitializeInteraction(View3D *viewer)
{
	main_viewer_ = viewer;

	if (kHRender)	delete kHRender;

	kHRender = new HologramRender();
	kHRender->readConfig();
	kHRender->initialize();

	if (!device_) {
		QDesktopWidget* desktop = QApplication::desktop();

		if (desktop->screenCount() > 1) {
			LOG("connected screen %d\n", desktop->screenCount());
			device_ = new KFullScreenDisplayDialog(kMainWindow);
			QRect qrect = desktop->screenGeometry(1);
			device_->setGeometry(qrect);
			device_->showFullScreen();

		}
		else if (desktop->screenCount() == 1) {
			device_ = new KFullScreenDisplayDialog(kMainWindow);
			device_->setGeometry(0, 30, kHRender->getWidth(), kHRender->getHeight());
			device_->show();

		}
	}
		
}



void Controller::Update()
{
	//LOG("update\n");
	main_viewer_->set_need_redraw();
}

void Controller::InitializeInteractiveModelingSession(View3D *viewer)
{
	__try {
		InitializeInteraction(viewer);
	}
	__except (MemAccessViolationSignalHandler(GetExceptionInformation())) {
	}
}

void Controller::OpenModel(QString fname)
{
	if (fname.contains(".dae", Qt::CaseInsensitive))
		OpenCollada(fname);
}

void Controller::OpenCollada(QString fname)
{
	if (!kHRender) {
		LOG("Error:  No Hologram Renderer\n");
		return;
	}

	beginTransaction();
	kModel = new model_kernel::ImportedModel();
	kCharacter = new model_kernel::AnimCharacter();

	model_kernel::ColladaLoad loader(kModel, kCharacter);

	LOG("loading %s file\n", fname.toStdString().c_str());
	loader.Load(fname.toStdWString().c_str());
	endTransaction();

	if (kMRender)	delete kMRender;

	kMRender = new ModelRender();
	kMRender->setModel(kModel);
	kMRender->setCharacter(kCharacter);
	   	 
	kHRender->setModel(kModel);
	kHRender->setCharacter(kCharacter);

	main_viewer_->set_need_redraw();
}

void Controller::GenCGH()
{
	if (!kHRender || !kModel)	return;

	LOG("Generating Hologram\n");
	QStatusBar* msg = kMainWindow->statusBar();
	msg->showMessage("Generating Hologram");

	kHRender->setCamera(main_viewer_->GetCamera());
	kHRender->render();

	bGenCGH_ = true;

	device_->setTexture(kHRender->getBuffer(), kHRender->getWidth(), kHRender->getHeight());

	main_viewer_->set_need_redraw();
}

void Controller::Recon()
{
	if (!kHRender || !kModel || !bGenCGH_)	return;

	kHRender->reconstruct_fromCGH();

}

void Controller::DrawTools()
{

}


void Controller::Draw()
{
	if (!kMRender)	return;

	kMRender->render();

}

void Controller::Draw2D()
{

}

void Controller::FitView() const
{
	//main_viewer_->FitViewToBoundingBox(all_model_->get_bounding_box());
}


void Controller::menu_cmd_(Command cmd)
{
	UpdateAllView();
}

void Controller::MenuCommand(Command cmd)
{
	menu_cmd_(cmd);
}

void Controller::MouseDoubleClick(const ivec2& wp)
{
	//LOG("double click\n");
	//main_viewer_->set_need_redraw();
}

void Controller::MouseLeftDown(const ivec2& wp)
{
	LOG("down\n");

	main_viewer_->set_need_redraw();
}

void Controller::MouseMove(const ivec2& wp)
{

	main_viewer_->set_need_redraw();
}

bool Controller::MouseScroll(int deltaz, const ivec2& wp)
{
	//bool ret = false;
	//if (ret) main_viewer_->set_need_redraw();
	//return ret;
	return true;
}

bool Controller::MouseRightDown(const ivec2& wp)
{
	//bool ret = false;
	//if (ret) main_viewer_->set_need_redraw();
	//return ret;

	main_viewer_->set_need_redraw();
	return true;
}

void Controller::MouseUp(const ivec2& wp)
{
	main_viewer_->set_need_redraw();

}

void Controller::CharacterInput(unsigned int input)
{
	/*
	LOG("character %d\n", input);
	//if (draw_tool_) draw_tool_->CharacterInput(input);
	switch (input) {

	case 37:		//Key_Left 
	{
		if (kRender) kRender->rotPlus();
	}
	break;
	case 39:		//Key_Right
	{
		if (kRender) kRender->rotMinus();
	}
	break;


	case 43:        // + Key
	{
		if (kRender) kRender->makeLarger();
	}
	break;
	case 45:        // - Key
	{
		if (kRender) kRender->makeSmaller();
	}
	break;
	}
	main_viewer_->set_need_redraw();
	*/
}


