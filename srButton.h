#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srButton : public srControl, public srIEvent
{
public:
	srButton() { flags = SR_CF_REPAINT_ON_HOVER | SR_CF_REPAINT_ON_LBUTTON_STATE; }
	virtual void draw(const srDrawInfo&) override;
	virtual void setArea(const srRect& r) override;
	virtual void getArea(srRect& r) override { r = area; return; }
	virtual srControlType type() override { return SR_CT_BUTTON; }

	virtual void setText(const std::string& str);

	virtual void setFlags(uint32_t f) override { flags = (flags&~0xFFFFFFFFULL) | f; return; }

	virtual void raiseClickEvent() override { if(onClick) onClick(); return; }
	std::function<void()> onClick;

protected:
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
