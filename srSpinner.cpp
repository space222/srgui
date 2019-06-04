#include <string>
#include <cairo.h>
#include <cstdio>
#include <pango/pangocairo.h>
#include "srgui.h"
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srButton.h"
#include "srTextField.h"
#include "srSpinner.h"

namespace srgui {

extern srgui::system_data srgui_data;

bool srSpinner::point_in_child(uint32_t index, const srPoint& p)
{
	if( index == 0 )
	{
		srRect text_area;
		text->getArea(text_area);
		return point_in_rect(text_area, p);
	}
	if( index == 1 )
	{
		srRect up_area;
		up->getArea(up_area);
		return point_in_rect(up_area, p);
	}
	srRect down_area;
	down->getArea(down_area);
	return point_in_rect(down_area, p);
}

bool parse_int(const std::string str, int &val)
{
	try {
		size_t pos = 0;
		val = std::stoi(str, &pos);
		return pos == str.size();
	} catch(std::invalid_argument) { /* just don't want to terminate */ }
	return false;
}

srSpinner::srSpinner(const srRect& a)
{
	area = a;
	flags = SR_CF_REPAINT_ON_LBUTTON_STATE | SR_CF_REPAINT_ON_HOVER;
	value = 0;
	text = new srTextField;
	text->setParent(this);
	text->setText("0");
	text->setArea({0, 0, a.width/2, a.height});
	up = new srButton;
	up->setParent(this);
	up->setText("\u2BC5");
	up->setArea({(int)a.width/2, 0, a.width/2, a.height/2});
	down = new srButton;
	down->setParent(this);
	down->setText("\u2BC6");
	down->setArea({(int)a.width/2, (int)a.height/2, a.width/2, a.height/2});
	down->onClick = [&]{ int a = 0; if( parse_int(text->getText(), a) ) value = a; value--; text->setText(std::to_string(value)); };
	up->onClick   = [&]{ int a = 0; if( parse_int(text->getText(), a) ) value = a; value++; text->setText(std::to_string(value)); };
	return;
}

void srSpinner::draw(const srDrawInfo& info)
{
// if a composite control only consists of other controls and no custom extra bits
// it no longer needs to draw its' children as this has been moved into srWindow.
/*
	srDrawSurface* surf = info.surface;

	srDrawInfo newinfo = info;

	newinfo.flags &= ~SR_DIF_MOUSE_OVER;
	uint32_t flags = info.flags;
	newinfo.parent_offset = srPoint{area.x,area.y} + info.parent_offset;
	newinfo.mouse_rel = newinfo.mouse_rel - srPoint{area.x, area.y};
	text->draw(newinfo);

	if( point_in_child(1, newinfo.mouse_rel) )
	{
		newinfo.flags |= (flags&(SR_DIF_MOUSE_LEFT|SR_DIF_MOUSE_OVER));
	} else {
		newinfo.flags &= ~(SR_DIF_MOUSE_LEFT|SR_DIF_MOUSE_OVER);
	}
	
	up->draw(newinfo);

	if( point_in_child(2, newinfo.mouse_rel) )
	{
		newinfo.flags |= (flags&(SR_DIF_MOUSE_LEFT|SR_DIF_MOUSE_OVER));
	} else {
		newinfo.flags &= ~(SR_DIF_MOUSE_LEFT|SR_DIF_MOUSE_OVER);
	}	
	down->draw(newinfo);
*/
	return;
}

} //end of srgui namespace


