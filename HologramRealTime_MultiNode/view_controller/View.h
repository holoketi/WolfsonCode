#pragma once

#include	<Windows.h>
#include    <stdio.h>
#include	<graphics/shadow_map.h>
#include	<graphics/event.h>
#include    <graphics/vec.h>
#include    <graphics/ivec.h>
#include    <graphics/geom.h>
#include    <graphics/quater.h>
#include    <graphics/real.h>
#include    <graphics/frame.h>
#include    <graphics/Camera.h>

using namespace graphics;

class View {

public:
	View(int w = 100, int h = 100);

	virtual void Initialize();

	void set_need_redraw(bool val = true);

	bool need_redraw() const;

	void clear_need_redraw();

	virtual void Render();

	virtual void Resize(int w, int h);

	virtual void Resize(int x, int y, int w, int h);

	virtual bool ProcessEvent(Event* ev);
	
	const Camera& GetCamera() const;

	Camera& GetCamera();
	   
	int GetWidth() const;

	int GetHeight() const;

	virtual void Update() {}

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


protected:

	bool		need_redraw_;

	Camera		camera_;			// opengl viewer : angle, far distance, camera, etc

	ivec2		org_p, pre_p;
};