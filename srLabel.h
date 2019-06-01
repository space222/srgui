#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srLabel : public srControl
{
public:
	srLabel() : text_layout(nullptr) { flags = 0; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_LABEL; }

	virtual void setText(const std::string& str);

protected:
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
