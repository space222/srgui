#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srButton.h"
#include "srWindow.h"

namespace srgui {

class srHScrollBar : public srControl, public srContainer, public srIEvent
{
public:
	srHScrollBar(const srRect&);
	virtual ~srHScrollBar() {  }

	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_HSCROLLBAR; }

	virtual srControl* getChild(uint32_t i) override
	{
		if( i == 0 ) return left;
		if( i == 1 ) return right;
		return nullptr;
	}

	virtual uint32_t getNumChildren() override { return 2; }
	virtual void addChild(srControl* c) override {}
	virtual bool point_in_child(uint32_t index, const srPoint& p) override;

	virtual void raiseMouseMoveEvent(const srEventInfo&) override;
	virtual void raiseMouseUpEvent(const srEventInfo&) override;
	virtual void raiseMouseDownEvent(const srEventInfo&) override;
	virtual void raiseClickEvent(const srEventInfo&) override;

	std::function<void()> onScroll;

	virtual int getValue() { return value; }
	virtual void setScrollInfo(int max, int page_sz);

protected:
	void calc_thumb_pos();
	srButton *left, *right;
	int max, page_size;
	int value;
	int thumb_loc, thumb_size;
	bool in_thumb_move;
};


} //end of srgui namespace
