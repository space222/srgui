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

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawRectangle({area.x, area.y, 15, 15});

	if( isChecked )
	{
		surf->setColor(0);
		surf->drawLine(area.x+11, area.y+2, area.x+8, area.y+13);
		surf->drawLine(area.x+8, area.y+13, area.x+3, area.y+7);
	}

	surf->setColor(srgui_data.UIStyle.buttonBackground);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(area.x+20), (int)(area.y)}, text_layout);

	return;
}


} //end of srgui namespace


