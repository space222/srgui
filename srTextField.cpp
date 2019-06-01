#include <string>
#include <chrono>
#include <cairo.h>
#include <iostream>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srTextField.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srTextField::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x,area.y} + info.parent_offset;
	srRect nar{ origin.x, origin.y, area.width, area.height };

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawRectangle(nar);

	surf->setColor(0);
	surf->drawLine(origin.x, origin.y, origin.x+nar.width, origin.y, 1);
	surf->drawLine(origin.x, origin.y, origin.x, origin.y+nar.height, 1);

	srRect text_rect, cursor_extents;
	text_layout->getExtents(text_rect);
	surf->setColor(srgui_data.UIStyle.textColor);
	text_layout->getCursorPos(cursor_pos, cursor_extents);
	surf->drawTextLayout(origin + srPoint{(int)(4+scroll_offset),4}, text_layout);

	std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
	if( (t - cursor_last_change) > std::chrono::milliseconds(blink_rate) )
	{
		cursor_last_change = t;
		blink_on = !blink_on;
	}

	if( display_cursor && blink_on && (info.flags & SR_DIF_FOCUS) )
	{
		surf->drawLine(origin.x+(cursor_extents.x+scroll_offset)+4, origin.y+cursor_extents.y+4, 
			       origin.x+(cursor_extents.x+scroll_offset)+4, origin.y+cursor_extents.y+4+cursor_extents.height);
	}
	return;
}

void srTextField::raiseKeyPressEvent(const srEventInfo& info)
{
	blink_on = true;
	srRect text_rect, cursor_extents;
	
	if( info.key )
	{
		text.insert(cursor_pos, 1, (char)info.key);
		cursor_pos++;

		text_layout->setText(text);
		text_layout->getExtents(text_rect);
		text_layout->getCursorPos(cursor_pos, cursor_extents);

		if( scroll_offset + cursor_extents.x >= area.width )
		{
			scroll_offset = -(cursor_extents.x - area.width + 6);
		}

	} else if( (info.mods & (1<<17)) && text.size() && cursor_pos > 0 ) {
		text.erase(cursor_pos-1, 1);
		cursor_pos--;

		text_layout->setText(text);
		text_layout->getExtents(text_rect);
		text_layout->getCursorPos(cursor_pos, cursor_extents);

		if( cursor_extents.x <= -scroll_offset )
		{
			scroll_offset = -(cursor_extents.x);
		}


	} else if( (info.mods & (1<<16)) && text.size() ) {
		text.erase(cursor_pos, 1);
		text_layout->setText(text);
	}


	return;
}

void srTextField::raiseClickEvent(const srEventInfo& info)
{
	srPoint p = info.mouse;
	p = p - srPoint{ 4,4 };
	p = p + srPoint{ -scroll_offset, 0 };
	int ndex = -1;
	//std::printf("clicked at {%i, %i}\n", p.x, p.y);
	text_layout->getIndexFromPos(p, ndex);
	cursor_pos = ndex;

	blink_on = true;
	return;
}

void srTextField::setText(const std::string& s)
{
	text = s;
	if( ! text_layout ) 
	{
		text_layout = new srPangoTextLayout;
		//text_layout->setFont("Sans 15");
	}
	cursor_pos = scroll_offset = 0;
	text_layout->setText(text);
	return;
}

} //end of srgui namespace


