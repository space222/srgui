#include <string>
#include <cairo.h>
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
	
	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawRectangle({origin.x, origin.y, area.width, area.height});

	surf->setColor(0);
	surf->drawLine(origin.x, origin.y, origin.x+area.width, origin.y, 1);
	surf->drawLine(origin.x, origin.y, origin.x, origin.y+area.height, 1);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(srgui_data.UIStyle.textColor);
	surf->drawTextLayout(origin + srPoint{4,4}, text_layout);

	if( info.flags & SR_DIF_FOCUS )
	{
		text_layout->getCursorPos(cursor_pos, r);
		//std::printf("x=%i, y=%i, w=%i, h=%i\n", r.x, r.y, r.width, r.height);
		surf->drawLine(origin.x+r.x+4, origin.y+r.y+4, origin.x+r.x+4, origin.y+r.y+4+r.height);
	}
	return;
}

void srTextField::raiseKeyPressEvent(const srEventInfo& info)
{
	if( info.key )
	{
		text.insert(cursor_pos, 1, (char)info.key);
		cursor_pos++;

	} else if( (info.mods & (1<<17)) && text.size() ) {
		text.erase(cursor_pos-1, 1);
		cursor_pos--;
	} else if( (info.mods & (1<<16)) && text.size() ) {
		text.erase(cursor_pos, 1);
	}

	text_layout->setText(text);
	return;
}

void srTextField::raiseClickEvent(const srEventInfo& info)
{
	srPoint p = info.mouse;
	p = p - srPoint{ 4,4 };
	int ndex = -1;
	//std::printf("clicked at {%i, %i}\n", p.x, p.y);
	text_layout->getIndexFromPos(p, ndex);
	cursor_pos = ndex;
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
	text_layout->setText(text);
	return;
}

} //end of srgui namespace


