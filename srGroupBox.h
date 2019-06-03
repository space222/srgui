#pragma once
#include <string>
#include <vector>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srGroupBox : public srControl, public srContainer
{
public:
	srGroupBox() : text_layout(nullptr) { flags = 0; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_GROUPBOX; }

	virtual void setText(const std::string& str);

	virtual srControl* getChild(uint32_t index) override
	{
		if( index >= children.size() ) return nullptr;
		return children[index];
	}

	virtual void addChild(srControl* c) override
	{
		c->setParent(this);
		children.push_back(c);
		return;
	}

	virtual uint32_t getNumChildren() override { return children.size(); }

	virtual bool point_in_child(uint32_t index, const srPoint& p) override
	{
		if( index >= children.size() ) return false;

		srRect r;
		children[index]->getArea(r);
		return point_in_rect(r, p);
	}

protected:
	std::vector<srControl*> children;
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
