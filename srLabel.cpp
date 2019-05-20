#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srLabel.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srLabel::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x,area.y} + info.parent_offset;
	
	srRect r;
	text_layout->getExtents(r);
	surf->setColor(srgui_data.UIStyle.textColor);
	surf->drawTextLayout(origin, text_layout);

	return;
}

void srLabel::setText(const std::string& s)
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


