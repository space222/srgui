#pragma once
#include <string>
#include <chrono>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srTextField : public srControl, public srIEvent
{
public:
	srTextField() : display_cursor(true), blink_on(true), 
		cursor_last_change(std::chrono::system_clock::now()), blink_rate(1000), cursor_pos(0), 
		text(), text_layout(nullptr) { flags = SR_CF_REPAINT_WHILE_FOCUS; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_TEXTFIELD; }

	virtual void setText(const std::string& str);
	virtual std::string getText() { return text; }

	virtual void setBlinkRate(int br) { blink_rate = br; return; }

	virtual void setFlags(uint32_t f) override { flags = (flags&~0xFFFFFFFFULL) | f; return; }

	virtual void raiseKeyPressEvent(const srEventInfo& info) override;
	virtual void raiseClickEvent(const srEventInfo& info) override;

protected:
	bool display_cursor, blink_on;
	std::chrono::system_clock::time_point cursor_last_change;
	int blink_rate;
	int cursor_pos;
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
