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
#include "srHScrollBar.h"

namespace srgui {

extern srgui::system_data srgui_data;

bool srHScrollBar::point_in_child(uint32_t index, const srPoint& p)
{
	if( index == 0 )
	{
		srRect r;
		left->getArea(r);
		return point_in_rect(r, p);
	}
	if( index == 1 )
	{
		srRect r;
		right->getArea(r);
		return point_in_rect(r, p);
	}
	return false;
}


srHScrollBar::srHScrollBar(const srRect& a) 
		: value(50), page_size(10), max(200), thumb_loc(0), thumb_size(5), in_thumb_move(false)
{
	area = a;
	flags = SR_CF_REPAINT_ON_LBUTTON_STATE | SR_CF_REPAINT_ON_MOUSE_MOVE;
	left = new srButton("\u2BC7", { 0, 0 });
	left->setSize(20, area.height);
	right = new srButton("\u2BC8", { (int)area.width - 20, 0 });
	right->setSize(20, area.height);
	left->onClick = [&]{ value -= page_size; 
		if( value <= 0 )  
		{
			value = 0;
		}
		return; 
	};

	right->onClick = [&]{ value += page_size;
		if( value > max )
		{
			value = max;
		}
		return; 
	};

	return;
}

void srHScrollBar::raiseMouseMoveEvent(const srEventInfo& ev)
{
	if( ! in_thumb_move ) return;

	value += ev.mouse.x<<1;  /* I have no idea why the exact relative amount is too slow, but *2 feels good */
	if( value > max ) value = max;
	else if( value < 0 ) value = 0;

	return;
}

void srHScrollBar::raiseMouseDownEvent(const srEventInfo& ev)
{
	in_thumb_move = ( ev.mouse.x > thumb_loc && ev.mouse.x < (thumb_loc + thumb_size) );
	return;
}

void srHScrollBar::raiseMouseUpEvent(const srEventInfo& ev)
{
	if( ! in_thumb_move )
	{
		//std::puts("page jump!");
		if( ev.mouse.x < thumb_loc )
		{
			value -= page_size;
			if( value < 0 ) value = 0;
		} else if( ev.mouse.x > (thumb_loc + thumb_size) ) {
			value += page_size;
			if( value > max ) value = max;
		}
	}

	in_thumb_move = false;
	return;
}

void srHScrollBar::raiseClickEvent(const srEventInfo& ev)
{

	return;
}

void srHScrollBar::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };

	surf->setColor(0x808080);
	surf->drawRectangle(newarea);

	int ts = (page_size / (float)max) * (newarea.width-40);
	if( ts < 5 ) ts = 5;

	surf->setColor(srgui_data.UIStyle.windowBackground);

	int xval = 21 + (int)((value / (float)max)*(area.width-42-ts)) + newarea.x;

	surf->drawRectangle({ xval, newarea.y, (uint32_t)ts, area.height });
	surf->setColor(0xFFFFFF);
	surf->drawLine( xval+1, newarea.y+1, xval+ts, newarea.y+1, 1 );
	surf->drawLine( xval+1, newarea.y+1, xval+1,  newarea.y+area.height-1, 1 );
	surf->setColor(0);
	surf->drawLine( xval, newarea.y+newarea.height, xval+ts, newarea.y+newarea.height, 1);
	surf->drawLine( xval+ts, newarea.y, xval+ts, newarea.y+newarea.height, 1);

	thumb_loc = xval - newarea.x;
	thumb_size = ts;
	return;
}

} //end of srgui namespace


