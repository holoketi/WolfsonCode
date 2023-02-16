#pragma once

#include	"view_controller/View3D.h"
#include	<graphics/ivec.h>
#include	<graphics/vec.h>

#include	<QtCore/QString>

using namespace graphics;

class Controller {

public:
	enum ViewMode {
		single_view_,
		multi_view_
	};
	enum State {
		s_no_tool,
		s_create_bezier_path,
		s_camera_rot_tool,
		s_camera_zoom_tool,
		s_camera_tran_tool,
		s_select_tool,
		s_path_edit,
		s_add_sculpt_point_tool,
		s_edit_sculpt_point_tool,
		s_create_rect,
		s_create_circle
	};

	enum Command {
		DELETE_SELECTED,
		UNDO,
		REDO,
		COPY,
		CUT,
		PASTE,
		SELECT,
		CREATE_BEZIER_PATH,
		EDIT_PATH,
		ADD_SCULPT_POINT,
		EDIT_SCULPT_POINT,
		APPEND_SCULPT_NODE,
		APPEND_SCULPT_WITH_CURVE_NODE,
		CREATE_RECT,
		CREATE_CIRCLE
	};

	void DrawTools();
	void Draw2D();
	void Draw();

	void MenuCommand(Command cmd);
	void MouseLeftDown(const ivec2& wp);
	void MouseMove(const ivec2& wp);
	void MouseUp(const ivec2& wp);
	void MouseDoubleClick(const ivec2& wp);

	// return true for any action, false for no action
	bool MouseScroll(int deltaz, const ivec2& wp);
	bool MouseRightDown(const ivec2& wp);

	void CharacterInput(unsigned int input);

	void InitializeInteraction(View3D *viewer);
	void InitializeInteractiveModelingSession(View3D *viewer);
	void InitializeModeler();

	void Update();

public:

	void FitView() const;

	Controller();

	void OpenModel(QString fname);
	void OpenCollada(QString fname);
	void GenCGH();	
	void Recon();


private:

	void menu_cmd_(Command cmd);

	State			main_state_;
	View3D*			main_viewer_;
	real			z_trans_;
	real			x_angle_;

	int             view_mode_;

	bool			bGenCGH_;

};


extern Controller *kMainController;
void UpdateAllView();
