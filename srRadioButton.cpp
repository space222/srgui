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

void srRadioButton::raiseClickEvent(const srEventInfo& ei)
{
	isChecked = true;
	if( r_group ) r_group->activateButton(this);
	return;
}

void srRadioButton::setChecked(bool v)
{
	if( isChecked == v ) return;
	isChecked = v;
	if( parent ) 
	{
		srControl* c = parent;
		while( c->getParent() ) c = c->getParent();
		srWindow* win = dynamic_cast<srWindow*>(c);
		if( win ) win->setDirty();
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
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	//surf->outlineRectangle(newarea);

	int h = area.height/2;

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawCircle({newarea.x+h, newarea.y+h}, h - 3);
	surf->setColor(0);
	surf->outlineCircle({newarea.x+h, newarea.y+h}, h - 3, 1);

	if( isChecked )
	{
		surf->drawCircle({newarea.x+h, newarea.y+h}, h/2 - 2);
	}

	surf->setColor(srgui_data.UIStyle.buttonBackground);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(newarea.x+20), (int)(newarea.y)}, text_layout);

	return;
}

void srRadioButton::setGroup(srRadioGroup* rg)
{
	r_group = rg;
	return;
}

} //end of srgui namespace


