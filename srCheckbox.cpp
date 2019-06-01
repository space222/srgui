#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srCheckbox.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srCheckbox::setText(const std::string& s)
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

void srCheckbox::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };

	//uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
	//				srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawRectangle({newarea.x, newarea.y, 15, 15});

	surf->setColor(0);
	surf->drawLine(newarea.x, newarea.y, newarea.x+15, newarea.y, 1);
	surf->drawLine(newarea.x, newarea.y, newarea.x, newarea.y+15, 1);

	if( isChecked )
	{
		surf->setColor(0);
		surf->drawLine(newarea.x+11, newarea.y+2, newarea.x+8, newarea.y+13);
		surf->drawLine(newarea.x+8, newarea.y+13, newarea.x+3, newarea.y+7);
	}

	surf->setColor(srgui_data.UIStyle.buttonBackground);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(newarea.x+20), (int)(newarea.y)}, text_layout);

	return;
}


} //end of srgui namespace


