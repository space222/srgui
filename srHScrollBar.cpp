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
		: value(50), page_size(10), max(200), thumb_loc(20), thumb_size(5), in_thumb_move(false)
{
	area = a;
	flags = SR_CF_REPAINT_ON_LBUTTON_STATE | SR_CF_REPAINT_ON_MOUSE_MOVE;
	left = new srButton("\u2BC7", { 0, 0 });
	left->setSize(20, area.height);
	right = new srButton("\u2BC8", { area.width - 20, 0 });
	right->setSize(20, area.height);
	left->onClick = [&]{ value -= (int)(max*0.02f); 
		if( value <= 0 )  
		{
			value = 0;
		}
		calc_thumb_pos();
		return; 
	};

	right->onClick = [&]{ value += (int)(max*0.02f);
		if( value > max-page_size )
		{
			value = max-page_size;
		}
		calc_thumb_pos();
		return; 
	};

	setScrollInfo(max, page_size);
	return;
}

void srHScrollBar::raiseMouseMoveEvent(const srEventInfo& ev)
{
	if( ! in_thumb_move ) return;

	thumb_loc += ev.mouse.x;
	if( thumb_loc < 20 ) thumb_loc = 20;
	else if( thumb_loc > area.width-22-thumb_size ) thumb_loc = area.width-22-thumb_size;
	float pos_pcent = (thumb_loc-20) / (float)(area.width-42-thumb_size);
	value = (int) ( pos_pcent * (max-page_size) );
	//std::printf("mx = %i, tl = %i, v = %i\n", ev.mouse.x, thumb_loc, value);
	
	srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());
	if( win ) win->setDirty();

	if( onScroll ) onScroll();

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
			if( onScroll ) onScroll();
			calc_thumb_pos();
		} else if( ev.mouse.x > (thumb_loc + thumb_size) ) {
			value += page_size;
			if( value > max-page_size ) value = max-page_size;
			if( onScroll ) onScroll();
			calc_thumb_pos();
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

	surf->setColor(srgui_data.UIStyle.windowBackground);

	int xval = thumb_loc + newarea.x;

	surf->drawRectangle({ xval, newarea.y, thumb_size, area.height });
	surf->setColor(0xFFFFFF);
	surf->drawLine( xval+1, newarea.y+1, xval+thumb_size, newarea.y+1, 1 );
	surf->drawLine( xval+1, newarea.y+1, xval+1,  newarea.y+area.height-1, 1 );
	surf->setColor(0);
	surf->drawLine( xval, newarea.y+newarea.height, xval+thumb_size, newarea.y+newarea.height, 1);
	surf->drawLine( xval+thumb_size, newarea.y, xval+thumb_size, newarea.y+newarea.height, 1);

	return;
}

void srHScrollBar::setScrollInfo(int Max, int Page_sz)
{
	if( Max < 0 || Page_sz < 0 ) return;
	max = Max;
	page_size = Page_sz;
	value = 0;

	thumb_size = (page_size / (float)max) * (area.width-40);
	if( thumb_size < 5 ) thumb_size = 5;

	srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());	
	if( win ) win->setDirty();
	return;
}

void srHScrollBar::calc_thumb_pos()
{
	thumb_loc = 20 + (int) ( ((value)/(float)(max-page_size)) * (area.width-42-thumb_size) );
	//std::printf("value = %i\n", value);
	return;
}

} //end of srgui namespace


