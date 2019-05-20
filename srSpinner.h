#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srLabel.h"
#include "srButton.h"

namespace srgui {

class srSpinner : public srControl, public srContainer
{
public:
	srSpinner(const srRect&);
	~srSpinner() { delete text; delete up; delete down; }

	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_SPINNER; }

	virtual srControl* getChild(uint32_t i) override
	{
		switch( i )
		{
		case 0: return text;
		case 1: return up;
		}
		return down;
	}

	virtual uint32_t getNumChildren() override { return 3; }
	virtual void addChild(srControl* c) override {}

	virtual bool point_in_child(uint32_t index, const srPoint& p) override;

	virtual void setFlags(uint32_t f) override { flags = (flags&~0xFFFFFFFFULL) | f; return; }

protected:
	int value;
	srLabel* text;
	srButton *up, *down;
};


} //end of srgui namespace
