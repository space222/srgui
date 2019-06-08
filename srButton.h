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
	srButton() : text_layout(nullptr) { flags = /* SR_CF_REPAINT_ON_HOVER | */ SR_CF_REPAINT_ON_LBUTTON_STATE; }
	virtual ~srButton() { delete text_layout; }

	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_BUTTON; }

	virtual void setText(const std::string& str);

	virtual void raiseClickEvent(const srEventInfo& ei) override { if(onClick) onClick(); return; }
	std::function<void()> onClick;

protected:
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
