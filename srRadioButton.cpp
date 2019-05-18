#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srRadioButton.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srRadioGroup::add(srRadioButton* b)
{
	for(auto i = begin(buttons); i != end(buttons); ++i)
	{
		if( b == *i ) return;
	}

	b->setGroup(this);
	buttons.push_back(b);
	return;
}

void srRadioGroup::remove(srRadioButton* b)
{
	for(auto i = begin(buttons); i != end(buttons); ++i)
	{
		if( b == *i )
		{
			buttons.erase(i);
			return;
		}
	}

	return;
}

void srRadioGroup::activateButton(srRadioButton* b)
{
	for(auto i = begin(buttons); i != end(buttons); ++i)
	{
		if( b == *i ) continue;
		(*i)->setChecked(false);
	}
	return;
}

void srRadioButton::raiseClickEvent()
{
	isChecked = true;
	if( r_group) r_group->activateButton(this);
	return;
}

void srRadioButton::setArea(const srRect& r)
{
	area = r;
	srControl* c = parent;
	if( c )
	{
		while( c->getParent() ) c = c->getParent();
		((srWindow*)c)->setDirty();
	}
	return;
}

void srRadioButton::setText(const std::string& s)
{
	text = s;
	if( ! text_layout ) 
	{
		text_layout = new srPangoTextLayout;
		//text_layout->setFont("Sans 15");
	}
	text_layout->setText(text);
	return;
}

void srRadioButton::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawCircle({area.x+5, area.y+5}, 10);
	surf->setColor(0);
	surf->outlineCircle({area.x+5, area.y+5}, 10);
		
	if( isChecked )
	{
		surf->drawCircle({area.x+5, area.y+5}, 6);
	}

	surf->setColor(srgui_data.UIStyle.buttonBackground);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(area.x+20), (int)(area.y)}, text_layout);

	return;
}

void srRadioButton::setGroup(srRadioGroup* rg)
{
	r_group = rg;
	return;
}

} //end of srgui namespace


