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
#include "srVScrollBar.h"

namespace srgui {

extern srgui::system_data srgui_data;

bool srVScrollBar::point_in_child(uint32_t index, const srPoint& p)
{
	if( index == 0 )
	{
		srRect r;
		up->getArea(r);
		return point_in_rect(r, p);
	}
	if( index == 1 )
	{
		srRect r;
		down->getArea(r);
		return point_in_rect(r, p);
	}
	return false;
}


srVScrollBar::srVScrollBar(const srRect& a) 
		: value(50), page_size(10), max(200), thumb_loc(20), thumb_size(5), in_thumb_move(false)
{
	area = a;
	flags = SR_CF_REPAINT_ON_LBUTTON_STATE | SR_CF_REPAINT_ON_MOUSE_MOVE;
	up = new srButton("\u2BC5", { 0, 0, area.width, 20 });
	down = new srButton("\u2BC6", { 0, area.height-20, area.width, 20 });
	up->onClick = [&]{ value -= (int)(max*0.02f); 
		if( value <= 0 )  
		{
			value = 0;
		}
		calc_thumb_pos();
		if( onScroll ) onScroll();
		return; 
	};

	down->onClick = [&]{ value += (int)(max*0.02f);
		if( value > max-page_size )
		{
			value = max-page_size;
		}
		calc_thumb_pos();
		if( onScroll ) onScroll();
		return; 
	};

	setScrollInfo(max, page_size);
	return;
}

void srVScrollBar::raiseMouseMoveEvent(const srEventInfo& ev)
{
	if( ! in_thumb_move ) return;

	thumb_loc += ev.mouse.y;
	if( thumb_loc < 20 ) thumb_loc = 20;
	else if( thumb_loc > area.height-22-thumb_size ) thumb_loc = area.height-22-thumb_size;
	float pos_pcent = (thumb_loc-20) / (float)(area.height-42-thumb_size);
	value = (int) ( pos_pcent * (max-page_size) );
	//std::printf("mx = %i, tl = %i, v = %i\n", ev.mouse.x, thumb_loc, value);
	
	srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());
	if( win ) win->setDirty();

	if( onScroll ) onScroll();

	return;
}

void srVScrollBar::raiseMouseDownEvent(const srEventInfo& ev)
{
	in_thumb_move = ( ev.mouse.y > thumb_loc && ev.mouse.y < (thumb_loc + thumb_size) );
	return;
}

void srVScrollBar::raiseMouseUpEvent(const srEventInfo& ev)
{
	if( ! in_thumb_move )
	{
		if( ev.mouse.y < thumb_loc )
		{
			value -= page_size;
			if( value < 0 ) value = 0;
			if( onScroll ) onScroll();
			calc_thumb_pos();
		} else if( ev.mouse.y > (thumb_loc + thumb_size) ) {
			value += page_size;
			if( value > max-page_size ) value = max-page_size;
			if( onScroll ) onScroll();
			calc_thumb_pos();
		}
	}

	in_thumb_move = false;
	return;
}

void srVScrollBar::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };

	surf->setColor(0x808080);
	surf->drawRectangle(newarea);

	surf->setColor(srgui_data.UIStyle.windowBackground);

	int yval = thumb_loc + newarea.y;

	surf->drawRectangle({ newarea.x, yval, newarea.width, thumb_size });
	surf->setColor(0xFFFFFF);
	surf->drawLine( newarea.x+1, yval+1, newarea.x+newarea.width, yval+1, 1 );
	surf->drawLine( newarea.x+1, yval+1, newarea.x+1,  yval+thumb_size, 1 );
	surf->setColor(0);
	surf->drawLine( newarea.x, yval+thumb_size, newarea.x+newarea.width, yval+thumb_size, 0.5);
	surf->drawLine( newarea.x+newarea.width, yval, newarea.x+newarea.width, yval+thumb_size, 0.5);

	return;
}

void srVScrollBar::setScrollInfo(int Max, int Page_sz)
{
	if( Max < 0 || Page_sz < 0 ) return;
	max = Max;
	page_size = Page_sz;
	value = 0;

	thumb_size = (page_size / (float)max) * (area.height-40);
	if( thumb_size < 5 ) thumb_size = 5;

	srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());	
	if( win ) win->setDirty();
	return;
}

void srVScrollBar::calc_thumb_pos()
{
	thumb_loc = 20 + (int) ( ((value)/(float)(max-page_size)) * (area.height-42-thumb_size) );
	return;
}

} //end of srgui namespace


