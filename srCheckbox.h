#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srCheckbox : public srControl, public srIEvent
{
public:
	srCheckbox() : isChecked(false), text_layout(nullptr) { flags = SR_CF_REPAINT_ON_LEFT_CLICK | SR_CF_REPAINT_ON_LBUTTON_STATE; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_CHECKBOX; }

	virtual void setText(const std::string& str);

	virtual void raiseClickEvent(const srEventInfo& ei) override { isChecked = !isChecked; return; }

	virtual bool checked() { return isChecked; }
	virtual void setChecked(bool v)
	{
		if( isChecked == v ) return;
		isChecked = v;
		if( !parent ) return;

		srControl* c = c->getParent();
		while( c->getParent() ) c = c->getParent();

		srWindow* win = dynamic_cast<srWindow*>(c);
		if( win ) win->setDirty();

		return;
	}

protected:
	bool isChecked;
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
