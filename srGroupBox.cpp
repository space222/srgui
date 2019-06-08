#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srGroupBox.h"

namespace srgui {

extern srgui::system_data srgui_data;

srGroupBox::srGroupBox(const std::string& s, const srRect& r) : srGroupBox()
{
	setText(s);
	area = r;
	return;
}

void srGroupBox::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x,area.y} + info.parent_offset;
	
	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawLine(origin.x, origin.y+(r.height/2), origin.x+area.width, origin.y+(r.height/2), 1);
	surf->drawLine(origin.x, origin.y+area.height-(r.height/2), origin.x+area.width, origin.y+area.height-(r.height/2), 1);
	surf->drawLine(origin.x, origin.y+(r.height/2), origin.x, origin.y+area.height-(r.height/2), 1);
	surf->drawLine(origin.x+area.width-1.5f, origin.y+(r.height/2), origin.x+area.width-1.5f, origin.y+area.height-(r.height/2), 1);
	surf->setColor(0xFFFFFF);
	surf->drawLine(origin.x, origin.y+(r.height/2)+1, origin.x+area.width, origin.y+(r.height/2)+1, 1);
	surf->drawLine(origin.x, origin.y+area.height-(r.height/2)+1, origin.x+area.width, origin.y+area.height-(r.height/2)+1, 1);
	surf->drawLine(origin.x+1.5f, origin.y+(r.height/2), origin.x+1.5f, origin.y+area.height-(r.height/2), 1);
	surf->drawLine(origin.x+area.width, origin.y+(r.height/2), origin.x+area.width, origin.y+area.height-(r.height/2), 1);
	
	surf->setColor(srgui_data.UIStyle.windowBackground);
	surf->drawRectangle({origin.x+10, origin.y, r.width, r.height});
	surf->setColor(srgui_data.UIStyle.textColor);
	surf->drawTextLayout(origin + srPoint{ 10, 0 } , text_layout);

	return;
}

void srGroupBox::setText(const std::string& s)
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


