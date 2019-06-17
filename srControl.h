#pragma once
#include <cstdint>
#include "srgui.h"

namespace srgui {

class srControl;
class srDrawSurface;

class srContainer
{
public:
	virtual srControl* getChild(uint32_t index) =0;
	virtual bool point_in_child(uint32_t index, const srPoint& p) =0;
	virtual void addChild(srControl*) =0;
	virtual uint32_t getNumChildren() =0;
};

class srIOverlay
{
public:
	virtual srDrawSurface* getSurface() =0;
	virtual bool getDirty() =0;
	virtual void clearDirty() =0;
	virtual void setDirty() =0;
};

struct srEventInfo
{
	/* mouse is absolute everywhere but raiseMouseMoveEvent, which sends relative movement */
	srPoint mouse;
	uint32_t mods, key;
	int mouse_button;
};

/* srIEvent: some event functions do not actually recv. any data
 * in srEventInfo, but take one anyway for consistency and future
 * use. (Focus events, specifically, currently)
 */
class srIEvent
{
public:
	virtual void raiseClickEvent(const srEventInfo&) {}
	virtual void raiseKeyPressEvent(const srEventInfo&) {}
	virtual void raiseGainFocusEvent(const srEventInfo&) {}
	virtual void raiseLoseFocusEvent(const srEventInfo&) {}
	virtual void raiseMouseDownEvent(const srEventInfo&) {}
	virtual void raiseMouseUpEvent(const srEventInfo&) {}
	virtual void raiseMouseMoveEvent(const srEventInfo&) {}
};


class srControl
{
public:
	srControl() : parent(nullptr), visible(true), enabled(true) { }
	virtual ~srControl() { }

	virtual uint64_t getFlags() { return flags; }
	virtual void setFlags(uint64_t f) { flags = f; return; }
	virtual void setArea(const srRect& r);
	virtual void getArea(srRect& r) { r = area; return; }
	virtual void setSize(int width, int height);

	virtual void setVisible(bool b);
	virtual bool getVisible() { return visible; }

	virtual void draw(const srDrawInfo&) =0;
	virtual srControlType type() =0;

	virtual srControl* getParent() { return parent; }
	virtual srControl* getToplevelParent();
	virtual void setParent(srControl* c) { parent = c; }
	
	friend class srWindow;
	friend void generateDrawList(std::vector<srRenderTask>& tasks);
	friend void SendEvent(srEventType event, int data0, int data1, int data2, int data3);
protected:
	bool visible, enabled;
	srRect area;
	srControl* parent;
	uint64_t flags;
};


} //end of srgui namespace


