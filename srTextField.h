#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srTextField : public srControl, public srIEvent
{
public:
	srTextField() : cursor_pos(0), text(), text_layout(nullptr) { flags = SR_CF_REPAINT_WHILE_FOCUS; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_TEXTFIELD; }

	virtual void setText(const std::string& str);
	virtual std::string getText() { return text; }

	virtual void setFlags(uint32_t f) override { flags = (flags&~0xFFFFFFFFULL) | f; return; }

	virtual void raiseKeyPressEvent(const srEventInfo& info) override;
	virtual void raiseClickEvent(const srEventInfo& info) override;

protected:
	int cursor_pos;
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
