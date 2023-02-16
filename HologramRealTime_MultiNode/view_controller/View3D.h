#pragma once

#include	<windows.h>
#include    <stdio.h>

#include	"gl/gl.h"
#include	"gl/glext.h"

#include	<graphics/shadow_map.h>
#include	<graphics/sys.h>
#include	<graphics/event.h>
#include    <graphics/vec.h>
#include    <graphics/ivec.h>
#include    <graphics/geom.h>
#include    <graphics/quater.h>
#include    <graphics/sys.h>
#include    <graphics/real.h>
#include    <graphics/frame.h>
#include    <graphics/Camera.h>

#include	"view_controller/View.h"

using namespace graphics;

class View3D : public View {

public:

	enum State {
		VIEW_IDLE,
		VIEW_TRANSLATE,
		VIEW_ROTATE,
		VIEW_ZOOM,
		VIEW_SELECT,
		VIEW_REGION_SELECT,
	};


	View3D(int w = 100, int h = 100);

	virtual void Initialize();

	virtual void Render();

	void FitViewToBoundingBox(const box3& box);

	void SetTopView(const box3& box);

	void set_draw_axes(bool val = true) { draw_axes_ = val; }

	bool draw_axes() const { return draw_axes_; }

	bool isIdleState() { return (state == VIEW_IDLE); }
	

	virtual void Update();

private:

	void	DrawAxes() const;


protected:

	virtual void WheelEventProc(int zdelta, const ivec2& pos);

	virtual void MoveEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void DownEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void RightDownEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void UpEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void MiddleDownEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void DoubleClickEventProc(const ivec2& p, int shift, int cont, int alt);

	virtual void CharacterInputEventProc(graphics::Event*);

	virtual void KeyboardDownEventProc(graphics::Event*);

	virtual void KeyboardUpEventProc(graphics::Event*);

	virtual void Render2D();
	   
private:

	static bool first;				// first instance?

	State		state;				// state of mouse action

	bool		draw_axes_;
};
