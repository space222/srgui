#pragma once
#include <vector>
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srWindow : public srControl, public srContainer
{
public:
	virtual void setTitle(const std::string& ttle) { title = ttle; isdirty = true; }

	virtual void setArea(const srRect& r) override 
	{ 
		if( r.width != area.width || r.height != area.height )
		{
			surface->setSize(r.width, r.height);
			isdirty = true;
		}
		area = r;
		return; 
	}

	virtual void getArea(srRect& r) override { r = area; return; }
	virtual void draw(const srDrawInfo&);

	virtual bool getDirty() { return isdirty; }
	virtual void setDirty() { isdirty = true; }

	virtual bool point_in_child(uint32_t index, const srPoint& p) override 
	{
		if( index >= children.size() ) return false;
		srRect r;
		children[index]->getArea(r);
		srPoint pt{ p.x - area.x, p.y - area.y };
		return point_in_rect( r, pt );
	}

	virtual void addChild(srControl* c) override
	{
		children.push_back(c);
		c->setParent(this);
		isdirty = true;
		return;
	}

	virtual uint32_t getNumChildren() override { return children.size(); }

	virtual srControl* getChild(uint32_t index) override 
	{
		if( index >= children.size() ) 
		{
			return nullptr;
		}
		return children[index];
	}

	virtual srControlType type() override { return SR_CT_WINDOW; }

	virtual void setFlags(uint32_t f) override { flags = (flags&~0xFFFFFFFFULL) | f; isdirty = true; return; }

	friend void SendEvent(srEventType event, int data0, int data1, int data2, int data3);
	friend srWindow* CreateWindow(const srRect&, void*, const std::string&);
	friend void generateDrawList(std::vector<srRenderTask>&);
protected:
	srWindow(int w, int h, srDrawSurface*);
	bool isdirty = true;
	srPangoTextLayout* caption_layout;
	std::vector<srControl*> children;
	std::string title;
	srDrawSurface* surface;
};

} //end of srgui namespace


